#include "SaffronPCH.h"
#include "OpenGLShader.h"

#include <string>
#include <sstream>
#include <limits>

#include <glm/gtc/type_ptr.hpp>

#include "Saffron/Renderer/Renderer.h"

namespace Se {

/////////////////////
/// HELPER MACROS ///
/////////////////////

#define UNIFORM_LOGGING 0

#if UNIFORM_LOGGING
#define SE_LOG_UNIFORM(...) SE_CORE_WARN(__VA_ARGS__)
#else
#define SE_LOG_UNIFORM(...)
#endif

////////////////////////
/// HELPER FUNCTIONS ///
////////////////////////

const char *FindToken(const char *str, const std::string &token)
{
	const char *t = str;
	while ( (t = strstr(t, token.c_str())) )
	{
		const bool left = str == t || isspace(t[-1]);
		const bool right = !t[token.size()] || isspace(t[token.size()]);
		if ( left && right )
			return t;

		t += token.size();
	}
	return nullptr;
}

const char *FindToken(const std::string &string, const std::string &token)
{
	return FindToken(string.c_str(), token);
}

std::vector<std::string> SplitString(const std::string &string, const std::string &delimiters)
{
	size_t start = 0;
	size_t end = string.find_first_of(delimiters);

	std::vector<std::string> result;

	while ( end <= std::string::npos )
	{
		std::string token = string.substr(start, end - start);
		if ( !token.empty() )
			result.push_back(token);

		if ( end == std::string::npos )
			break;

		start = end + 1;
		end = string.find_first_of(delimiters, start);
	}

	return result;
}

std::vector<std::string> SplitString(const std::string &string, const char delimiter)
{
	return SplitString(string, std::string(1, delimiter));
}

std::vector<std::string> Tokenize(const std::string &string)
{
	return SplitString(string, " \t\n");
}

std::vector<std::string> GetLines(const std::string &string)
{
	return SplitString(string, "\n");
}

std::string GetBlock(const char *str, const char **outPosition)
{
	const char *end = strstr(str, "}");
	if ( !end )
		return str;

	if ( outPosition )
		*outPosition = end;
	const auto length = static_cast<Int32>(end - str + 1);
	return std::string(str, length);
}

std::string GetStatement(const char *str, const char **outPosition)
{
	const char *end = strstr(str, ";");
	if ( !end )
		return str;

	if ( outPosition )
		*outPosition = end;
	const auto length = static_cast<Int32>(end - str + 1);
	return std::string(str, length);
}

bool StartsWith(const std::string &string, const std::string &start)
{
	return string.find(start) == 0;
}

static bool IsTypeStringResource(const std::string &type)
{
	if ( type == "sampler2D" )		return true;
	if ( type == "sampler2DMS" )		return true;
	if ( type == "samplerCube" )		return true;
	if ( type == "sampler2DShadow" )	return true;
	return false;
}



//////////////////////////////////////////////////////////////////
/// OpenGL Shader
//////////////////////////////////////////////////////////////////

OpenGLShader::OpenGLShader(const std::string &filepath)
	: m_AssetPath(filepath)
{
	const std::filesystem::path fsPath = filepath;
	m_Name = fsPath.stem().string();

	OpenGLShader::Reload();
}

Shared<OpenGLShader> OpenGLShader::CreateFromString(const std::string &source)
{
	Shared<OpenGLShader> shader = Shared<OpenGLShader>::Create();
	shader->Load(source);
	return shader;
}

void OpenGLShader::Bind()
{
	Renderer::Submit([=]() {glUseProgram(m_RendererID); });
}

void OpenGLShader::Reload()
{
	Load(ReadFromFile(m_AssetPath));
}

void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback &callback)
{
	m_ShaderReloadedCallbacks.push_back(callback);
}

