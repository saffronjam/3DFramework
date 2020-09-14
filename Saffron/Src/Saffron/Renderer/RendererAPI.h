#pragma once

#include "Saffron/Config.h"
#include "Saffron/Renderer/PrimitiveType.h"

namespace Se
{
using RendererID = Uint32;

class RendererAPI
{
public:
	enum class Type
	{
		None = 0, OpenGL = 1
	};

	struct Capabilities
	{
		std::string Vendor;
		std::string Renderer;
		std::string Version;

		int MaxSamples = 0;
		float MaxAnisotropy = 0.0f;
		int MaxTextureUnits = 0;
	};

public:
	virtual ~RendererAPI() = default;

	static void Init();
	static void Shutdown();

	static void Clear(float r, float g, float b, float a);
	static void DrawIndexed(Uint32 count, PrimitiveType type, bool depthTest = true);

	static Capabilities &GetCapabilities() { return m_sCapabilities; }
	static void SetLineThickness(float thickness);
	static void SetClearColor(float r, float g, float b, float a);

	static Type CurrentAPI() { return m_sCurrentAPI; }

private:
	static void LoadRequiredAssets();

private:
	static Type m_sCurrentAPI;
	static Capabilities m_sCapabilities;
};
}
