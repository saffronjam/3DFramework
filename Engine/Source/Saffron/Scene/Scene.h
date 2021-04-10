#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderGraph.h"

// TODO: Temporary
#include "Saffron/Rendering/Mesh.h"

namespace Se
{
class Scene : public MemManaged<Scene>
{
public:
	struct CameraTransforms
	{
		Matrix MainView;
		Matrix MainProjection;
		mutable Matrix CurrentView;
		mutable Matrix CurrentProjection;
		Vector3 ShadowCameraPos;
	};

public:
	Scene();

	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnGuiRender();

	void SetViewMatrix(const Matrix& view);
	void SetProjectionMatrix(const Matrix& projection);

	const CameraTransforms& GetCameraTransforms() const;

private:
	CameraTransforms _cameraTransforms;
	Shared<Mesh> _cube;

	RenderGraph _renderGraph;
};
}