void OpenGLShader::UploadUniformBuffer(const Uniform::BufferBase &uniformBuffer)
{
	for ( unsigned int i = 0; i < uniformBuffer.GetUniformCount(); i++ )
	{
		const Uniform::Decl &decl = uniformBuffer.GetUniforms()[i];
		switch ( decl.DeclType )
		{
		case Uniform::Type::Float:
		{
			const float value = *reinterpret_cast<const float *>(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]() { UploadUniformFloat(decl.Name, value); });
			break;
		}
		case Uniform::Type::Float3:
		{
			const auto &values = *reinterpret_cast<const glm::vec3 *>(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]() { UploadUniformFloat3(decl.Name, values);	 });
			break;
		}
		case Uniform::Type::Float4:
		{
			const auto &values = *reinterpret_cast<const glm::vec4 *>(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]() { UploadUniformFloat4(decl.Name, values); });
			break;
		}
		case Uniform::Type::Matrix4x4:
		{
			const auto &values = *reinterpret_cast<const glm::mat4 *>(uniformBuffer.GetBuffer() + decl.Offset);
			Renderer::Submit([=]() { UploadUniformMat4(decl.Name, values); });
			break;
		}
		default:
			break;
		}
	}
}

void OpenGLShader::SetVSMaterialUniformBuffer(const Buffer &buffer)
{
	Renderer::Submit([this, buffer]() {
		glUseProgram(m_RendererID);
		ResolveAndSetUniforms(m_VSMaterialUniformBuffer, buffer);
					 });
}

void OpenGLShader::SetPSMaterialUniformBuffer(const Buffer &buffer)
{
	Renderer::Submit([this, buffer]() {
		glUseProgram(m_RendererID);
		ResolveAndSetUniforms(m_PSMaterialUniformBuffer, buffer);
					 });
}

void OpenGLShader::SetFloat(const std::string &name, float value)
{
	Renderer::Submit([=]() { UploadUniformFloat(name, value); });
}

void OpenGLShader::SetInt(const std::string &name, int value)
{
	Renderer::Submit([=]() { UploadUniformInt(name, value); });
}

void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value)
{
	Renderer::Submit([=]() { UploadUniformFloat3(name, value); });
}

void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &value)
{
	Renderer::Submit([=]() { UploadUniformMat4(name, value); });
}

void OpenGLShader::SetMat4FromRenderThread(const std::string &name, const glm::mat4 &value, bool bind)
{
	if ( bind )
	{
		UploadUniformMat4(name, value);
	}
	else
	{
		const int location = glGetUniformLocation(m_RendererID, name.c_str());
		if ( location != -1 )
			UploadUniformMat4(location, value);
	}
}

void OpenGLShader::SetIntArray(const std::string &name, int *values, Uint32 size)
{
	Renderer::Submit([=]() { UploadUniformIntArray(name, values, size);	 });
}

void OpenGLShader::Load(const std::string &source)
{
	m_ShaderSource = PreProcess(source);
	if ( !m_IsCompute )
		Parse();

	Renderer::Submit([=]()
					 {
						 if ( m_RendererID )
							 glDeleteProgram(m_RendererID);

						 CompileAndUploadShader();
						 if ( !m_IsCompute )
						 {
							 ResolveUniforms();
							 ValidateUniforms();
						 }

						 if ( m_Loaded )
						 {
							 for ( auto &callback : m_ShaderReloadedCallbacks )
								 callback();
						 }

						 m_Loaded = true;
					 });
}

std::string OpenGLShader::ReadFromFile(const std::string &filepath) const
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if ( in )
	{
		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
	}
	else
	{
		SE_CORE_ASSERT(false, "Could not load shader from file! Filepath: " + filepath);
	}
	in.close();
	return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string &source)
{
	std::unordered_map<GLenum, std::string> shaderSources;

	const char *typeToken = "#type";
	const size_t typeTokenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0);
	while ( pos != std::string::npos )
	{
		const size_t eol = source.find_first_of("\r\n", pos);
		SE_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		const size_t begin = pos + typeTokenLength + 1;
		std::string type = source.substr(begin, eol - begin);
		SE_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel" || type == "compute", "Invalid shader type specified");

		const size_t nextLinePos = source.find_first_not_of("\r\n", eol);
		pos = source.find(typeToken, nextLinePos);
		auto shaderType = ShaderTypeFromString(type);
		shaderSources[shaderType] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

		// Compute shaders cannot contain other types
		if ( shaderType == GL_COMPUTE_SHADER )
		{
			m_IsCompute = true;
			break;
		}
	}

	return shaderSources;
}


