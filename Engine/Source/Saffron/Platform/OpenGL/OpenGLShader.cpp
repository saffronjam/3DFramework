#include "SaffronPCH.h"
#include "OpenGLShader.h"

#include "Saffron/Core/Misc.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
#define UNIFORM_LOGGING 0
#if UNIFORM_LOGGING
#define HZ_LOG_UNIFORM(...) SE_CORE_WARN(__VA_ARGS__)
#else
#define HZ_LOG_UNIFORM
#endif

OpenGLShader::OpenGLShader(const Filepath& filepath)
{
	m_Filepath = Move(filepath);

	const auto fpString = m_Filepath.string();

	size_t found = fpString.find_last_of("/\\");
	m_Name = found != std::string::npos ? fpString.substr(found + 1) : fpString;
	found = m_Name.find_last_of('.');
	m_Name = found != std::string::npos ? m_Name.substr(0, found) : m_Name;

	static int i = 0;
	SE_CORE_INFO("Loading no: {0}: filepath: {1}", i++, filepath.string());

	OpenGLShader::Reload();
}

Shared<OpenGLShader> OpenGLShader::Create(const Buffer& source)
{
	Shared<OpenGLShader> shader = Shared<OpenGLShader>::Create();
	shader->Load(source);
	return shader;
}

void OpenGLShader::Reload()
{
	const auto source = ReadShaderFromFile(m_Filepath);
	Load(source);
}

void OpenGLShader::Load(const Buffer& source)
{
	m_ShaderSource = PreProcess(source);
	if (!m_IsCompute) Parse();

	Renderer::Submit([=]()
	{
		static int i = 0;
		SE_CORE_INFO("Submitting no: {0}: ", i++);


		if (m_RendererID)
			glDeleteProgram(m_RendererID);

		CompileAndUploadShader();
		if (!m_IsCompute)
		{
			ResolveUniforms();
			ValidateUniforms();
		}

		if (m_Loaded)
		{
			for (auto& callback : m_ShaderReloadedCallbacks) callback();
		}

		m_Loaded = true;
	});
}

void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
{
	m_ShaderReloadedCallbacks.push_back(callback);
}

void OpenGLShader::Bind()
{
	Renderer::Submit([=]()
	{
		glUseProgram(m_RendererID);
	});
}

Buffer OpenGLShader::ReadShaderFromFile(const Filepath& filepath) const
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in)
	{
		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
	}
	else
	{
		SE_CORE_ASSERT(false, "Could not load shader!");
	}

	in.close();
	return Buffer::Copy(result.c_str(), result.length() * sizeof(String::value_type));
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const Buffer& source)
{
	std::unordered_map<GLenum, std::string> shaderSources;

	String data(reinterpret_cast<const char*>(source.Data()), source.Size());

	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = data.find(typeToken, 0);
	while (pos != std::string::npos)
	{
		size_t eol = data.find_first_of("\r\n", pos);
		SE_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + typeTokenLength + 1;
		std::string type = data.substr(begin, eol - begin);
		SE_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel" || type == "compute",
		               "Invalid shader type specified");

		size_t nextLinePos = data.find_first_not_of("\r\n", eol);
		pos = data.find(typeToken, nextLinePos);
		auto shaderType = ShaderTypeFromString(type);
		shaderSources[shaderType] = data.substr(nextLinePos,
		                                        pos - (nextLinePos == std::string::npos ? data.size() - 1 : nextLinePos
		                                        ));

		// Compute shaders cannot contain other types
		if (shaderType == GL_COMPUTE_SHADER)
		{
			m_IsCompute = true;
			break;
		}
	}

	return shaderSources;
}

// Parsing helper functions
const char* FindToken(const char* str, const std::string& token)
{
	const char* t = str;
	while (t = strstr(t, token.c_str()))
	{
		bool left = str == t || isspace(t[-1]);
		bool right = !t[token.size()] || isspace(t[token.size()]);
		if (left && right) return t;

		t += token.size();
	}
	return nullptr;
}

const char* FindToken(const std::string& string, const std::string& token)
{
	return FindToken(string.c_str(), token);
}

std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
{
	size_t start = 0;
	size_t end = string.find_first_of(delimiters);

	std::vector<std::string> result;

	while (end <= std::string::npos)
	{
		std::string token = string.substr(start, end - start);
		if (!token.empty()) result.push_back(token);

		if (end == std::string::npos) break;

		start = end + 1;
		end = string.find_first_of(delimiters, start);
	}

	return result;
}

