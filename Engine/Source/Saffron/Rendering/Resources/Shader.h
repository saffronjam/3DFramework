#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Rendering/RendererApi.h"
#include "Saffron/Rendering/Resource.h"
#include "Saffron/Rendering/Resources/ShaderUniform.h"


namespace Se
{
struct ShaderUniform
{
};

struct ShaderUniformCollection
{
};

enum class UniformType
{
	None = 0,
	Float,
	Float2,
	Float3,
	Float4,
	Matrix3x3,
	Matrix4x4,
	Int32,
	Uint32
};

struct UniformDecl
{
	UniformType Type;
	std::ptrdiff_t Offset;
	String Name;
};

struct UniformBuffer
{
	// TODO: This currently represents a byte buffer that has been
	// packed with uniforms. This was primarily created for OpenGL,
	// and needs to be revisted for other rendering APIs. Furthermore,
	// this currently does not assume any alignment. This also has
	// nothing to do with GL uniform buffers, this is simply a CPU-side
	// buffer abstraction.
	Uint8* Buffer;
	ArrayList<UniformDecl> Uniforms;
};

struct UniformBufferBase
{
	virtual const Uint8* GetBuffer() const = 0;
	virtual const UniformDecl* GetUniforms() const = 0;
	virtual unsigned int GetUniformCount() const = 0;
};

template <unsigned int N, unsigned int U>
struct UniformBufferDeclaration : UniformBufferBase
{
	Uint8 Buffer[N];
	UniformDecl Uniforms[U];
	std::ptrdiff_t Cursor = 0;
	int Index = 0;

	const Uint8* GetBuffer() const override { return Buffer; }

	const UniformDecl* GetUniforms() const override { return Uniforms; }

	unsigned int GetUniformCount() const override { return U; }

	template <typename T>
	void Push(const String& name, const T& data)
	{
	}

	template <>
	void Push(const String& name, const float& data)
	{
		Uniforms[Index++] = {UniformType::Float, Cursor, name};
		memcpy(Buffer + Cursor, &data, sizeof(float));
		Cursor += sizeof(float);
	}

	template <>
	void Push(const String& name, const Vector3f& data)
	{
		Uniforms[Index++] = {UniformType::Float3, Cursor, name};
		memcpy(Buffer + Cursor, value_ptr(data), sizeof(Vector3f));
		Cursor += sizeof(Vector3f);
	}

	template <>
	void Push(const String& name, const Vector4f& data)
	{
		Uniforms[Index++] = {UniformType::Float4, Cursor, name};
		memcpy(Buffer + Cursor, value_ptr(data), sizeof(Vector4f));
		Cursor += sizeof(Vector4f);
	}

	template <>
	void Push(const String& name, const Matrix4f& data)
	{
		Uniforms[Index++] = {UniformType::Matrix4x4, Cursor, name};
		memcpy(Buffer + Cursor, value_ptr(data), sizeof(Matrix4f));
		Cursor += sizeof(Matrix4f);
	}
};

class Shader : public Resource
{
public:
	using ShaderReloadedCallback = Function<void()>;

	static void OnGuiRender();

	virtual void Reload() = 0;
	virtual void Bind() = 0;

	size_t GetResourceID() override;

	virtual RendererID GetRendererID() const = 0;
	virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) = 0;

	// Temporary while we don't have materials
	virtual void SetFloat(const String& name, float value) = 0;
	virtual void SetInt(const String& name, int value) = 0;
	virtual void SetBool(const String& name, bool value) = 0;
	virtual void SetFloat2(const String& name, const Vector2f& value) = 0;
	virtual void SetFloat3(const String& name, const Vector3f& value) = 0;
	virtual void SetMat4(const String& name, const Matrix4f& value) = 0;
	virtual void SetMat4FromRenderThread(const String& name, const Matrix4f& value, bool bind = true) = 0;

	virtual void SetIntArray(const String& name, int* values, Uint32 size) = 0;

	virtual const String& GetName() const = 0;

	// Represents a complete shader program stored in a single file.
	// Note: currently for simplicity this is simply a string filepath, however
	//       in the future this will be an asset object + metadata
	static Shared<Shader> Create(Filepath filepath);
	static Shared<Shader> Create(const Buffer& source);

	virtual void SetVSMaterialUniformBuffer(Buffer buffer) = 0;
	virtual void SetPSMaterialUniformBuffer(Buffer buffer) = 0;

	virtual const ShaderUniformBufferList& GetVSRendererUniforms() const = 0;
	virtual const ShaderUniformBufferList& GetPSRendererUniforms() const = 0;
	virtual bool HasVSMaterialUniformBuffer() const = 0;
	virtual bool HasPSMaterialUniformBuffer() const = 0;
	virtual const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const = 0;
	virtual const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const = 0;

	virtual const ShaderResourceList& GetResources() const = 0;

	virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) = 0;

public:
	mutable EventSubscriberList<void> Reloaded;

protected:
	Filepath m_Filepath;

private:
	static constexpr const char* ShaderLocation = "Assets/Shaders/";
	static constexpr const char* ShaderExtension = ".glsl";
};
}
