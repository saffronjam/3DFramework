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

	static void BeginScene(const glm::mat4 &viewProj, bool depthTest = true);
	static void EndScene();

	// Primitives
	static void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color);
	static void DrawQuad(const glm::mat4 &transform, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

	static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
	static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color);
	static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
	static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

	static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color);
	static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color);
	static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
	static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Shared<Texture2D> &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

	static void DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color = glm::vec4(1.0f));

	static const Statistics &GetStats();
	static void ResetStats();
private:
	static void FlushAndReset();
	static void FlushAndResetLines();
};
}