std::vector<std::string> SplitString(const std::string& string, const char delimiter)
{
	return SplitString(string, std::string(1, delimiter));
}

std::vector<std::string> Tokenize(const std::string& string)
{
	return SplitString(string, " \t\n\r");
}

std::vector<std::string> GetLines(const std::string& string)
{
	return SplitString(string, "\n");
}

std::string GetBlock(const char* str, const char** outPosition)
{
	const char* end = strstr(str, "}");
	if (!end) return str;

	if (outPosition) *outPosition = end;
	uint32_t length = end - str + 1;
	return std::string(str, length);
}

std::string GetStatement(const char* str, const char** outPosition)
{
	const char* end = strstr(str, ";");
	if (!end) return str;

	if (outPosition) *outPosition = end;
	uint32_t length = end - str + 1;
	return std::string(str, length);
}

bool StartsWith(const std::string& string, const std::string& start)
{
	return string.find(start) == 0;
}


void OpenGLShader::Parse()
{
	const char* token;
	const char* vstr;
	const char* fstr;

	m_Resources.clear();
	m_Structs.clear();
	m_VSMaterialUniformBuffer.Reset();
	m_PSMaterialUniformBuffer.Reset();

	auto& vertexSource = m_ShaderSource[GL_VERTEX_SHADER];
	auto& fragmentSource = m_ShaderSource[GL_FRAGMENT_SHADER];

	// Vertex Shader
	vstr = vertexSource.c_str();
	while (token = FindToken(vstr, "struct")) ParseUniformStruct(GetBlock(token, &vstr), ShaderDomain::Vertex);

	vstr = vertexSource.c_str();
	while (token = FindToken(vstr, "uniform")) ParseUniform(GetStatement(token, &vstr), ShaderDomain::Vertex);

	// Fragment Shader
	fstr = fragmentSource.c_str();
	while (token = FindToken(fstr, "struct")) ParseUniformStruct(GetBlock(token, &fstr), ShaderDomain::Pixel);

	fstr = fragmentSource.c_str();
	while (token = FindToken(fstr, "uniform")) ParseUniform(GetStatement(token, &fstr), ShaderDomain::Pixel);
}

static bool IsTypeStringResource(const std::string& type)
{
	if (type == "sampler1D") return true;
	if (type == "sampler2D") return true;
	if (type == "sampler2DMS") return true;
	if (type == "samplerCube") return true;
	if (type == "sampler2DShadow") return true;
	return false;
}

ShaderStruct* OpenGLShader::FindStruct(const std::string& name)
{
	for (ShaderStruct* s : m_Structs)
	{
		if (s->GetName() == name) return s;
	}
	return nullptr;
}

void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain)
{
	std::vector<std::string> tokens = Tokenize(statement);
	uint32_t index = 0;

	index++; // "uniform"
	std::string typeString = tokens[index++];
	std::string name = tokens[index++];
	// Strip ; from name if present
	if (const char* s = strstr(name.c_str(), ";")) name = std::string(name.c_str(), s - name.c_str());

	std::string n(name);
	int32_t count = 1;
	const char* namestr = n.c_str();
	if (const char* s = strstr(namestr, "["))
	{
		name = std::string(namestr, s - namestr);

		const char* end = strstr(namestr, "]");
		std::string c(s + 1, end - s);
		count = atoi(c.c_str());
	}

	if (IsTypeStringResource(typeString))
	{
		ShaderResourceDeclaration* declaration = new OpenGLShaderResourceDeclaration(
			OpenGLShaderResourceDeclaration::StringToType(typeString), name, count);
		m_Resources.push_back(declaration);
	}
	else
	{
		OpenGLShaderUniformDeclaration::Type t = OpenGLShaderUniformDeclaration::StringToType(typeString);
		OpenGLShaderUniformDeclaration* declaration = nullptr;

		if (t == OpenGLShaderUniformDeclaration::Type::None)
		{
			// Find struct
			ShaderStruct* s = FindStruct(typeString);
			SE_CORE_ASSERT(s, "");
			declaration = new OpenGLShaderUniformDeclaration(domain, s, name, count);
		}
		else
		{
			declaration = new OpenGLShaderUniformDeclaration(domain, t, name, count);
		}

		if (StartsWith(name, "r_"))
		{
			if (domain == ShaderDomain::Vertex) static_cast<OpenGLShaderUniformBufferDeclaration*>(
				m_VSRendererUniformBuffers.front())->PushUniform(declaration);
			else if (domain == ShaderDomain::Pixel) static_cast<OpenGLShaderUniformBufferDeclaration*>(
				m_PSRendererUniformBuffers.front())->PushUniform(declaration);
		}
		else
		{
			if (domain == ShaderDomain::Vertex)
			{
				if (!m_VSMaterialUniformBuffer)
					m_VSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDeclaration("", domain));

				m_VSMaterialUniformBuffer->PushUniform(declaration);
			}
			else if (domain == ShaderDomain::Pixel)
			{
				if (!m_PSMaterialUniformBuffer)
					m_PSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDeclaration("", domain));

				m_PSMaterialUniformBuffer->PushUniform(declaration);
			}
		}
	}
}