void OpenGLShader::Parse()
{
	const char *token;
	const char *vStr;
	const char *fStr;

	m_Resources.clear();
	m_Structs.clear();
	m_VSMaterialUniformBuffer.Reset();
	m_PSMaterialUniformBuffer.Reset();

	auto &vertexSource = m_ShaderSource[GL_VERTEX_SHADER];
	auto &fragmentSource = m_ShaderSource[GL_FRAGMENT_SHADER];

	// Vertex Shader
	vStr = vertexSource.c_str();
	while ( (token = FindToken(vStr, "struct")) )
		ParseUniformStruct(GetBlock(token, &vStr), ShaderDomain::Vertex);

	vStr = vertexSource.c_str();
	while ( (token = FindToken(vStr, "uniform")) )
		ParseUniform(GetStatement(token, &vStr), ShaderDomain::Vertex);

	// Fragment Shader
	fStr = fragmentSource.c_str();
	while ( (token = FindToken(fStr, "struct")) )
		ParseUniformStruct(GetBlock(token, &fStr), ShaderDomain::Pixel);

	fStr = fragmentSource.c_str();
	while ( (token = FindToken(fStr, "uniform")) )
		ParseUniform(GetStatement(token, &fStr), ShaderDomain::Pixel);
}


void OpenGLShader::ParseUniform(const std::string &statement, ShaderDomain domain)
{
	std::vector<std::string> tokens = Tokenize(statement);
	Uint32 index = 0;

	index++; // "uniform"
	const std::string typeString = tokens[index++];
	std::string name = tokens[index++];
	// Strip ; from name if present
	if ( const char *s = strstr(name.c_str(), ";") )
		name = std::string(name.c_str(), s - name.c_str());

	const std::string n(name);
	Int32 count = 1;
	const char *nameStr = n.c_str();
	if ( const char *s = strstr(nameStr, "[") )
	{
		name = std::string(nameStr, s - nameStr);

		const char *end = strstr(nameStr, "]");
		const std::string c(s + 1, end - s);
		count = atoi(c.c_str());
	}

	if ( IsTypeStringResource(typeString) )
	{
		ShaderResourceDeclaration *declaration = new OpenGLShaderResourceDeclaration(OpenGLShaderResourceDeclaration::StringToType(typeString), name, count);
		m_Resources.push_back(declaration);
	}
	else
	{
		const OpenGLShaderUniformDeclaration::Type t = OpenGLShaderUniformDeclaration::StringToType(typeString);
		OpenGLShaderUniformDeclaration *declaration;

		if ( t == OpenGLShaderUniformDeclaration::Type::None )
		{
			// Find struct
			ShaderStruct *s = FindStruct(typeString);
			SE_CORE_ASSERT(s, "");
			declaration = new OpenGLShaderUniformDeclaration(domain, s, name, count);
		}
		else
		{
			declaration = new OpenGLShaderUniformDeclaration(domain, t, name, count);
		}

		if ( StartsWith(name, "r_") )
		{
			if ( domain == ShaderDomain::Vertex )
				dynamic_cast<OpenGLShaderUniformBufferDeclaration *>(m_VSRendererUniformBuffers.front())->PushUniform(declaration);
			else if ( domain == ShaderDomain::Pixel )
				dynamic_cast<OpenGLShaderUniformBufferDeclaration *>(m_PSRendererUniformBuffers.front())->PushUniform(declaration);
		}
		else
		{
			if ( domain == ShaderDomain::Vertex )
			{
				if ( !m_VSMaterialUniformBuffer )
					m_VSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDeclaration("", domain));

				m_VSMaterialUniformBuffer->PushUniform(declaration);
			}
			else if ( domain == ShaderDomain::Pixel )
			{
				if ( !m_PSMaterialUniformBuffer )
					m_PSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDeclaration("", domain));

				m_PSMaterialUniformBuffer->PushUniform(declaration);
			}
		}
	}
}

