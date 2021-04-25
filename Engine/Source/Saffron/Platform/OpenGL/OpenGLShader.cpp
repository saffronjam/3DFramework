#include "SaffronPCH.h"
#include "OpenGLShader.h"

#include "Saffron/Core/Misc.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
#define UNIFORM_LOGGING 0
#if UNIFORM_LOGGING
template <typename Arg, typename... Args>
constexpr void LogUniform(const String& message, Arg&& arg, Args&& ... args)
{
	Log::CoreWarn(message, std::forward(arg), std::forward(args));
}
#else
template <typename Arg, typename... Args>
constexpr void LogUniform(const String& message, Arg&& arg, Args&& ... args)
{
}
#define SE_LOG_UNIFORM
#endif

OpenGLShader::OpenGLShader(const Filepath& filepath)
{
	_filepath = Move(filepath);

	const auto fpString = _filepath.string();

	size_t found = fpString.find_last_of("/\\");
	_name = found != String::npos ? fpString.substr(found + 1) : fpString;
	found = _name.find_last_of('.');
	_name = found != String::npos ? _name.substr(0, found) : _name;

	static int i = 0;
	Log::CoreInfo("Loading no: {0}: filepath: {1}", i++, filepath.string());

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
	const auto source = ReadShaderFromFile(_filepath);
	Load(source);
}

void OpenGLShader::Load(const Buffer& source)
{
	_shaderSource = PreProcess(source);
	if (!_isCompute) Parse();

	Renderer::Submit([=]()
	{
		if (_rendererID)
			glDeleteProgram(_rendererID);

		CompileAndUploadShader();
		if (!_isCompute)
		{
			ResolveUniforms();
			ValidateUniforms();
		}

		if (_loaded)
		{
			for (auto& callback : _shaderReloadedCallbacks) callback();
		}

		_loaded = true;
	});
}

void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
{
	_shaderReloadedCallbacks.push_back(callback);
}

void OpenGLShader::Bind()
{
	Renderer::Submit([=]()
	{
		glUseProgram(_rendererID);
	});
}

Buffer OpenGLShader::ReadShaderFromFile(const Filepath& filepath) const
{
	String result;
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
		Debug::Assert(false, "Could not load shader!");
	}

	in.close();
	return Buffer::Copy(result.c_str(), result.length() * sizeof(String::value_type));
}

HashMap<GLenum, String> OpenGLShader::PreProcess(const Buffer& source)
{
	HashMap<GLenum, String> shaderSources;

	String data(reinterpret_cast<const char*>(source.Data()), source.Size());

	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = data.find(typeToken, 0);
	while (pos != String::npos)
	{
		size_t eol = data.find_first_of("\r\n", pos);
		Debug::Assert(eol != String::npos, "Syntax error");
		size_t begin = pos + typeTokenLength + 1;
		String type = data.substr(begin, eol - begin);
		Debug::Assert(type == "vertex" || type == "fragment" || type == "pixel" || type == "compute",
		               "Invalid shader type specified");

		size_t nextLinePos = data.find_first_not_of("\r\n", eol);
		pos = data.find(typeToken, nextLinePos);
		auto shaderType = ShaderTypeFromString(type);
		shaderSources[shaderType] = data.substr(nextLinePos,
		                                        pos - (nextLinePos == String::npos ? data.size() - 1 : nextLinePos));

		// Compute shaders cannot contain other types
		if (shaderType == GL_COMPUTE_SHADER)
		{
			_isCompute = true;
			break;
		}
	}

	return shaderSources;
}

// Parsing helper functions
const char* FindToken(const char* str, const String& token)
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

const char* FindToken(const String& string, const String& token)
{
	return FindToken(string.c_str(), token);
}

ArrayList<String> SplitString(const String& string, const String& delimiters)
{
	size_t start = 0;
	size_t end = string.find_first_of(delimiters);

	ArrayList<String> result;

	while (end <= String::npos)
	{
		String token = string.substr(start, end - start);
		if (!token.empty()) result.push_back(token);

		if (end == String::npos) break;

		start = end + 1;
		end = string.find_first_of(delimiters, start);
	}

	return result;
}

ArrayList<String> SplitString(const String& string, const char delimiter)
{
	return SplitString(string, String(1, delimiter));
}

ArrayList<String> Tokenize(const String& string)
{
	return SplitString(string, " \t\n\r");
}