void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain)
{
	std::vector<std::string> tokens = Tokenize(block);

	uint32_t index = 0;
	index++; // struct
	std::string name = tokens[index++];
	ShaderStruct* uniformStruct = new ShaderStruct(name);
	index++; // {
	while (index < tokens.size())
	{
		if (tokens[index] == "}") break;

		std::string type = tokens[index++];
		std::string name = tokens[index++];

		// Strip ; from name if present
		if (const char* s = strstr(name.c_str(), ";")) name = std::string(name.c_str(), s - name.c_str());

		uint32_t count = 1;
		const char* namestr = name.c_str();
		if (const char* s = strstr(namestr, "["))
		{
			name = std::string(namestr, s - namestr);

			const char* end = strstr(namestr, "]");
			std::string c(s + 1, end - s);
			count = atoi(c.c_str());
		}
		ShaderUniformDeclaration* field = new OpenGLShaderUniformDeclaration(
			domain, OpenGLShaderUniformDeclaration::StringToType(type), name, count);
		uniformStruct->AddField(field);
	}
	m_Structs.push_back(uniformStruct);
}

void OpenGLShader::ResolveUniforms()
{
	glUseProgram(m_RendererID);

	for (size_t i = 0; i < m_VSRendererUniformBuffers.size(); i++)
	{
		OpenGLShaderUniformBufferDeclaration* decl = static_cast<OpenGLShaderUniformBufferDeclaration*>(
			m_VSRendererUniformBuffers[i]);
		const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
		for (size_t j = 0; j < uniforms.size(); j++)
		{
			OpenGLShaderUniformDeclaration* uniform = static_cast<OpenGLShaderUniformDeclaration*>(uniforms[j]);
			if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::Struct)
			{
				const ShaderStruct& s = uniform->GetShaderUniformStruct();
				const auto& fields = s.GetFields();
				for (size_t k = 0; k < fields.size(); k++)
				{
					OpenGLShaderUniformDeclaration* field = static_cast<OpenGLShaderUniformDeclaration*>(fields[k]);
					field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
				}
			}
			else
			{
				uniform->m_Location = GetUniformLocation(uniform->m_Name);
			}
		}
	}

	for (size_t i = 0; i < m_PSRendererUniformBuffers.size(); i++)
	{
		OpenGLShaderUniformBufferDeclaration* decl = static_cast<OpenGLShaderUniformBufferDeclaration*>(
			m_PSRendererUniformBuffers[i]);
		const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
		for (size_t j = 0; j < uniforms.size(); j++)
		{
			OpenGLShaderUniformDeclaration* uniform = static_cast<OpenGLShaderUniformDeclaration*>(uniforms[j]);
			if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::Struct)
			{
				const ShaderStruct& s = uniform->GetShaderUniformStruct();
				const auto& fields = s.GetFields();
				for (size_t k = 0; k < fields.size(); k++)
				{
					OpenGLShaderUniformDeclaration* field = static_cast<OpenGLShaderUniformDeclaration*>(fields[k]);
					field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
				}
			}
			else
			{
				uniform->m_Location = GetUniformLocation(uniform->m_Name);
			}
		}
	}

	{
		const auto& decl = m_VSMaterialUniformBuffer;
		if (decl)
		{
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* uniform = static_cast<OpenGLShaderUniformDeclaration*>(uniforms[j]);
				if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::Struct)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* field = static_cast<OpenGLShaderUniformDeclaration*>(fields[k]);
						field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->m_Name);
				}
			}
		}
	}

	{
		const auto& decl = m_PSMaterialUniformBuffer;
		if (decl)
		{
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* uniform = static_cast<OpenGLShaderUniformDeclaration*>(uniforms[j]);
				if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::Struct)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* field = static_cast<OpenGLShaderUniformDeclaration*>(fields[k]);
						field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->m_Name);
				}
			}
		}
	}

	uint32_t sampler = 0;
	for (size_t i = 0; i < m_Resources.size(); i++)
	{
		OpenGLShaderResourceDeclaration* resource = static_cast<OpenGLShaderResourceDeclaration*>(m_Resources[i]);
		int32_t location = GetUniformLocation(resource->m_Name);

		if (resource->GetCount() == 1)
		{
			resource->m_Register = sampler;
			if (location != -1) UploadUniformInt(location, sampler);

			sampler++;
		}
		else if (resource->GetCount() > 1)
		{
			resource->m_Register = sampler;
			uint32_t count = resource->GetCount();
			int* samplers = new int[count];
			for (uint32_t s = 0; s < count; s++) samplers[s] = sampler++;
			UploadUniformIntArray(resource->GetName(), samplers, count);
			delete[] samplers;
		}
	}
}

