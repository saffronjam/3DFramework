#include "GuiMgr.h"

GuiMgr::GuiMgr()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}

GuiMgr::~GuiMgr()
{
	ImGui::DestroyContext();
}
