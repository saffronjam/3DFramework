#pragma once

#include <Saffron/Config.h>

#include <Saffron/Core/Application.h>
#include <Saffron/Core/Math/AABB.h>
#include <Saffron/Core/Math/Ray.h>

#include <Saffron/System/Log.h>
#include <Saffron/System/Time.h>
#include <Saffron/System/Timer.h>

#include <Saffron/Editor/SceneHierarchyPanel.h>

#include <Saffron/Core/Event/Event.h>
#include <Saffron/Core/Event/ApplicationEvent.h>
#include <Saffron/Core/Event/KeyboardEvent.h>
#include <Saffron/Core/Event/MouseEvent.h>

#include <Saffron/Gui/Gui.h>

#include <Saffron/Input/Mouse.h>
#include <Saffron/Input/Keyboard.h>

// --- SaffronRenderer API ------------------------------
#include <Saffron/Renderer/Renderer.h>
#include <Saffron/Renderer/Renderer2D.h>
#include <Saffron/Renderer/SceneRenderer.h>
#include <Saffron/Renderer/RenderPass.h>
#include <Saffron/Renderer/Framebuffer.h>
#include <Saffron/Renderer/VertexBuffer.h>
#include <Saffron/Renderer/IndexBuffer.h>
#include <Saffron/Renderer/Pipeline.h>
#include <Saffron/Renderer/Texture.h>
#include <Saffron/Renderer/Shader.h>
#include <Saffron/Renderer/Mesh.h>
#include <Saffron/Renderer/Camera.h>
#include <Saffron/Renderer/Material.h>

// -------Saffron Scene ---------------------------------------
#include <Saffron/Scene/Entity.h>
#include <Saffron/Scene/Scene.h>
#include <Saffron/Scene/SceneCamera.h>
#include <Saffron/Scene/SceneSerializer.h>
#include <Saffron/Scene/Components.h>

// -------Saffron Scene ---------------------------------------
#include <Saffron/Script/ScriptEngine.h>