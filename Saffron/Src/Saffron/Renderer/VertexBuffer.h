#pragma once

#include "Saffron/Config.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Renderer/RendererAPI.h"

namespace Se {

enum class ShaderDataType
{
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static Uint32 ShaderDataTypeSize(ShaderDataType type);

class VertexBuffer : public RefCounted
{
public:
	///////////////////////////////////////////////////////////////
	/// Vertex Buffer Element
	///////////////////////////////////////////////////////////////

	struct Element
	{
		std::string Name;
		ShaderDataType Type;
		Uint32 Size{};
		Uint32 Offset{};
		bool Normalized{};

		Element() = default;
		Element(ShaderDataType type, std::string name, bool normalized = false);

		Uint32 GetComponentCount() const;
	};

	///////////////////////////////////////////////////////////////
	/// Vertex Buffer Layout
	///////////////////////////////////////////////////////////////

	class Layout
	{
	public:
		Layout() = default;
		Layout(const std::initializer_list<Element> &elements);

		Uint32 GetStride() const { return m_Stride; }
		const std::vector<Element> &GetElements() const { return m_Elements; }

		std::vector<Element>::iterator begin() { return m_Elements.begin(); }
		std::vector<Element>::iterator end() { return m_Elements.end(); }
		std::vector<Element>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<Element>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride();

	private:
		std::vector<Element> m_Elements;
		Uint32 m_Stride = 0;
	};

	///////////////////////////////////////////////////////////////
	/// Vertex Buffer Usage
	///////////////////////////////////////////////////////////////

	enum class Usage
	{
		None = 0, Static = 1, Dynamic = 2
	};

	///////////////////////////////////////////////////////////////
	/// Vertex Buffer
	///////////////////////////////////////////////////////////////

	virtual ~VertexBuffer() = default;

	virtual void Bind() const = 0;

	virtual const Layout &GetLayout() const = 0;
	virtual unsigned int GetSize() const = 0;
	virtual RendererID GetRendererID() const = 0;

	virtual void SetLayout(const Layout &layout) = 0;
	virtual void SetData(void *buffer, Uint32 size, Uint32 offset = 0) = 0;
	virtual void SetData(const Buffer &buffer, Uint32 offset = 0) = 0;

	static Ref<VertexBuffer> Create(void *data, Uint32 size, Usage usage = Usage::Static);
	static Ref<VertexBuffer> Create(Uint32 size, Usage usage = Usage::Dynamic);
};



}