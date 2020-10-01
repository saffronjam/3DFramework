//
// Note:	this file is to be included in client applications ONLY
//			NEVER include this file anywhere in the engine codebase
//
#pragma once

#include "Saffron/Core/Application.h"
#include "Saffron/Core/Log.h"
#include "Saffron/Core/Time.h"
#include "Saffron/Core/Timer.h"

#include "Saffron/Core/Event.h"
#include "Saffron/Core/Events/AppEvent.h"
#include "Saffron/Core/Events/KeyboardEvent.h"
#include "Saffron/Core/Events/MouseEvent.h"

#include "Saffron/Core/Math/AABB.h"
#include "Saffron/Core/Math/Ray.h"

#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Input.h"

// --- SaffronRender API ------------------------------
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/SceneRenderer.h"
#include "Saffron/Renderer/RenderPass.h"
#include "Saffron/Renderer/Framebuffer.h"
#include "Saffron/Renderer/VertexBuffer.h"
#include "Saffron/Renderer/IndexBuffer.h"
#include "Saffron/Renderer/Pipeline.h"
#include "Saffron/Renderer/Texture.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Renderer/Camera.h"
#include "Saffron/Renderer/Material.h"
#include "Saffron/Renderer/EditorViewport.h"
// ---------------------------------------------------

// Scenes
#include "Saffron/Scene/Entity.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Scene/SceneCamera.h"
#include "Saffron/Scene/SceneSerializer.h"
#include "Saffron/Scene/Components.h"