void OpenGLShader::ValidateUniforms()
{
}

int32_t OpenGLShader::GetUniformLocation(const std::string& name) const
{
	int32_t result = glGetUniformLocation(m_RendererID, name.c_str());
	if (result == -1)
		SE_CORE_WARN("Could not find uniform '{0}' in shader", name);

	return result;
}

GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
{
	if (type == "vertex") return GL_VERTEX_SHADER;
	if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
	if (type == "compute") return GL_COMPUTE_SHADER;

	return GL_NONE;
}

void OpenGLShader::CompileAndUploadShader()
{
	std::vector<GLuint> shaderRendererIDs;

	GLuint program = glCreateProgram();
	for (auto& kv : m_ShaderSource)
	{
		GLenum type = kv.first;
		std::string& source = kv.second;

		GLuint shaderRendererID = glCreateShader(type);
		const GLchar* sourceCstr = static_cast<const GLchar*>(source.c_str());
		glShaderSource(shaderRendererID, 1, &sourceCstr, nullptr);

		glCompileShader(shaderRendererID);

		GLint isCompiled = 0;
		glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shaderRendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

			SE_CORE_ERROR("Shader compilation failed ({0}):\n{1}", m_Filepath, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(shaderRendererID);

			SE_CORE_ASSERT(false, "Failed");
		}

		shaderRendererIDs.push_back(shaderRendererID);
		glAttachShader(program, shaderRendererID);
	}

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, static_cast<int*>(&isLinked));
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		SE_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_Filepath, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		for (auto id : shaderRendererIDs)
			glDeleteShader(id);
	}

	// Always detach shaders after a successful link.
	for (auto id : shaderRendererIDs)
		glDetachShader(program, id);

	m_RendererID = program;
}

void OpenGLShader::SetVSMaterialUniformBuffer(Buffer buffer)
{
	Renderer::Submit([this, buffer]()
	{
		glUseProgram(m_RendererID);
		ResolveAndSetUniforms(m_VSMaterialUniformBuffer, buffer);
	});
}

void OpenGLShader::SetPSMaterialUniformBuffer(Buffer buffer)
{
	Renderer::Submit([this, buffer]()
	{
		glUseProgram(m_RendererID);
		ResolveAndSetUniforms(m_PSMaterialUniformBuffer, buffer);
	});
}

void OpenGLShader::ResolveAndSetUniforms(const Shared<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer)
{
	const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		OpenGLShaderUniformDeclaration* uniform = static_cast<OpenGLShaderUniformDeclaration*>(uniforms[i]);
		if (uniform->IsArray()) ResolveAndSetUniformArray(uniform, buffer);
		else ResolveAndSetUniform(uniform, buffer);
	}
}