void OpenGLShader::ParseUniformStruct(const std::string &block, ShaderDomain domain)
{
	std::vector<std::string> tokens = Tokenize(block);

	Uint32 index = 0;
	index++; // struct
	std::string name = tokens[index++];
	auto *uniformStruct = new ShaderStruct(name);
	index++; // {
	while ( static_cast<size_t>(index) < tokens.size() )
	{
		if ( tokens[index] == "}" )
			break;

		std::string type = tokens[index++];
		// TODO: Look into why this is reassigning name
		std::string name = tokens[index++];

		// Strip ; from name if present
		if ( const char *s = strstr(name.c_str(), ";") )
			name = std::string(name.c_str(), s - name.c_str());

		Uint32 count = 1;
		const char *nameStr = name.c_str();
		if ( const char *s = strstr(nameStr, "[") )
		{
			name = std::string(nameStr, s - nameStr);

			const char *end = strstr(nameStr, "]");
			std::string c(s + 1, end - s);
			count = atoi(c.c_str());
		}
		ShaderUniformDeclaration *field = new OpenGLShaderUniformDeclaration(domain, OpenGLShaderUniformDeclaration::StringToType(type), name, count);
		uniformStruct->AddField(field);
	}
	m_Structs.push_back(uniformStruct);
}

ShaderStruct *OpenGLShader::FindStruct(const std::string &name)
{
	for ( ShaderStruct *s : m_Structs )
	{
		if ( s->GetName() == name )
			return s;
	}
	return nullptr;
}

Int32 OpenGLShader::GetUniformLocation(const std::string &name) const
{
	const Int32 result = glGetUniformLocation(m_RendererID, name.c_str());
	if ( result == -1 )
		SE_CORE_WARN("Could not find uniform '{0}' in shader {1}", name, m_AssetPath);

	return result;
}


