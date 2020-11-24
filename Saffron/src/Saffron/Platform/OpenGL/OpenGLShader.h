#pragma once

#include "Saffron/Renderer/Shader.h"
#include <glad/glad.h>

#include "OpenGLShaderUniform.h"

namespace Se {

class OpenGLShader : public Shader
{
public:
	OpenGLShader() = default;
	OpenGLShader(const Filepath &filepath);
	OpenGLShader(const String &source);

	void Bind() override;

	void Reload() override;

	void UploadUniformBuffer(const Uniform::BufferBase &uniformBuffer) override;

	RendererID GetRendererID() const override { return m_RendererID; }
	const String &GetName() const override { return m_Name; }
	size_t GetIdentifier() override;

	void SetVSMaterialUniformBuffer(const Buffer &buffer) override;
	void SetPSMaterialUniformBuffer(const Buffer &buffer) override;
	void SetFloat(const String &name, float value) override;
	void SetInt(const String &name, int value) override;
	void SetFloat3(const String &name, const Vector3f &value) override;
	void SetMat4(const String &name, const Matrix4f &value) override;
	void SetMat4FromRenderThread(const String &name, const Matrix4f &value, bool bind = true) override;
	void SetIntArray(const String &name, int *values, Uint32 size) override;

private:
	void Load(const String &source);

	String ReadFromFile(const Filepath &filepath) const;
	std::unordered_map<GLenum, String> PreProcess(const String &source);
	void Parse();
	void ParseUniform(const String &statement, ShaderDomain domain);
	void ParseUniformStruct(const String &block, ShaderDomain domain);
	ShaderStruct *FindStruct(const String &name);

	Int32 GetUniformLocation(const String &name) const;

	void ResolveUniforms();
	void ValidateUniforms();
	void CompileAndUploadShader();
	static GLenum ShaderTypeFromString(const String &type);

	void ResolveAndSetUniforms(const Shared<OpenGLShaderUniformBufferDeclaration> &decl, const Buffer &buffer);
	void ResolveAndSetUniform(OpenGLShaderUniformDeclaration *uniform, const Buffer &buffer);
	void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration *uniform, const Buffer &buffer);
	void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration &field, const Uint8 *data, Int32 offset);

	void UploadUniformInt(Uint32 location, Int32 value);
	void UploadUniformIntArray(Uint32 location, Int32 *values, Int32 count);
	void UploadUniformFloat(Uint32 location, float value);
	void UploadUniformFloat2(Uint32 location, const Vector2f &value);
	void UploadUniformFloat3(Uint32 location, const Vector3f &value);
	void UploadUniformFloat4(Uint32 location, const Vector4f &value);
	void UploadUniformMat3(Uint32 location, const Matrix3f &values);
	void UploadUniformMat4(Uint32 location, const Matrix4f &values);
	void UploadUniformMat4Array(Uint32 location, const Matrix4f &values, Uint32 count);

	void UploadUniformInt(const String &name, Int32 value);
	void UploadUniformIntArray(const String &name, Int32 *values, Uint32 count);
	void UploadUniformFloat(const String &name, float value);
	void UploadUniformFloat2(const String &name, const Vector2f &values);
	void UploadUniformFloat3(const String &name, const Vector3f &values);
	void UploadUniformFloat4(const String &name, const Vector4f &values);
	void UploadUniformMat3(const String &name, const Vector4f &values);
	void UploadUniformMat4(const String &name, const Matrix4f &values);

	void UploadUniformStruct(OpenGLShaderUniformDeclaration *uniform, const Uint8 *buffer, Uint32 offset);

	const ShaderUniformBufferDeclaration::List &GetVSRendererUniforms() const override { return m_VSRendererUniformBuffers; }
	const ShaderUniformBufferDeclaration::List &GetPSRendererUniforms() const override { return m_PSRendererUniformBuffers; }
	const ShaderUniformBufferDeclaration &GetVSMaterialUniformBuffer() const override { return *m_VSMaterialUniformBuffer; }
	const ShaderUniformBufferDeclaration &GetPSMaterialUniformBuffer() const override { return *m_PSMaterialUniformBuffer; }
	const ShaderResourceDeclaration::List &GetResources() const override { return m_Resources; }

	bool HasVSMaterialUniformBuffer() const override { return static_cast<bool>(m_VSMaterialUniformBuffer); }
	bool HasPSMaterialUniformBuffer() const override { return static_cast<bool>(m_PSMaterialUniformBuffer); }
private:
	RendererID m_RendererID = 0;
	bool m_Loaded = false;
	bool m_IsCompute = false;

	String m_Name;
	Filepath m_Filepath;
	UnorderedMap<GLenum, String> m_ShaderSource;

	ShaderUniformBufferDeclaration::List m_VSRendererUniformBuffers;
	ShaderUniformBufferDeclaration::List m_PSRendererUniformBuffers;
	Shared<OpenGLShaderUniformBufferDeclaration> m_VSMaterialUniformBuffer;
	Shared<OpenGLShaderUniformBufferDeclaration> m_PSMaterialUniformBuffer;
	ShaderResourceDeclaration::List m_Resources;
	ShaderStruct::List m_Structs;

};

}