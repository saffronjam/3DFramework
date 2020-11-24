#pragma once


#include <unordered_map>

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/Renderer/ShaderUniform.h"
#include "Saffron/Resource/Resource.h"

namespace Se
{
class Shader : public Resource, public Signaller
{
public:
	struct Signals
	{
		static SignalAggregate<void> OnReload;
	};

	struct Uniform
	{
		enum class Type
		{
			None = 0,
			Float, Float2, Float3, Float4,
			Matrix3x3, Matrix4x4,
			Int32, Uint32
		};

		struct Declaration
		{
			Type DeclarationType;
			std::ptrdiff_t Offset;
			String Name;
		};

		struct Buffer
		{
			Uint8 *Data;
			ArrayList<Declaration> Uniforms;
		};

		struct BufferBase
		{
			virtual ~BufferBase() = default;
			virtual const Uint8 *GetBuffer() const = 0;
			virtual const Declaration *GetUniforms() const = 0;
			virtual unsigned int GetUniformCount() const = 0;
		};

		template<unsigned int N, unsigned int U>
		struct BufferDeclaration : BufferBase
		{
			Uint8 Buffer[N]{};
			Declaration Uniforms[U];
			std::ptrdiff_t Cursor = 0;
			int Index = 0;

			const Uint8 *GetBuffer() const override { return Buffer; }
			const Declaration *GetUniforms() const override { return Uniforms; }
			unsigned int GetUniformCount() const override { return U; }

			template<typename T>
			void Push(const String &name, const T &data) {}

			template<>
			void Push(const String &name, const float &data)
			{
				Uniforms[Index++] = { Type::Float, Cursor, name };
				memcpy(Buffer + Cursor, &data, sizeof(float));
				Cursor += sizeof(float);
			}

			template<>
			void Push(const String &name, const Vector3f &data)
			{
				Uniforms[Index++] = { Type::Float3, Cursor, name };
				memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(Vector3f));
				Cursor += sizeof(Vector3f);
			}

			template<>
			void Push(const String &name, const Vector4f &data)
			{
				Uniforms[Index++] = { Type::Float4, Cursor, name };
				memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(Vector4f));
				Cursor += sizeof(Vector4f);
			}

			template<>
			void Push(const String &name, const Matrix4f &data)
			{
				Uniforms[Index++] = { Type::Matrix4x4, Cursor, name };
				memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(Matrix4f));
				Cursor += sizeof(Matrix4f);
			}

			~BufferDeclaration() override = default;
		};
	};

	//////////////////////////////////////////
	/// Actual Shader class Implementation ///
	//////////////////////////////////////////

public:
	virtual ~Shader() = default;

	static void OnGuiRender();

	virtual void Reload() = 0;

	virtual void Bind() = 0;
	virtual RendererID GetRendererID() const = 0;
	virtual void UploadUniformBuffer(const Uniform::BufferBase &uniformBuffer) = 0;

	// TODO: Implement materials and remove these
	virtual void SetFloat(const String &name, float value) = 0;
	virtual void SetInt(const String &name, int value) = 0;
	virtual void SetFloat3(const String &name, const Vector3f &value) = 0;
	virtual void SetMat4(const String &name, const Matrix4f &value) = 0;
	virtual void SetMat4FromRenderThread(const String &name, const Matrix4f &value, bool bind = true) = 0;
	virtual void SetIntArray(const String &name, int *values, Uint32 size) = 0;

	virtual const String &GetName() const = 0;

	virtual void SetVSMaterialUniformBuffer(const Buffer &buffer) = 0;
	virtual void SetPSMaterialUniformBuffer(const Buffer &buffer) = 0;

	virtual const ShaderUniformBufferDeclaration::List &GetVSRendererUniforms() const = 0;
	virtual const ShaderUniformBufferDeclaration::List &GetPSRendererUniforms() const = 0;
	virtual bool HasVSMaterialUniformBuffer() const = 0;
	virtual bool HasPSMaterialUniformBuffer() const = 0;
	virtual const ShaderUniformBufferDeclaration &GetVSMaterialUniformBuffer() const = 0;
	virtual const ShaderUniformBufferDeclaration &GetPSMaterialUniformBuffer() const = 0;

	virtual const ShaderResourceDeclaration::List &GetResources() const = 0;

	static Shared<Shader> Create(const Filepath &filepath);
	static Shared<Shader> Create(const String &source);

};

}