void OpenGLShader::ResolveUniforms()
{
	//TODO: Improve this section and avoid repeating same code four times

	glUseProgram(m_RendererID);

	for ( auto *VSUniformBuffer : m_VSRendererUniformBuffers )
	{
		auto *decl = dynamic_cast<OpenGLShaderUniformBufferDeclaration *>(VSUniformBuffer);
		const auto &uniforms = decl->GetUniformDeclarations();
		for ( auto *uniform : uniforms )
		{
			auto *glUniform = dynamic_cast<OpenGLShaderUniformDeclaration *>(uniform);
			if ( glUniform->GetType() == OpenGLShaderUniformDeclaration::Type::Struct )
			{
				const auto &uniformStruct = glUniform->GetShaderUniformStruct();
				const auto &fields = uniformStruct.GetFields();
				for ( auto *field : fields )
				{
					auto *glField = dynamic_cast<OpenGLShaderUniformDeclaration *>(field);
					glField->m_Location = GetUniformLocation(glUniform->m_Name + "." + glField->m_Name);
				}
			}
			else
			{
				glUniform->m_Location = GetUniformLocation(glUniform->m_Name);
			}
		}
	}

	for ( auto *PSUniformBuffer : m_PSRendererUniformBuffers )
	{
		auto *decl = dynamic_cast<OpenGLShaderUniformBufferDeclaration *>(PSUniformBuffer);
		const auto &uniforms = decl->GetUniformDeclarations();
		for ( auto *uniform : uniforms )
		{
			auto *glUniform = dynamic_cast<OpenGLShaderUniformDeclaration *>(uniform);
			if ( glUniform->GetType() == OpenGLShaderUniformDeclaration::Type::Struct )
			{
				const auto &uniformStruct = glUniform->GetShaderUniformStruct();
				const auto &fields = uniformStruct.GetFields();
				for ( auto *field : fields )
				{
					auto *glField = dynamic_cast<OpenGLShaderUniformDeclaration *>(field);
					glField->m_Location = GetUniformLocation(glUniform->m_Name + "." + glField->m_Name);
				}
			}
			else
			{
				glUniform->m_Location = GetUniformLocation(glUniform->m_Name);
			}
		}
	}

	{
		const auto &decl = m_VSMaterialUniformBuffer;
		if ( decl )
		{
			const auto &uniforms = decl->GetUniformDeclarations();
			for ( auto *uniform : uniforms )
			{
				auto *glUniform = dynamic_cast<OpenGLShaderUniformDeclaration *>(uniform);
				if ( glUniform->GetType() == OpenGLShaderUniformDeclaration::Type::Struct )
				{
					const auto &uniformStruct = glUniform->GetShaderUniformStruct();
					const auto &fields = uniformStruct.GetFields();
					for ( auto *field : fields )
					{
						auto *glField = dynamic_cast<OpenGLShaderUniformDeclaration *>(field);
						glField->m_Location = GetUniformLocation(glUniform->m_Name + "." + glField->m_Name);
					}
				}
				else
				{
					glUniform->m_Location = GetUniformLocation(glUniform->m_Name);
				}
			}
		}
	}

	{
		const auto &decl = m_PSMaterialUniformBuffer;
		if ( decl )
		{
			const auto &uniforms = decl->GetUniformDeclarations();
			for ( auto *uniform : uniforms )
			{
				auto *glUniform = dynamic_cast<OpenGLShaderUniformDeclaration *>(uniform);
				if ( glUniform->GetType() == OpenGLShaderUniformDeclaration::Type::Struct )
				{
					const auto &uniformStruct = glUniform->GetShaderUniformStruct();
					const auto &fields = uniformStruct.GetFields();
					for ( auto *field : fields )
					{
						auto *glField = dynamic_cast<OpenGLShaderUniformDeclaration *>(field);
						glField->m_Location = GetUniformLocation(glUniform->m_Name + "." + glField->m_Name);
					}
				}
				else
				{
					glUniform->m_Location = GetUniformLocation(glUniform->m_Name);
				}
			}
		}
	}

	Uint32 sampler = 0;
	for ( auto *resource : m_Resources )
	{
		auto *glResource = dynamic_cast<OpenGLShaderResourceDeclaration *>(resource);
		const Int32 location = GetUniformLocation(glResource->m_Name);

		if ( glResource->GetCount() == 1 )
		{
			glResource->m_Register = sampler;
			if ( location != -1 )
				UploadUniformInt(location, sampler);

			sampler++;
		}
		else if ( resource->GetCount() > 1 )
		{
			glResource->m_Register = 0;
			const Uint32 count = glResource->GetCount();
			int *samplers = new int[count];
			for ( Uint32 s = 0; s < count; s++ )
				samplers[s] = s;
			UploadUniformIntArray(glResource->GetName(), samplers, count);
			delete[] samplers;
		}
	}
}

void OpenGLShader::ValidateUniforms()
{

}

