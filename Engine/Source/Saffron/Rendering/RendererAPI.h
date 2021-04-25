#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/PrimitiveType.h"

namespace Se
{
using RendererID = uint;

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
	static void DrawIndexed(uint count, PrimitiveType type, bool depthTest = true);

	static RendererCapabilities& GetCapabilities() { return _sCapabilities; }

	static void SetLineThickness(float thickness);
	static void SetClearColor(float r, float g, float b, float a);

	static RendererApiType Current() { return _sCurrentAPI; }

private:
	static RendererApiType _sCurrentAPI;
	static RendererCapabilities _sCapabilities;
	static float _lineThickness;
};
}
