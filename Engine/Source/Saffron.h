//
// Note:	this file is to be included in client applications ONLY
//			NEVER include this file anywhere in the engine codebase
//
#pragma once
//
// Note:	this file is to be included in client applications ONLY
//			NEVER include this file anywhere in the engine codebase
//
#pragma once

#include "Saffron/Common/App.h"
#include "Saffron/Common/Log.h"
#include "Saffron/Common/Time.h"
#include "Saffron/Common/Timer.h"
#include "Saffron/Common/Global.h"

#include "Saffron/Common/Event.h"
#include "Saffron/Common/Events/AppEvent.h"
#include "Saffron/Common/Events/KeyboardEvent.h"
#include "Saffron/Common/Events/MouseEvent.h"

#include "Saffron/Common/BatchLoader.h"
#include "Saffron/Common/FileIOManager.h"
#include "Saffron/Common/Misc.h"
#include "Saffron/Common/Run.h"

//#include "Saffron/Editor/AssetPanel.h"
//#include "Saffron/Editor/ViewportPane.h"
//#include "Saffron/Editor/EntityPanel.h"
//#include "Saffron/Editor/ScriptPanel.h"
//#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Editor/EditorCamera.h"

//#include "Saffron/Gui/Gui.h
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"

#include "Saffron/Math/SaffronMath.h"

// --- SaffronRender API ------------------------------
//#include "Saffron/Rendering/Renderer.h"
//#include "Saffron/Rendering/SceneRenderer.h"
//#include "Saffron/Rendering/RenderPass.h"
#include "Saffron/Rendering/Resources.h"
#include "Saffron/Rendering/Mesh.h"
// ---------------------------------------------------

// Scenes
//#include "Saffron/Entity/Entity.h"
#include "Saffron/Scene/Scene.h"
//#include "Saffron/Scene/SceneCamera.h"
//#include "Saffron/Serialize/SceneSerializer.h"
//#include "Saffron/Serialize/ProjectSerializer.h"
//#include "Saffron/Entity/EntityComponents.h"

#include "Saffron/Base.h"
//#include "Saffron/Editor/EditorTerminal.h"
//#include "Saffron/Editor/EntityPanel.h"
//#include "Saffron/Editor/ScenePanel.h"
//#include "Saffron/Editor/SplashScreenPane.h"

#include "Saffron/Scene/EditorScene.h"
//#include "Saffron/Scene/ModelSpaceScene.h"
//#include "Saffron/Scene/RuntimeScene.h"

//#include "Saffron/Gui/Gui.h"
//#include "Saffron/Script/ScriptEngine.h"

#ifdef SAFFRON_ENTRY_POINT
#include <Saffron/EntryPoint.h>
#endif
