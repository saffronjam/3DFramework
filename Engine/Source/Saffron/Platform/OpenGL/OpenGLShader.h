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

	RendererID GetRendererID() const override { return _rendererID; }

	void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) override;

	void SetVSMaterialUniformBuffer(Buffer buffer) override;
	void SetPSMaterialUniformBuffer(Buffer buffer) override;

	void SetInt(const String& name, int value) override;
	void SetBool(const String& name, bool value) override;
	void SetFloat(const String& name, float value) override;
	void SetFloat2(const String& name, const Vector2& value) override;
	void SetFloat3(const String& name, const Vector3& value) override;
	void SetMat4(const String& name, const Matrix4& value) override;
	void SetMat4FromRenderThread(const String& name, const Matrix4& value, bool bind = true) override;

	void SetIntArray(const String& name, int* values, uint size) override;

	const String& GetName() const override { return _name; }

private:
	void Load(const Buffer& source);

	Buffer ReadShaderFromFile(const Filepath& filepath) const;
	HashMap<GLenum, String> PreProcess(const Buffer& source);
	void Parse();
	void ParseUniform(const String& statement, ShaderDomain domain);
	void ParseUniformStruct(const String& block, ShaderDomain domain);
	ShaderStruct* FindStruct(const String& name);

	int32_t GetUniformLocation(const String& name) const;

	void ResolveUniforms();
	void ValidateUniforms();
	void CompileAndUploadShader();
	static GLenum ShaderTypeFromString(const String& type);

	void ResolveAndSetUniforms(const Shared<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer);
	void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
	void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
	void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, uchar* data, int32_t offset);

	void UploadUniformInt(uint location, int32_t value);
	void UploadUniformIntArray(uint location, int32_t* values, int32_t count);
	void UploadUniformFloat(uint location, float value);
	void UploadUniformFloat2(uint location, const Vector2& value);
	void UploadUniformFloat3(uint location, const Vector3& value);
	void UploadUniformFloat4(uint location, const Vector4& value);
	void UploadUniformMat3(uint location, const Matrix3& values);
	void UploadUniformMat4(uint location, const Matrix4& values);
	void UploadUniformMat4Array(uint location, const Matrix4& values, uint count);

	void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, uchar* buffer, uint offset);

	void UploadUniformInt(const String& name, int32_t value);
	void UploadUniformIntArray(const String& name, int32_t* values, uint count);

	void UploadUniformFloat(const String& name, float value);
	void UploadUniformFloat2(const String& name, const Vector2& value);
	void UploadUniformFloat3(const String& name, const Vector3& value);
	void UploadUniformFloat4(const String& name, const Vector4& value);

	void UploadUniformMat4(const String& name, const Matrix4& value);

	const ShaderUniformBufferList& GetVSRendererUniforms() const override;
	const ShaderUniformBufferList& GetPSRendererUniforms() const override;

	bool HasVSMaterialUniformBuffer() const override;
	bool HasPSMaterialUniformBuffer() const override;

	const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override;
	const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const override;

	const ShaderResourceList& GetResources() const override;
private:
	RendererID _rendererID = 0;
	bool _loaded = false;
	bool _isCompute = false;

	String _name;
	HashMap<GLenum, String> _shaderSource;

	ArrayList<ShaderReloadedCallback> _shaderReloadedCallbacks;

	ShaderUniformBufferList _vSRendererUniformBuffers;
	ShaderUniformBufferList _pSRendererUniformBuffers;
	Shared<OpenGLShaderUniformBufferDeclaration> _vSMaterialUniformBuffer;
	Shared<OpenGLShaderUniformBufferDeclaration> _pSMaterialUniformBuffer;
	ShaderResourceList _resources;
	ShaderStructList _structs;
};
}
