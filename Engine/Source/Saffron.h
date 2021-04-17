//
// Note:	this file is to be included in client applications ONLY
//			NEVER include this file anywhere in the engine codebase
//
#pragma once

#include "Saffron/Core/App.h"
#include "Saffron/Core/Log.h"
#include "Saffron/Core/Time.h"
#include "Saffron/Core/Timer.h"
#include "Saffron/Core/Global.h"

#include "Saffron/Core/Event.h"
#include "Saffron/Core/Events/AppEvent.h"
#include "Saffron/Core/Events/KeyboardEvent.h"
#include "Saffron/Core/Events/MouseEvent.h"

#include "Saffron/Math/AABB.h"
#include "Saffron/Math/Ray.h"

#include "Saffron/Editor/AssetPanel.h"
#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Editor/EntityComponentsPanel.h"
#include "Saffron/Editor/ScriptPanel.h"

#include "Saffron/Gui/Gui.h"

// --- SaffronRender API ------------------------------
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Resources/Framebuffer.h"
#include "Saffron/Rendering/Resources/VertexBuffer.h"
#include "Saffron/Rendering/Resources/IndexBuffer.h"
#include "Saffron/Rendering/Resources/Pipeline.h"
#include "Saffron/Rendering/Resources/Texture.h"
#include "Saffron/Rendering/Resources/Shader.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Camera.h"
#include "Saffron/Rendering/Material.h"
// ---------------------------------------------------

// Scenes
#include "Saffron/Entity/Entity.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Scene/SceneCamera.h"
#include "Saffron/Serialize/SceneSerializer.h"
#include "Saffron/Serialize/ProjectSerializer.h"
#include "Saffron/Entity/EntityComponents.h"

#include "Saffron/Base.h"
#include "Saffron/Editor/EditorTerminal.h"
#include "Saffron/Editor/EntityComponentsPanel.h"
#include "Saffron/Editor/SceneHierarchyPanel.h"
#include "Saffron/Editor/SplashScreenPane.h"
#include "Saffron/Scene/EditorScene.h"
#include "Saffron/Scene/RuntimeScene.h"

#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Core/Run.h"
#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Script/ScriptEngine.h"

#ifdef SAFFRON_ENTRY_POINT
#include <Saffron/EntryPoint.h>
#endif
