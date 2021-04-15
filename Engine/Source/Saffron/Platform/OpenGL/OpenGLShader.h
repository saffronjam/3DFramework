#pragma once

#include <glad/glad.h>

#include "Saffron/Rendering/Resources/Shader.h"
#include "Saffron/Platform/OpenGL/OpenGLShaderUniform.h"

namespace Se
{
class OpenGLShader : public Shader
{
public:
	OpenGLShader() = default;
	OpenGLShader(const Filepath& filepath);
	static Shared<OpenGLShader> Create(const Buffer& source);

	void Reload() override;
	void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;

	void Bind() override;

	RendererID GetRendererID() const override { return m_RendererID; }

	void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) override;

	void SetVSMaterialUniformBuffer(Buffer buffer) override;
	void SetPSMaterialUniformBuffer(Buffer buffer) override;

	void SetInt(const std::string& name, int value) override;
	void SetBool(const std::string& name, bool value) override;
	void SetFloat(const std::string& name, float value) override;
	void SetFloat2(const std::string& name, const glm::vec2& value) override;
	void SetFloat3(const std::string& name, const glm::vec3& value) override;
	void SetMat4(const std::string& name, const glm::mat4& value) override;
	void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind = true) override;

	void SetIntArray(const std::string& name, int* values, uint32_t size) override;

	const std::string& GetName() const override { return m_Name; }

private:
	void Load(const Buffer& source);

	Buffer ReadShaderFromFile(const std::string& filepath) const;
	std::unordered_map<GLenum, std::string> PreProcess(const Buffer& source);
	void Parse();
	void ParseUniform(const std::string& statement, ShaderDomain domain);
	void ParseUniformStruct(const std::string& block, ShaderDomain domain);
	ShaderStruct* FindStruct(const std::string& name);

	int32_t GetUniformLocation(const std::string& name) const;

	void ResolveUniforms();
	void ValidateUniforms();
	void CompileAndUploadShader();
	static GLenum ShaderTypeFromString(const std::string& type);

	void ResolveAndSetUniforms(const Shared<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer);
	void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
	void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
	void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, Uint8* data, int32_t offset);

	void UploadUniformInt(uint32_t location, int32_t value);
	void UploadUniformIntArray(uint32_t location, int32_t* values, int32_t count);
	void UploadUniformFloat(uint32_t location, float value);
	void UploadUniformFloat2(uint32_t location, const glm::vec2& value);
	void UploadUniformFloat3(uint32_t location, const glm::vec3& value);
	void UploadUniformFloat4(uint32_t location, const glm::vec4& value);
	void UploadUniformMat3(uint32_t location, const glm::mat3& values);
	void UploadUniformMat4(uint32_t location, const glm::mat4& values);
	void UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count);

	void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, Uint8* buffer, uint32_t offset);

	void UploadUniformInt(const std::string& name, int32_t value);
	void UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count);

	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

	void UploadUniformMat4(const std::string& name, const glm::mat4& value);

	const ShaderUniformBufferList& GetVSRendererUniforms() const override;
	const ShaderUniformBufferList& GetPSRendererUniforms() const override;

	bool HasVSMaterialUniformBuffer() const override;
	bool HasPSMaterialUniformBuffer() const override;

	const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override;
	const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const override;

	const ShaderResourceList& GetResources() const override;
private:
	RendererID m_RendererID = 0;
	bool m_Loaded = false;
	bool m_IsCompute = false;

	std::string m_Name, m_AssetPath;
	std::unordered_map<GLenum, std::string> m_ShaderSource;

	std::vector<ShaderReloadedCallback> m_ShaderReloadedCallbacks;

	ShaderUniformBufferList m_VSRendererUniformBuffers;
	ShaderUniformBufferList m_PSRendererUniformBuffers;
	Shared<OpenGLShaderUniformBufferDeclaration> m_VSMaterialUniformBuffer;
	Shared<OpenGLShaderUniformBufferDeclaration> m_PSMaterialUniformBuffer;
	ShaderResourceList m_Resources;
	ShaderStructList m_Structs;
};
}
