#include "Saffron/SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/System/Log.h"
#include "Saffron/System/SaffronMath.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Se
{

static Uint32 ShaderTypeFromString(const std::string &type)
{
	if ( type == "vertex" )
		return GL_VERTEX_SHADER;
	if ( type == "fragment" || type == "pixel" )
		return GL_FRAGMENT_SHADER;

	SE_CORE_ASSERT(false, "Unknown shader type!");
	return 0;
}

struct OpenGLShader::UniformBindHelper
{
	UniformBindHelper(const OpenGLShader &shader, Int32 m_RendererID, const std::string &name)
		: savedProgram(0),
		currentProgram(m_RendererID),
		location(-1)
	{
		if ( currentProgram )
		{
			// Enable program object
			glGetIntegerv(GL_CURRENT_PROGRAM, &savedProgram);
			if ( currentProgram != savedProgram )
				glUseProgram(currentProgram);

			// Store uniform location for further use outside constructor
			location = shader.GetUniformLocation(name);
		}
	}

	~UniformBindHelper()
	{
		// Disable program object
		if ( currentProgram && (currentProgram != savedProgram) )
			glUseProgram(savedProgram);
	}

	Int32 savedProgram;
	Uint32 currentProgram;
	Int32 location;
};

OpenGLShader::OpenGLShader(const std::string &filepath)
{
	//SE_PROFILE_FUNCTION();

	const auto source = ReadFile(filepath);
	const auto shaderSources = PreProcess(source);
	Compile(shaderSources);

	// Extract name from filepath
	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	const auto lastDot = filepath.rfind('.');
	const auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);
}

OpenGLShader::OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc)
	: m_Name(name)
{
	//SE_PROFILE_FUNCTION();

	std::unordered_map<Uint32, std::string> sources;
	sources[GL_VERTEX_SHADER] = vertexSrc;
	sources[GL_FRAGMENT_SHADER] = fragmentSrc;
	Compile(sources);
}

OpenGLShader::~OpenGLShader()
{
	//SE_PROFILE_FUNCTION();

	glDeleteProgram(m_RendererID);
}

std::string OpenGLShader::ReadFile(const std::string &filepath)
{
	//SE_PROFILE_FUNCTION();

	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if ( in )
	{
		in.seekg(0, std::ios::end);
		const size_t size = in.tellg();
		if ( size != -1 )
		{
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(&result[0], size);
			in.close();
		}
		else
		{
			SE_CORE_ERROR("Could not read from file '{0}'", filepath);
		}
	}
	else
	{
		SE_CORE_ERROR("Could not open file '{0}'", filepath);
	}

	return result;
}

std::unordered_map<Uint32, std::string> OpenGLShader::PreProcess(const std::string &source)
{
	//SE_PROFILE_FUNCTION();

	std::unordered_map<Uint32, std::string> shaderSources;

	const char *typeToken = "#type";
	const size_t typeTokenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0); //Start of shader type declaration line

	while ( pos != std::string::npos )
	{
		//End of shader type declaration line
		const size_t fileEnd = source.find_first_of("\r\n", pos);
		SE_CORE_ASSERT(eol != std::string::npos, "Syntax error");

		//Start of shader type name (after "#type " keyword)
		const size_t begin = pos + typeTokenLength + 1;
		std::string type = source.substr(begin, fileEnd - begin);
		SE_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

		//Start of shader code after shader type declaration line
		const size_t nextLinePos = source.find_first_not_of("\r\n", fileEnd);
		SE_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");

		//Start of next shader type declaration line
		pos = source.find(typeToken, nextLinePos);
		shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
	}

	return shaderSources;
}