ArrayList<String> GetLines(const String& string)
{
	return SplitString(string, "\n");
}

String GetBlock(const char* str, const char** outPosition)
{
	const char* end = strstr(str, "}");
	if (!end) return str;

	if (outPosition) *outPosition = end;
	uint length = end - str + 1;
	return String(str, length);
}

String GetStatement(const char* str, const char** outPosition)
{
	const char* end = strstr(str, ";");
	if (!end) return str;

	if (outPosition) *outPosition = end;
	uint length = end - str + 1;
	return String(str, length);
}

bool StartsWith(const String& string, const String& start)
{
	return string.find(start) == 0;
}


void OpenGLShader::Parse()
{
	const char* token;
	const char* vstr;
	const char* fstr;

	_resources.clear();
	_structs.clear();
	_vSMaterialUniformBuffer.Reset();
	_pSMaterialUniformBuffer.Reset();

	auto& vertexSource = _shaderSource[GL_VERTEX_SHADER];
	auto& fragmentSource = _shaderSource[GL_FRAGMENT_SHADER];

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

static bool IsTypeStringResource(const String& type)
{
	if (type == "sampler1D") return true;
	if (type == "sampler2D") return true;
	if (type == "sampler2DMS") return true;
	if (type == "samplerCube") return true;
	if (type == "sampler2DShadow") return true;
	return false;
}

ShaderStruct* OpenGLShader::FindStruct(const String& name)
{
	for (ShaderStruct* s : _structs)
	{
		if (s->GetName() == name) return s;
	}
	return nullptr;
}

void OpenGLShader::ParseUniform(const String& statement, ShaderDomain domain)
{
	ArrayList<String> tokens = Tokenize(statement);
	uint index = 0;

	index++; // "uniform"
	String typeString = tokens[index++];
	String name = tokens[index++];
	// Strip ; from name if present
	if (const char* s = strstr(name.c_str(), ";")) name = String(name.c_str(), s - name.c_str());

	String n(name);
	int32_t count = 1;
	const char* namestr = n.c_str();
	if (const char* s = strstr(namestr, "["))
	{
		name = String(namestr, s - namestr);

		const char* end = strstr(namestr, "]");
		String c(s + 1, end - s);
		count = atoi(c.c_str());
	}

	if (IsTypeStringResource(typeString))
	{
		ShaderResourceDeclaration* declaration = new OpenGLShaderResourceDeclaration(
			OpenGLShaderResourceDeclaration::StringToType(typeString), name, count);
		_resources.push_back(declaration);
	}
	else
	{
		OpenGLShaderUniformDeclaration::Type t = OpenGLShaderUniformDeclaration::StringToType(typeString);
		OpenGLShaderUniformDeclaration* declaration = nullptr;

		if (t == OpenGLShaderUniformDeclaration::Type::None)
		{
			// Find struct
			ShaderStruct* s = FindStruct(typeString);
			Debug::Assert(s, "");
			declaration = new OpenGLShaderUniformDeclaration(domain, s, name, count);
		}
		else
		{
			declaration = new OpenGLShaderUniformDeclaration(domain, t, name, count);
		}

		if (StartsWith(name, "r_"))
		{
			if (domain == ShaderDomain::Vertex)
				static_cast<OpenGLShaderUniformBufferDeclaration*>(_vSRendererUniformBuffers.front())->PushUniform(
					declaration);
			else if (domain == ShaderDomain::Pixel)
				static_cast<OpenGLShaderUniformBufferDeclaration*>(_pSRendererUniformBuffers.front())->PushUniform(
					declaration);
		}
		else
		{
			if (domain == ShaderDomain::Vertex)
			{
				if (!_vSMaterialUniformBuffer)
					_vSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDeclaration("", domain));

				_vSMaterialUniformBuffer->PushUniform(declaration);
			}
			else if (domain == ShaderDomain::Pixel)
			{
				if (!_pSMaterialUniformBuffer)
					_pSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDeclaration("", domain));

				_pSMaterialUniformBuffer->PushUniform(declaration);
			}
		}
	}
}

