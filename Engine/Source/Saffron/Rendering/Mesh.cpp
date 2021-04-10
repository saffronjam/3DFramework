#include "SaffronPCH.h"

#include "Saffron/Rendering/Mesh.h"

namespace Se
{
Mesh::Mesh()
{
	constexpr int side = 1.0f;
	const ArrayList<PosColorVertex> vertices = {
		{Vector3{-1.0f, 1.0f, 1.0f}, Color(0xff000000)}, {Vector3{1.0f, 1.0f, 1.0f}, Color(0xff0000ff)},
		{Vector3{-1.0f, -1.0f, 1.0f}, Color(0xff00ff00)}, {Vector3{1.0f, -1.0f, 1.0f}, Color(0xff00ffff)},
		{Vector3{-1.0f, 1.0f, -1.0f}, Color(0xffff0000)}, {Vector3{1.0f, 1.0f, -1.0f}, Color(0xffff00ff)},
		{Vector3{-1.0f, -1.0f, -1.0f}, Color(0xffffff00)}, {Vector3{1.0f, -1.0f, -1.0f}, Color(0xffffffff)},
	};

	const ArrayList<Uint16> indices = {
		0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6, 1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
	};

	_vertexBuffer = VertexBuffer::Create(Buffer::Copy(vertices), PosColorVertex::GetLayout());
	_indexBuffer = IndexBuffer::Create(Buffer::Copy(indices));
}

void Mesh::Bind()
{
}

void Mesh::SetTransform(const Matrix& transform)
{
	_transform = transform;
}

Shared<Mesh> Mesh::Create()
{
	return CreateShared<Mesh>();
}
}
