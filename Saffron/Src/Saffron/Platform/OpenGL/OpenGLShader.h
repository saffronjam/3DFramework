#pragma once

#include "glad/glad.h"

#include "Saffron/Renderer/Shader.h"
#include "Saffron/Platform/OpenGL/OpenGLShaderUniform.h"

namespace Se
{
class OpenGLShader : public Shader
{
public:
	OpenGLShader() = default;
	OpenGLShader(const std::string &filepath);
	static Ref<OpenGLShader> CreateFromString(const std::string &source);

	void Bind() override;

	void Reload() override;
	void AddShaderReloadedCallback(const ShaderReloadedCallback &callback) override;

	RendererID GetRendererID() const override { return m_RendererID; }
	const std::string &GetName() const override { return m_Name; }

	void UploadUniformBuffer(const Uniform::BufferBase &uniformBuffer) override;
	void SetVSMaterialUniformBuffer(const Buffer &buffer) override;
	void SetPSMaterialUniformBuffer(const Buffer &buffer) override;
	void SetFloat(const std::string &name, float value) override;
	void SetInt(const std::string &name, int value) override;
	void SetFloat3(const std::string &name, const glm::vec3 &value) override;
	void SetMat4(const std::string &name, const glm::mat4 &value) override;
	void SetMat4FromRenderThread(const std::string &name, const glm::mat4 &value, bool bind = true) override;
	void SetIntArray(const std::string &name, int *values, Uint32 size) override;

private:
	void Load(const std::string &source);

	std::string ReadFromFile(const std::string &filepath) const;
	std::unordered_map<GLenum, std::string> PreProcess(const std::string &source);
	void Parse();
	void ParseUniform(const std::string &statement, ShaderDomain domain);
	void ParseUniformStruct(const std::string &block, ShaderDomain domain);
	ShaderStruct *FindStruct(const std::string &name);

	Int32 GetUniformLocation(const std::string &name) const;

	void ResolveUniforms();
	void ValidateUniforms();
	void CompileAndUploadShader();
	static GLenum ShaderTypeFromString(const std::string &type);

	void ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDeclaration> &decl, Buffer buffer);
	void ResolveAndSetUniform(OpenGLShaderUniformDeclaration *uniform, const Buffer &buffer);
	void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration *uniform, const Buffer &buffer);
	void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration &field, const Uint8 *data, Int32 offset);

	void UploadUniformInt(Uint32 location, Int32 value);
	void UploadUniformIntArray(Uint32 location, Int32 *values, Int32 count);
	void UploadUniformFloat(Uint32 location, float value);
	void UploadUniformFloat2(Uint32 location, const glm::vec2 &value);
	void UploadUniformFloat3(Uint32 location, const glm::vec3 &value);
	void UploadUniformFloat4(Uint32 location, const glm::vec4 &value);
	void UploadUniformMat3(Uint32 location, const glm::mat3 &values);
	void UploadUniformMat4(Uint32 location, const glm::mat4 &values);
	void UploadUniformMat4Array(Uint32 location, const glm::mat4 &values, Uint32 count);

	void UploadUniformStruct(OpenGLShaderUniformDeclaration *uniform, const Uint8 *buffer, Uint32 offset);

	void UploadUniformInt(const std::string &name, Int32 value);
	void UploadUniformIntArray(const std::string &name, Int32 *values, Uint32 count);

	void UploadUniformFloat(const std::string &name, float value);
	void UploadUniformFloat2(const std::string &name, const glm::vec2 &values);
	void UploadUniformFloat3(const std::string &name, const glm::vec3 &values);
	void UploadUniformFloat4(const std::string &name, const glm::vec4 &values);

	void UploadUniformMat4(const std::string &name, const glm::mat4 &values);

	const ShaderUniformBufferDeclaration::List &GetVSRendererUniforms() const override { return m_VSRendererUniformBuffers; }
	const ShaderUniformBufferDeclaration::List &GetPSRendererUniforms() const override { return m_PSRendererUniformBuffers; }
	bool HasVSMaterialUniformBuffer() const override { return static_cast<bool>(m_VSMaterialUniformBuffer); }
	bool HasPSMaterialUniformBuffer() const override { return static_cast<bool>(m_PSMaterialUniformBuffer); }
	const ShaderUniformBufferDeclaration &GetVSMaterialUniformBuffer() const override { return *m_VSMaterialUniformBuffer; }
	const ShaderUniformBufferDeclaration &GetPSMaterialUniformBuffer() const override { return *m_PSMaterialUniformBuffer; }
	const ShaderResourceDeclaration::List &GetResources() const override { return m_Resources; }
private:
	RendererID m_RendererID = 0;
	bool m_Loaded = false;
	bool m_IsCompute = false;

	std::string m_Name, m_AssetPath;
	std::unordered_map<GLenum, std::string> m_ShaderSource;

	std::vector<ShaderReloadedCallback> m_ShaderReloadedCallbacks;

	ShaderUniformBufferDeclaration::List m_VSRendererUniformBuffers;
	ShaderUniformBufferDeclaration::List m_PSRendererUniformBuffers;
	Ref<OpenGLShaderUniformBufferDeclaration> m_VSMaterialUniformBuffer;
	Ref<OpenGLShaderUniformBufferDeclaration> m_PSMaterialUniformBuffer;
	ShaderResourceDeclaration::List m_Resources;
	ShaderStruct::List m_Structs;
};
}