void OpenGLShader::CompileAndUploadShader()
{
	std::vector<GLuint> shaderRendererIDs;

	const GLuint program = glCreateProgram();
	for ( auto &[type, source] : m_ShaderSource )
	{
		GLuint shaderRendererID = glCreateShader(type);
		const auto *source_cStr = static_cast<const GLchar *>(source.c_str());
		glShaderSource(shaderRendererID, 1, &source_cStr, nullptr);

		glCompileShader(shaderRendererID);

		GLint isCompiled = GL_FALSE;
		glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
		if ( isCompiled == GL_FALSE )
		{
			GLint maxLength = 0;
			glGetShaderiv(shaderRendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

			SE_CORE_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

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
	glGetProgramiv(program, GL_LINK_STATUS, static_cast<int *>(&isLinked));
	if ( isLinked == GL_FALSE )
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		SE_CORE_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		for ( auto id : shaderRendererIDs )
			glDeleteShader(id);
	}

	// Always detach shaders after a successful link.
	for ( auto id : shaderRendererIDs )
		glDetachShader(program, id);

	m_RendererID = program;
}

GLenum OpenGLShader::ShaderTypeFromString(const std::string &type)
{
	if ( type == "vertex" )
		return GL_VERTEX_SHADER;
	if ( type == "fragment" || type == "pixel" )
		return GL_FRAGMENT_SHADER;
	if ( type == "compute" )
		return GL_COMPUTE_SHADER;

	return GL_NONE;
}

void OpenGLShader::ResolveAndSetUniforms(const Shared<OpenGLShaderUniformBufferDeclaration> &decl, const Buffer &buffer)
{
	const auto &uniforms = decl->GetUniformDeclarations();
	for ( auto *uniform : uniforms )
	{
		auto *glUniform = dynamic_cast<OpenGLShaderUniformDeclaration *>(uniform);
		if ( glUniform->IsArray() )
			ResolveAndSetUniformArray(glUniform, buffer);
		else
			ResolveAndSetUniform(glUniform, buffer);
	}
}

void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration *uniform, const Buffer &buffer)
{
	if ( uniform->GetLocation() == -1 )
		return;

	SE_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

	const Uint32 offset = uniform->GetOffset();
	switch ( uniform->GetType() )
	{
	case OpenGLShaderUniformDeclaration::Type::Float32:
		UploadUniformFloat(uniform->GetLocation(), *reinterpret_cast<const float *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Int32:
		UploadUniformInt(uniform->GetLocation(), *reinterpret_cast<const int *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec2:
		UploadUniformFloat2(uniform->GetLocation(), *reinterpret_cast<const glm::vec2 *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec3:
		UploadUniformFloat3(uniform->GetLocation(), *reinterpret_cast<const glm::vec3 *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec4:
		UploadUniformFloat4(uniform->GetLocation(), *reinterpret_cast<const glm::vec4 *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat3:
		UploadUniformMat3(uniform->GetLocation(), *reinterpret_cast<const glm::mat3 *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat4:
		UploadUniformMat4(uniform->GetLocation(), *reinterpret_cast<const glm::mat4 *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Struct:
		UploadUniformStruct(uniform, buffer.Data(), offset);
		break;
	default:
		SE_CORE_ASSERT(false, "Unknown uniform type!");
	}
}

void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration *uniform, const Buffer &buffer)
{
	//SE_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

	Uint32 offset = uniform->GetOffset();
	switch ( uniform->GetType() )
	{
	case OpenGLShaderUniformDeclaration::Type::Float32:
		UploadUniformFloat(uniform->GetLocation(), *reinterpret_cast<const float *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Int32:
		UploadUniformInt(uniform->GetLocation(), *reinterpret_cast<const int *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec2:
		UploadUniformFloat2(uniform->GetLocation(), *reinterpret_cast<const glm::vec2 *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec3:
		UploadUniformFloat3(uniform->GetLocation(), *reinterpret_cast<const glm::vec3 *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec4:
		UploadUniformFloat4(uniform->GetLocation(), *reinterpret_cast<const glm::vec4 *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat3:
		UploadUniformMat3(uniform->GetLocation(), *reinterpret_cast<const glm::mat3 *>(&buffer.Data()[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat4:
		UploadUniformMat4Array(uniform->GetLocation(), *reinterpret_cast<const glm::mat4 *>(&buffer.Data()[offset]), uniform->GetCount());
		break;
	case OpenGLShaderUniformDeclaration::Type::Struct:
		UploadUniformStruct(uniform, buffer.Data(), offset);
		break;
	default:
		SE_CORE_ASSERT(false, "Unknown uniform type!");
	}
}

void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration &field, const Uint8 *data, Int32 offset)
{
	switch ( field.GetType() )
	{
	case OpenGLShaderUniformDeclaration::Type::Float32:
		UploadUniformFloat(field.GetLocation(), *reinterpret_cast<const float *>(&data[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Int32:
		UploadUniformInt(field.GetLocation(), *reinterpret_cast<const int *>(&data[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec2:
		UploadUniformFloat2(field.GetLocation(), *reinterpret_cast<const glm::vec2 *>(&data[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec3:
		UploadUniformFloat3(field.GetLocation(), *reinterpret_cast<const glm::vec3 *>(&data[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Vec4:
		UploadUniformFloat4(field.GetLocation(), *reinterpret_cast<const glm::vec4 *>(&data[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat3:
		UploadUniformMat3(field.GetLocation(), *reinterpret_cast<const glm::mat3 *>(&data[offset]));
		break;
	case OpenGLShaderUniformDeclaration::Type::Mat4:
		UploadUniformMat4(field.GetLocation(), *reinterpret_cast<const glm::mat4 *>(&data[offset]));
		break;
	default:
		SE_CORE_ASSERT(false, "Unknown uniform type!");
	}
}

void OpenGLShader::UploadUniformInt(Uint32 location, Int32 value)
{
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(Uint32 location, Int32 *values, Int32 count)
{
	glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(Uint32 location, float value)
{
	glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(Uint32 location, const glm::vec2 &value)
{
	glUniform2f(location, value.x, value.y);
}

void OpenGLShader::UploadUniformFloat3(Uint32 location, const glm::vec3 &value)
{
	glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::UploadUniformFloat4(Uint32 location, const glm::vec4 &value)
{
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::UploadUniformMat3(Uint32 location, const glm::mat3 &values)
{
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(values));
}

void OpenGLShader::UploadUniformMat4(Uint32 location, const glm::mat4 &values)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(values));
}

void OpenGLShader::UploadUniformMat4Array(Uint32 location, const glm::mat4 &values, Uint32 count)
{
	glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(values));
}

void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration *uniform, const Uint8 *buffer, Uint32 offset)
{
	const ShaderStruct &uniformStruct = uniform->GetShaderUniformStruct();
	const auto &fields = uniformStruct.GetFields();
	for ( auto *field : fields )
	{
		auto *glField = dynamic_cast<OpenGLShaderUniformDeclaration *>(field);
		ResolveAndSetUniformField(*glField, buffer, offset);
		offset += glField->m_Size;
	}
}

void OpenGLShader::UploadUniformInt(const std::string &name, Int32 value)
{
	const Int32 location = GetUniformLocation(name);
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(const std::string &name, Int32 *values, Uint32 count)
{
	const Int32 location = GetUniformLocation(name);
	glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(const std::string &name, float value)
{
	glUseProgram(m_RendererID);
	const auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if ( location != -1 )
		glUniform1f(location, value);
	else
		SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformFloat2(const std::string &name, const glm::vec2 &values)
{
	glUseProgram(m_RendererID);
	const auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if ( location != -1 )
		glUniform2f(location, values.x, values.y);
	else
		SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformFloat3(const std::string &name, const glm::vec3 &values)
{
	glUseProgram(m_RendererID);
	const auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if ( location != -1 )
		glUniform3f(location, values.x, values.y, values.z);
	else
		SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformFloat4(const std::string &name, const glm::vec4 &values)
{
	glUseProgram(m_RendererID);
	const auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if ( location != -1 )
		glUniform4f(location, values.x, values.y, values.z, values.w);
	else
		SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformMat3(const std::string &name, const glm::vec4 &values)
{
	glUseProgram(m_RendererID);
	const auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if ( location != -1 )
		glUniformMatrix3fv(location, 1, GL_FALSE, reinterpret_cast<const float *>(&values));
	else
		SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}

void OpenGLShader::UploadUniformMat4(const std::string &name, const glm::mat4 &values)
{
	glUseProgram(m_RendererID);
	const auto location = glGetUniformLocation(m_RendererID, name.c_str());
	if ( location != -1 )
		glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const float *>(&values));
	else
		SE_LOG_UNIFORM("Uniform '{0}' not found!", name);
}
}