void OpenGLShader::ParseUniformStruct(const String& block, ShaderDomain domain)
{
	ArrayList<String> tokens = Tokenize(block);

	uint index = 0;
	index++; // struct
	String name = tokens[index++];
	ShaderStruct* uniformStruct = new ShaderStruct(name);
	index++; // {
	while (index < tokens.size())
	{
		if (tokens[index] == "}") break;

		String type = tokens[index++];
		String name = tokens[index++];

		// Strip ; from name if present
		if (const char* s = strstr(name.c_str(), ";")) name = String(name.c_str(), s - name.c_str());

		uint count = 1;
		const char* namestr = name.c_str();
		if (const char* s = strstr(namestr, "["))
		{
			name = String(namestr, s - namestr);

			const char* end = strstr(namestr, "]");
			String c(s + 1, end - s);
			count = atoi(c.c_str());
		}
		ShaderUniformDeclaration* field = new OpenGLShaderUniformDeclaration(
			domain, OpenGLShaderUniformDeclaration::StringToType(type), name, count);
		uniformStruct->AddField(field);
	}
	_structs.push_back(uniformStruct);
}

void OpenGLShader::ResolveUniforms()
{
	glUseProgram(_rendererID);

	for (size_t i = 0; i < _vSRendererUniformBuffers.size(); i++)
	{
		OpenGLShaderUniformBufferDeclaration* decl = static_cast<OpenGLShaderUniformBufferDeclaration*>(
			_vSRendererUniformBuffers[i]);
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
					field->_location = GetUniformLocation(uniform->_name + "." + field->_name);
				}
			}
			else
			{
				uniform->_location = GetUniformLocation(uniform->_name);
			}
		}
	}

	for (size_t i = 0; i < _pSRendererUniformBuffers.size(); i++)
	{
		OpenGLShaderUniformBufferDeclaration* decl = static_cast<OpenGLShaderUniformBufferDeclaration*>(
			_pSRendererUniformBuffers[i]);
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
					field->_location = GetUniformLocation(uniform->_name + "." + field->_name);
				}
			}
			else
			{
				uniform->_location = GetUniformLocation(uniform->_name);
			}
		}
	}

	{
		const auto& decl = _vSMaterialUniformBuffer;
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
						field->_location = GetUniformLocation(uniform->_name + "." + field->_name);
					}
				}
				else
				{
					uniform->_location = GetUniformLocation(uniform->_name);
				}
			}
		}
	}

	{
		const auto& decl = _pSMaterialUniformBuffer;
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
						field->_location = GetUniformLocation(uniform->_name + "." + field->_name);
					}
				}
				else
				{
					uniform->_location = GetUniformLocation(uniform->_name);
				}
			}
		}
	}

	uint sampler = 0;
	for (size_t i = 0; i < _resources.size(); i++)
	{
		OpenGLShaderResourceDeclaration* resource = static_cast<OpenGLShaderResourceDeclaration*>(_resources[i]);
		int32_t location = GetUniformLocation(resource->_name);

		if (resource->GetCount() == 1)
		{
			resource->_register = sampler;
			if (location != -1) UploadUniformInt(location, sampler);

			sampler++;
		}
		else if (resource->GetCount() > 1)
		{
			resource->_register = sampler;
			uint count = resource->GetCount();
			int* samplers = new int[count];
			for (uint s = 0; s < count; s++) samplers[s] = sampler++;
			UploadUniformIntArray(resource->GetName(), samplers, count);
			delete[] samplers;
		}
	}
}

void OpenGLShader::ValidateUniforms()
{
}

int32_t OpenGLShader::GetUniformLocation(const String& name) const
{
	int32_t result = glGetUniformLocation(_rendererID, name.c_str());
	if (result == -1)
		Log::CoreWarn("Could not find uniform '{0}' in shader", name);

	return result;
}

GLenum OpenGLShader::ShaderTypeFromString(const String& type)
{
	if (type == "vertex") return GL_VERTEX_SHADER;
	if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
	if (type == "compute") return GL_COMPUTE_SHADER;

	return GL_NONE;
}

