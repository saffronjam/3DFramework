#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources.h"

namespace Se
{
struct PosColorVertex
{
	PosColorVertex(const Vector3& position, const Color& color) :
		Position(position),
		Color(color)
	{
	}

	static VertexLayout GetLayout()
	{
		return VertexLayout().Begin().Add(VertexAttribute::Position, 3, VertexAttributeType::Float).Add(
			VertexAttribute::Color0, 4, VertexAttributeType::Float).End();
	}

	Vector3 Position;
	Color Color;
};

class Mesh
{
public:
	Mesh();

	void Bind();

	void SetTransform(const Matrix& transform);

	static Shared<Mesh> Create();

private:
	Shared<VertexBuffer> _vertexBuffer;
	Shared<IndexBuffer> _indexBuffer;
	Matrix _transform;

	friend class Renderer;
	friend class SceneRenderer;
};
}
