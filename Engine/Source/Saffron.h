//
// Note:	this file is to be included in client applications ONLY
//			NEVER include this file anywhere in the engine codebase
//
#pragma once

#include "Saffron/Common/App.h"
#include "Saffron/Common/Layer.h"
#include "Saffron/Common/SubscriberList.h"
#include "Saffron/Event/Event.h"
#include "Saffron/Graphics/Camera.h"
#include "Saffron/Graphics/EditorCamera.h"
#include "Saffron/Graphics/Model.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Scene/SceneRegistry.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Serialize/SceneSerializer.h"
#include "Saffron/Ui/Panels.h"
#include "Saffron/Ui/Ui.h"

#ifdef SE_ENTRY_POINT
#include <Saffron/Core/EntryPoint.h>
#endif