void OpenGLShader::Compile(const std::unordered_map<Uint32, std::string> &shaderSources)
{
	//SE_PROFILE_FUNCTION();

	const Uint32 program = glCreateProgram();
	SE_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
	std::array<Uint32, 2> glShaderIDs;
	int glShaderIDIndex = 0;
	for ( const auto &kv : shaderSources )
	{
		const Uint32 type = kv.first;
		const std::string &source = kv.second;

		const GLuint shader = glCreateShader(type);

		const GLchar *sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, nullptr);

		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if ( isCompiled == GL_FALSE )
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			SE_CORE_ERROR("{0}", infoLog.data());
			SE_CORE_ASSERT(false, "Shader compilation failure!");
			break;
		}

		glAttachShader(program, shader);
		glShaderIDs[glShaderIDIndex++] = shader;
	}

	m_RendererID = program;

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

		// We don't need the program anymore.
		glDeleteProgram(program);

		for ( auto id : glShaderIDs )
			glDeleteShader(id);

		SE_CORE_ERROR("{0}", infoLog.data());
		SE_CORE_ASSERT(false, "Shader link failure!");
		return;
	}

	for ( auto id : glShaderIDs )
	{
		glDetachShader(program, id);
		glDeleteShader(id);
	}
}

void OpenGLShader::Bind() const
{
	//SE_PROFILE_FUNCTION();

	glUseProgram(m_RendererID);
}

void OpenGLShader::Unbind() const
{
	//SE_PROFILE_FUNCTION();

	glUseProgram(0);
}

void OpenGLShader::SetInt(const std::string &name, int value)
{
	//SE_PROFILE_FUNCTION();

	UploadUniformInt(name, value);
}

void OpenGLShader::SetIntArray(const std::string &name, int *values, uint32_t count)
{
	UploadUniformIntArray(name, values, count);
}

void OpenGLShader::SetFloat(const std::string &name, float value)
{
	//SE_PROFILE_FUNCTION();

	UploadUniformFloat(name, value);
}

void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value)
{
	//SE_PROFILE_FUNCTION();

	UploadUniformFloat3(name, value);
}

void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &value)
{
	//SE_PROFILE_FUNCTION();

	UploadUniformFloat4(name, value);
}

void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &value)
{
	//SE_PROFILE_FUNCTION();

	UploadUniformMat4(name, value);
}

void OpenGLShader::UploadUniformInt(const std::string &name, int value)
{
	const UniformBindHelper helper(*this, m_RendererID, name);
	if ( helper.location != -1 )
		glUniform1i(helper.location, value);
}

void OpenGLShader::UploadUniformIntArray(const std::string &name, int *values, uint32_t count)
{
	const UniformBindHelper helper(*this, m_RendererID, name);
	if ( helper.location != -1 )
		glUniform1iv(helper.location, count, values);
}

void OpenGLShader::UploadUniformFloat(const std::string &name, float value)
{
	const UniformBindHelper helper(*this, m_RendererID, name);
	if ( helper.location != -1 )
		glUniform1f(helper.location, value);
}

void OpenGLShader::UploadUniformFloat2(const std::string &name, const glm::vec2 &value)
{
	const UniformBindHelper helper(*this, m_RendererID, name);
	if ( helper.location != -1 )
		glUniform2f(helper.location, value.x, value.y);
}

void OpenGLShader::UploadUniformFloat3(const std::string &name, const glm::vec3 &value)
{
	const UniformBindHelper helper(*this, m_RendererID, name);
	if ( helper.location != -1 )
		glUniform3f(helper.location, value.x, value.y, value.z);
}

void OpenGLShader::UploadUniformFloat4(const std::string &name, const glm::vec4 &value)
{
	const UniformBindHelper helper(*this, m_RendererID, name);
	if ( helper.location != -1 )
		glUniform4f(helper.location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::UploadUniformMat3(const std::string &name, const glm::mat3 &matrix)
{
	const UniformBindHelper helper(*this, m_RendererID, name);
	if ( helper.location != -1 )
		glUniformMatrix3fv(helper.location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformMat4(const std::string &name, const glm::mat4 &matrix)
{
	const UniformBindHelper helper(*this, m_RendererID, name);
	if ( helper.location != -1 )
		glUniformMatrix4fv(helper.location, 1, GL_FALSE, glm::value_ptr(matrix));
}

Int32 OpenGLShader::GetUniformLocation(const std::string &name) const
{
	// Check the cache
	const auto it = m_UniformLocationCache.find(name);
	if ( it != m_UniformLocationCache.end() )
		// Already in cache, return it
		return it->second;

	// Not in cache, request the location from OpenGL
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	m_UniformLocationCache.insert(std::make_pair(name, location));

	if ( location == -1 )
		SE_WARN("Uniform \"{0}\" not found in shader program (ID: {1})", name.c_str(), m_RendererID);

	return location;
}

}
