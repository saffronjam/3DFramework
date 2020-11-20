#pragma once

#include "Saffron/Renderer/Camera.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{
class Renderer2D
{
public:
	struct Statistics
	{
		Uint32 DrawCalls = 0;
		Uint32 QuadCount = 0;
		Uint32 LineCount = 0;

		Uint32 GetTotalVertexCount() const { return QuadCount * 4 + LineCount * 2; }
		Uint32 GetTotalIndexCount() const { return QuadCount * 6 + LineCount * 2; }
	};

public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const Matrix4f &viewProj, bool depthTest = true);
	static void EndScene();

	// Primitives
	static void DrawQuad(const Matrix4f &transform, const Vector4f &color);
	static void DrawQuad(const Matrix4f &transform, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const Vector4f &tintColor = Vector4f(1.0f));

	static void DrawQuad(const Vector2f &position, const Vector2f &size, const Vector4f &color);
	static void DrawQuad(const Vector3f &position, const Vector2f &size, const Vector4f &color);
	static void DrawQuad(const Vector2f &position, const Vector2f &size, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const Vector4f &tintColor = Vector4f(1.0f));
	static void DrawQuad(const Vector3f &position, const Vector2f &size, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const Vector4f &tintColor = Vector4f(1.0f));

	static void DrawRotatedQuad(const Vector2f &position, const Vector2f &size, float rotation, const Vector4f &color);
	static void DrawRotatedQuad(const Vector3f &position, const Vector2f &size, float rotation, const Vector4f &color);
	static void DrawRotatedQuad(const Vector2f &position, const Vector2f &size, float rotation, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const Vector4f &tintColor = Vector4f(1.0f));
	static void DrawRotatedQuad(const Vector3f &position, const Vector2f &size, float rotation, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const Vector4f &tintColor = Vector4f(1.0f));

	static void DrawLine(const Vector3f &p0, const Vector3f &p1, const Vector4f &color = Vector4f(1.0f));

	static const Statistics &GetStats();
	static void ResetStats();
private:
	static void FlushAndReset();
	static void FlushAndResetLines();
};
}
