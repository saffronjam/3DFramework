#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/PrimitiveType.h"

namespace Se
{
using RendererID = Uint32;

enum class RendererApiType
{
	None = 0,
	OpenGL = 1
};

struct RendererCapabilities
{
	String Vendor;
	String Renderer;
	String Version;

	int MaxSamples = 0;
	float MaxAnisotropy = 0.0f;
	int MaxTextureUnits = 0;
};

class RendererApi
{
public:
	virtual ~RendererApi() = default;

	static void Init();
	static void Shutdown();

	static void Clear(float r, float g, float b, float a);
	static void DrawIndexed(Uint32 count, PrimitiveType type, bool depthTest = true);

	static RendererCapabilities& GetCapabilities() { return m_sCapabilities; }

	static void SetLineThickness(float thickness);
	static void SetClearColor(float r, float g, float b, float a);

	static RendererApiType Current() { return m_sCurrentAPI; }

private:
	static RendererApiType m_sCurrentAPI;
	static RendererCapabilities m_sCapabilities;
	static float m_LineThickness;
};
}