void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
{
	if (uniform->GetLocation() == -1) return;

	SE_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

	uint32_t offset = uniform->GetOffset();
	switch (uniform->GetType())
	{
	case OpenGLShaderUniformDeclaration::Type::Bool: UploadUniformFloat(
			uniform->GetLocation(), *(bool*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Float32: UploadUniformFloat(
			uniform->GetLocation(), *(float*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Int32: UploadUniformInt(uniform->GetLocation(),
	                                                                   *(int32_t*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec2: UploadUniformFloat2(
			uniform->GetLocation(), *(glm::vec2*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec3: UploadUniformFloat3(
			uniform->GetLocation(), *(glm::vec3*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec4: UploadUniformFloat4(
			uniform->GetLocation(), *(glm::vec4*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat3: UploadUniformMat3(uniform->GetLocation(),
	                                                                   *(glm::mat3*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat4: UploadUniformMat4(uniform->GetLocation(),
	                                                                   *(glm::mat4*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Struct: UploadUniformStruct(uniform, buffer.Data(), offset);
		break;
	default: SE_CORE_ASSERT(false, "Unknown uniform type!");
	}
}

void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
{
	//SE_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

	uint32_t offset = uniform->GetOffset();
	switch (uniform->GetType())
	{
	case OpenGLShaderUniformDeclaration::Type::Bool: UploadUniformFloat(
			uniform->GetLocation(), *(bool*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Float32: UploadUniformFloat(
			uniform->GetLocation(), *(float*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Int32: UploadUniformInt(uniform->GetLocation(),
	                                                                   *(int32_t*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec2: UploadUniformFloat2(
			uniform->GetLocation(), *(glm::vec2*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec3: UploadUniformFloat3(
			uniform->GetLocation(), *(glm::vec3*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec4: UploadUniformFloat4(
			uniform->GetLocation(), *(glm::vec4*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat3: UploadUniformMat3(uniform->GetLocation(),
	                                                                   *(glm::mat3*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat4: UploadUniformMat4Array(
			uniform->GetLocation(), *(glm::mat4*)&buffer.Data()[offset], uniform->GetCount());
		break;
	case OpenGLShaderUniformDeclaration::Type::Struct: UploadUniformStruct(uniform, buffer.Data(), offset);
		break;
	default: SE_CORE_ASSERT(false, "Unknown uniform type!");
	}
}

void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, Uint8* data, int32_t offset)
{
	switch (field.GetType())
	{
	case OpenGLShaderUniformDeclaration::Type::Bool: UploadUniformFloat(field.GetLocation(), *(bool*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Float32: UploadUniformFloat(field.GetLocation(), *(float*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Int32: UploadUniformInt(field.GetLocation(), *(int32_t*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec2: UploadUniformFloat2(
			field.GetLocation(), *(glm::vec2*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec3: UploadUniformFloat3(
			field.GetLocation(), *(glm::vec3*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec4: UploadUniformFloat4(
			field.GetLocation(), *(glm::vec4*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat3: UploadUniformMat3(field.GetLocation(), *(glm::mat3*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat4: UploadUniformMat4(field.GetLocation(), *(glm::mat4*)&data[offset]);
		break;
	default: SE_CORE_ASSERT(false, "Unknown uniform type!");
	}
}

void OpenGLShader::UploadUniformBuffer(const UniformBufferBase& uniformBuffer)
{
	for (unsigned int i = 0; i < uniformBuffer.GetUniformCount(); i++)
	{
		const UniformDecl& decl = uniformBuffer.GetUniforms()[i];
		switch (decl.Type)
		{
		case UniformType::Float:
		{
			const std::string& name = decl.Name;
			float value = *(float*)(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]()
			{
				UploadUniformFloat(name, value);
			});
		}
		case UniformType::Float3:
		{
			const std::string& name = decl.Name;
			glm::vec3& values = *(glm::vec3*)(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]()
			{
				UploadUniformFloat3(name, values);
			});
		}
		case UniformType::Float4:
		{
			const std::string& name = decl.Name;
			glm::vec4& values = *(glm::vec4*)(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]()
			{
				UploadUniformFloat4(name, values);
			});
		}
		case UniformType::Matrix4x4:
		{
			const std::string& name = decl.Name;
			glm::mat4& values = *(glm::mat4*)(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]()
			{
				UploadUniformMat4(name, values);
			});
		}
		}
	}
}

void OpenGLShader::SetFloat(const std::string& name, float value)
{
	Renderer::Submit([=]()
	{
		UploadUniformFloat(name, value);
	});
}

void OpenGLShader::SetInt(const std::string& name, int value)
{
	Renderer::Submit([=]()
	{
		UploadUniformInt(name, value);
	});
}

void OpenGLShader::SetBool(const std::string& name, bool value)
{
	Renderer::Submit([=]()
	{
		UploadUniformInt(name, value);
	});
}

void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
{
	Renderer::Submit([=]()
	{
		UploadUniformFloat2(name, value);
	});
}

void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
{
	Renderer::Submit([=]()
	{
		UploadUniformFloat3(name, value);
	});
}

void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
{
	Renderer::Submit([=]()
	{
		UploadUniformMat4(name, value);
	});
}

void OpenGLShader::SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind)
{
	if (bind)
	{
		UploadUniformMat4(name, value);
	}
	else
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1) UploadUniformMat4(location, value);
	}
}

void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t size)
{
	Renderer::Submit([=]()
	{
		UploadUniformIntArray(name, values, size);
	});
}

void OpenGLShader::UploadUniformInt(uint32_t location, int32_t value)
{
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(uint32_t location, int32_t* values, int32_t count)
{
	glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(uint32_t location, float value)
{
	glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(uint32_t location, const glm::vec2& value)
{
	glUniform2f(location, value.x, value.y);
}

void OpenGLShader::UploadUniformFloat3(uint32_t location, const glm::vec3& value)
{
	glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::UploadUniformFloat4(uint32_t location, const glm::vec4& value)
{
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::UploadUniformMat3(uint32_t location, const glm::mat3& value)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(value));
}

void OpenGLShader::UploadUniformMat4(uint32_t location, const glm::mat4& value)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
}

void OpenGLShader::UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count)
{
	glUniformMatrix4fv(location, count, GL_FALSE, value_ptr(values));
}

void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, Uint8* buffer, uint32_t offset)
{
	const ShaderStruct& s = uniform->GetShaderUniformStruct();
	const auto& fields = s.GetFields();
	for (size_t k = 0; k < fields.size(); k++)
	{
		OpenGLShaderUniformDeclaration* field = static_cast<OpenGLShaderUniformDeclaration*>(fields[k]);
		ResolveAndSetUniformField(*field, buffer, offset);
		offset += field->m_Size;
	}
}

void OpenGLShader::UploadUniformInt(const std::string& name, int32_t value)
{
	int32_t location = GetUniformLocation(name);
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count)
{
	int32_t location = GetUniformLocation(name);
	glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
{
	glUseProgram(m_RendererID);
	auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location != -1)
		glUniform1f(location, value);
	else HZ_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
{
	glUseProgram(m_RendererID);
	auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location != -1)
		glUniform2f(location, values.x, values.y);
	else HZ_LOG_UNIFORM("Uniform '{0}' not found!", name);
}


void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
{
	glUseProgram(m_RendererID);
	auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location != -1)
		glUniform3f(location, values.x, values.y, values.z);
	else HZ_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
{
	glUseProgram(m_RendererID);
	auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location != -1)
		glUniform4f(location, values.x, values.y, values.z, values.w);
	else HZ_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& values)
{
	glUseProgram(m_RendererID);
	auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location != -1)
		glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&values);
	else HZ_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

const ShaderUniformBufferList& OpenGLShader::GetVSRendererUniforms() const
{
	return m_VSRendererUniformBuffers;
}

const ShaderUniformBufferList& OpenGLShader::GetPSRendererUniforms() const
{
	return m_PSRendererUniformBuffers;
}

bool OpenGLShader::HasVSMaterialUniformBuffer() const
{
	return static_cast<bool>(m_VSMaterialUniformBuffer);
}

bool OpenGLShader::HasPSMaterialUniformBuffer() const
{
	return static_cast<bool>(m_PSMaterialUniformBuffer);
}

const ShaderUniformBufferDeclaration& OpenGLShader::GetVSMaterialUniformBuffer() const
{
	return *m_VSMaterialUniformBuffer;
}

const ShaderUniformBufferDeclaration& OpenGLShader::GetPSMaterialUniformBuffer() const
{
	return *m_PSMaterialUniformBuffer;
}

const ShaderResourceList& OpenGLShader::GetResources() const
{
	return m_Resources;
}
}
