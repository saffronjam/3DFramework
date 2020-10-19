#pragma once

#include <string>
#include <unordered_map>

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/Renderer/ShaderUniform.h"

namespace Se
{
class Shader : public RefCounted
{
public:
	struct Uniform
	{
		enum class Type
		{
			None = 0,
			Float, Float2, Float3, Float4,
			Matrix3x3, Matrix4x4,
			Int32, Uint32
		};

		struct Collection
		{

		};

		struct Decl
		{
			Type DeclType;
			std::ptrdiff_t Offset;
			std::string Name;
		};

		struct Buffer
		{
			Uint8 *Data;
			std::vector<Decl> Uniforms;
		};

		struct BufferBase
		{
			virtual ~BufferBase() = default;
			virtual const Uint8 *GetBuffer() const = 0;
			virtual const Decl *GetUniforms() const = 0;
			virtual unsigned int GetUniformCount() const = 0;
		};


		template<unsigned int N, unsigned int U>
		struct BufferDeclaration : BufferBase
		{
			Uint8 Buffer[N]{};
			Decl Uniforms[U];
			std::ptrdiff_t Cursor = 0;
			int Index = 0;

			const Uint8 *GetBuffer() const override { return Buffer; }
			const Decl *GetUniforms() const override { return Uniforms; }
			unsigned int GetUniformCount() const override { return U; }

			template<typename T>
			void Push(const std::string &name, const T &data) {}

			template<>
			void Push(const std::string &name, const float &data)
			{
				Uniforms[Index++] = { Type::Float, Cursor, name };
				memcpy(Buffer + Cursor, &data, sizeof(float));
				Cursor += sizeof(float);
			}

			template<>
			void Push(const std::string &name, const glm::vec3 &data)
			{
				Uniforms[Index++] = { Type::Float3, Cursor, name };
				memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec3));
				Cursor += sizeof(glm::vec3);
			}

			template<>
			void Push(const std::string &name, const glm::vec4 &data)
			{
				Uniforms[Index++] = { Type::Float4, Cursor, name };
				memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec4));
				Cursor += sizeof(glm::vec4);
			}

			template<>
			void Push(const std::string &name, const glm::mat4 &data)
			{
				Uniforms[Index++] = { Type::Matrix4x4, Cursor, name };
				memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::mat4));
				Cursor += sizeof(glm::mat4);
			}

			~BufferDeclaration() override = default;
		};

		class Declaration
		{

		};

	};

	//////////////////////////////////////////
	/// Actual Shader class Implementation ///
	//////////////////////////////////////////

public:
	using ShaderReloadedCallback = std::function<void()>;

public:
	virtual ~Shader() = default;

	static void OnGuiRender();

	virtual void Reload() = 0;

	virtual void Bind() = 0;
	virtual RendererID GetRendererID() const = 0;
	virtual void UploadUniformBuffer(const Uniform::BufferBase &uniformBuffer) = 0;

	// TODO: Implement materials and remove these
	virtual void SetFloat(const std::string &name, float value) = 0;
	virtual void SetInt(const std::string &name, int value) = 0;
	virtual void SetFloat3(const std::string &name, const glm::vec3 &value) = 0;
	virtual void SetMat4(const std::string &name, const glm::mat4 &value) = 0;
	virtual void SetMat4FromRenderThread(const std::string &name, const glm::mat4 &value, bool bind = true) = 0;
	virtual void SetIntArray(const std::string &name, int *values, Uint32 size) = 0;

	virtual const std::string &GetName() const = 0;

	virtual void SetVSMaterialUniformBuffer(const Buffer &buffer) = 0;
	virtual void SetPSMaterialUniformBuffer(const Buffer &buffer) = 0;

	virtual const ShaderUniformBufferDeclaration::List &GetVSRendererUniforms() const = 0;
	virtual const ShaderUniformBufferDeclaration::List &GetPSRendererUniforms() const = 0;
	virtual bool HasVSMaterialUniformBuffer() const = 0;
	virtual bool HasPSMaterialUniformBuffer() const = 0;
	virtual const ShaderUniformBufferDeclaration &GetVSMaterialUniformBuffer() const = 0;
	virtual const ShaderUniformBufferDeclaration &GetPSMaterialUniformBuffer() const = 0;

	virtual const ShaderResourceDeclaration::List &GetResources() const = 0;

	virtual void AddShaderReloadedCallback(const ShaderReloadedCallback &callback) = 0;

	static Ref<Shader> Create(const std::string &filepath);
	static Ref<Shader> CreateFromString(const std::string &source);

	// TODO: Create Asset manager and remove this
	static std::vector<Ref<Shader>> m_sAllShaders;
};

class ShaderLibrary : public RefCounted
{
public:
	void Add(const Ref<Shader> &shader);
	void Load(const std::string &path);
	void Load(const std::string &name, const std::string &path);

	const Ref<Shader> &Get(const std::string &name) const;
private:
	std::unordered_map<std::string, Ref<Shader>> m_Shaders;
};

}