void OpenGLShader::CompileAndUploadShader()
{
	ArrayList<GLuint> shaderRendererIDs;

	GLuint program = glCreateProgram();
	for (auto& kv : _shaderSource)
	{
		GLenum type = kv.first;
		String& source = kv.second;

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
			ArrayList<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

			Log::CoreError("Shader compilation failed ({0}):\n{1}", _filepath, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(shaderRendererID);

			Debug::Assert(false, "Failed");
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
		ArrayList<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		Log::CoreError("Shader linking failed ({0}):\n{1}", _filepath, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		for (auto id : shaderRendererIDs)
			glDeleteShader(id);
	}

	// Always detach shaders after a successful link.
	for (auto id : shaderRendererIDs)
		glDetachShader(program, id);

	_rendererID = program;
}

void OpenGLShader::SetVSMaterialUniformBuffer(Buffer buffer)
{
	Renderer::Submit([this, buffer]()
	{
		glUseProgram(_rendererID);
		ResolveAndSetUniforms(_vSMaterialUniformBuffer, buffer);
	});
}

void OpenGLShader::SetPSMaterialUniformBuffer(Buffer buffer)
{
	Renderer::Submit([this, buffer]()
	{
		glUseProgram(_rendererID);
		ResolveAndSetUniforms(_pSMaterialUniformBuffer, buffer);
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

	Debug::Assert(uniform->GetLocation() != -1, "Uniform has invalid location!");

	uint offset = uniform->GetOffset();
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
			uniform->GetLocation(), *(Vector2*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec3: UploadUniformFloat3(
			uniform->GetLocation(), *(Vector3*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec4: UploadUniformFloat4(
			uniform->GetLocation(), *(Vector4*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat3: UploadUniformMat3(uniform->GetLocation(),
	                                                                   *(Matrix3*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat4: UploadUniformMat4(uniform->GetLocation(),
	                                                                   *(Matrix4*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Struct: UploadUniformStruct(uniform, buffer.Data(), offset);
		break;
	default: Debug::Assert(false, "Unknown uniform type!");
	}
}

void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
{
	//Debug::Assert(uniform->GetLocation() != -1, "Uniform has invalid location!");

	uint offset = uniform->GetOffset();
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
			uniform->GetLocation(), *(Vector2*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec3: UploadUniformFloat3(
			uniform->GetLocation(), *(Vector3*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec4: UploadUniformFloat4(
			uniform->GetLocation(), *(Vector4*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat3: UploadUniformMat3(uniform->GetLocation(),
	                                                                   *(Matrix3*)&buffer.Data()[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat4: UploadUniformMat4Array(
			uniform->GetLocation(), *(Matrix4*)&buffer.Data()[offset], uniform->GetCount());
		break;
	case OpenGLShaderUniformDeclaration::Type::Struct: UploadUniformStruct(uniform, buffer.Data(), offset);
		break;
	default: Debug::Assert(false, "Unknown uniform type!");
	}
}

void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, uchar* data, int32_t offset)
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
			field.GetLocation(), *(Vector2*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec3: UploadUniformFloat3(
			field.GetLocation(), *(Vector3*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec4: UploadUniformFloat4(
			field.GetLocation(), *(Vector4*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat3: UploadUniformMat3(field.GetLocation(), *(Matrix3*)&data[offset]);
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat4: UploadUniformMat4(field.GetLocation(), *(Matrix4*)&data[offset]);
		break;
	default: Debug::Assert(false, "Unknown uniform type!");
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
			const String& name = decl.Name;
			float value = *(float*)(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]()
			{
				UploadUniformFloat(name, value);
			});
		}
		case UniformType::Float3:
		{
			const String& name = decl.Name;
			Vector3& values = *(Vector3*)(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]()
			{
				UploadUniformFloat3(name, values);
			});
		}
		case UniformType::Float4:
		{
			const String& name = decl.Name;
			Vector4& values = *(Vector4*)(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]()
			{
				UploadUniformFloat4(name, values);
			});
		}
		case UniformType::Matrix4x4:
		{
			const String& name = decl.Name;
			Matrix4& values = *(Matrix4*)(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]()
			{
				UploadUniformMat4(name, values);
			});
		}
		}
	}
}

void OpenGLShader::SetFloat(const String& name, float value)
{
	Renderer::Submit([=]()
	{
		UploadUniformFloat(name, value);
	});
}

void OpenGLShader::SetInt(const String& name, int value)
{
	Renderer::Submit([=]()
	{
		UploadUniformInt(name, value);
	});
}

void OpenGLShader::SetBool(const String& name, bool value)
{
	Renderer::Submit([=]()
	{
		UploadUniformInt(name, value);
	});
}

void OpenGLShader::SetFloat2(const String& name, const Vector2& value)
{
	Renderer::Submit([=]()
	{
		UploadUniformFloat2(name, value);
	});
}

void OpenGLShader::SetFloat3(const String& name, const Vector3& value)
{
	Renderer::Submit([=]()
	{
		UploadUniformFloat3(name, value);
	});
}

void OpenGLShader::SetMat4(const String& name, const Matrix4& value)
{
	Renderer::Submit([=]()
	{
		UploadUniformMat4(name, value);
	});
}

void OpenGLShader::SetMat4FromRenderThread(const String& name, const Matrix4& value, bool bind)
{
	if (bind)
	{
		UploadUniformMat4(name, value);
	}
	else
	{
		int location = glGetUniformLocation(_rendererID, name.c_str());
		if (location != -1) UploadUniformMat4(location, value);
	}
}

void OpenGLShader::SetIntArray(const String& name, int* values, uint size)
{
	Renderer::Submit([=]()
	{
		UploadUniformIntArray(name, values, size);
	});
}

void OpenGLShader::UploadUniformInt(uint location, int32_t value)
{
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(uint location, int32_t* values, int32_t count)
{
	glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(uint location, float value)
{
	glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(uint location, const Vector2& value)
{
	glUniform2f(location, value.x, value.y);
}

void OpenGLShader::UploadUniformFloat3(uint location, const Vector3& value)
{
	glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::UploadUniformFloat4(uint location, const Vector4& value)
{
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::UploadUniformMat3(uint location, const Matrix3& value)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(value));
}

void OpenGLShader::UploadUniformMat4(uint location, const Matrix4& value)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
}

void OpenGLShader::UploadUniformMat4Array(uint location, const Matrix4& values, uint count)
{
	glUniformMatrix4fv(location, count, GL_FALSE, value_ptr(values));
}

void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, uchar* buffer, uint offset)
{
	const ShaderStruct& s = uniform->GetShaderUniformStruct();
	const auto& fields = s.GetFields();
	for (size_t k = 0; k < fields.size(); k++)
	{
		OpenGLShaderUniformDeclaration* field = static_cast<OpenGLShaderUniformDeclaration*>(fields[k]);
		ResolveAndSetUniformField(*field, buffer, offset);
		offset += field->_size;
	}
}

void OpenGLShader::UploadUniformInt(const String& name, int32_t value)
{
	int32_t location = GetUniformLocation(name);
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(const String& name, int32_t* values, uint count)
{
	int32_t location = GetUniformLocation(name);
	glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(const String& name, float value)
{
	glUseProgram(_rendererID);
	auto location = glGetUniformLocation(_rendererID, name.c_str());
	if (location != -1)
		glUniform1f(location, value);
	else SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformFloat2(const String& name, const Vector2& values)
{
	glUseProgram(_rendererID);
	auto location = glGetUniformLocation(_rendererID, name.c_str());
	if (location != -1)
		glUniform2f(location, values.x, values.y);
	else SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}


void OpenGLShader::UploadUniformFloat3(const String& name, const Vector3& values)
{
	glUseProgram(_rendererID);
	auto location = glGetUniformLocation(_rendererID, name.c_str());
	if (location != -1)
		glUniform3f(location, values.x, values.y, values.z);
	else SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformFloat4(const String& name, const Vector4& values)
{
	glUseProgram(_rendererID);
	auto location = glGetUniformLocation(_rendererID, name.c_str());
	if (location != -1)
		glUniform4f(location, values.x, values.y, values.z, values.w);
	else SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformMat4(const String& name, const Matrix4& values)
{
	glUseProgram(_rendererID);
	auto location = glGetUniformLocation(_rendererID, name.c_str());
	if (location != -1)
		glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&values);
	else SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

const ShaderUniformBufferList& OpenGLShader::GetVSRendererUniforms() const
{
	return _vSRendererUniformBuffers;
}

const ShaderUniformBufferList& OpenGLShader::GetPSRendererUniforms() const
{
	return _pSRendererUniformBuffers;
}

bool OpenGLShader::HasVSMaterialUniformBuffer() const
{
	return static_cast<bool>(_vSMaterialUniformBuffer);
}

bool OpenGLShader::HasPSMaterialUniformBuffer() const
{
	return static_cast<bool>(_pSMaterialUniformBuffer);
}

const ShaderUniformBufferDeclaration& OpenGLShader::GetVSMaterialUniformBuffer() const
{
	return *_vSMaterialUniformBuffer;
}

const ShaderUniformBufferDeclaration& OpenGLShader::GetPSMaterialUniformBuffer() const
{
	return *_pSMaterialUniformBuffer;
}

const ShaderResourceList& OpenGLShader::GetResources() const
{
	return _resources;
}
}
