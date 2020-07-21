#include "GuiMgr.h"

#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Window.h"

GuiMgr::GuiMgr(Window &window)
        : m_window(window)
{
    IMGUI_CHECKVERSION();
    auto ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx);
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window.GetCoreWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

GuiMgr::~GuiMgr()
{
    ImGui::DestroyContext();
}

void GuiMgr::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiMgr::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
