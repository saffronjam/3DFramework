#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Buffer.h"
#include "Saffron/Renderer/RendererAPI.h"

namespace Se {

enum class ShaderDataType
{
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static Uint32 ShaderDataTypeSize(ShaderDataType type);

class VertexBuffer : public ReferenceCounted
{
public:
	///////////////////////////////////////////////////////////////
	/// Vertex Buffer Element
	///////////////////////////////////////////////////////////////

	struct Element
	{
		String Name;
		ShaderDataType Type;
		Uint32 Size{};
		Uint32 Offset{};
		bool Normalized{};

		Element() = default;
		Element(ShaderDataType type, String name, bool normalized = false);

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
		const ArrayList<Element> &GetElements() const { return m_Elements; }

		ArrayList<Element>::iterator begin() { return m_Elements.begin(); }
		ArrayList<Element>::iterator end() { return m_Elements.end(); }
		ArrayList<Element>::const_iterator begin() const { return m_Elements.begin(); }
		ArrayList<Element>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride();

	private:
		ArrayList<Element> m_Elements;
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

	virtual unsigned int GetSize() const = 0;
	virtual RendererID GetRendererID() const = 0;

	virtual void SetData(void *buffer, Uint32 size, Uint32 offset = 0) = 0;
	virtual void SetData(const Buffer &buffer, Uint32 offset = 0) = 0;

	static Shared<VertexBuffer> Create(void *data, Uint32 size, Usage usage = Usage::Static);
	static Shared<VertexBuffer> Create(Uint32 size, Usage usage = Usage::Dynamic);
};



}