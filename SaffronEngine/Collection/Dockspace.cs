using System.Numerics;
using ImGuiNET;

namespace SaffronEngine.Collection
{
    public class Dockspace
    {
        public void Begin()
        {
            var viewport = ImGui.GetMainViewport();

            ImGui.SetNextWindowPos(viewport.GetWorkPos());
            ImGui.SetNextWindowSize(viewport.GetWorkSize());
            ImGui.SetNextWindowViewport(viewport.ID);

            ImGuiWindowFlags hostWindowFlags = 0;
            hostWindowFlags |=
                ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoCollapse | ImGuiWindowFlags.NoResize |
                ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoDocking |
                ImGuiWindowFlags.NoBringToFrontOnFocus | ImGuiWindowFlags.NoNavFocus |
                ImGuiWindowFlags.NoBackground | ImGuiWindowFlags.MenuBar;

            ImGui.PushStyleVar(ImGuiStyleVar.WindowRounding, 0.0f);
            ImGui.PushStyleVar(ImGuiStyleVar.WindowBorderSize, 0.0f);
            ImGui.PushStyleVar(ImGuiStyleVar.WindowPadding, Vector2.Zero);
            ImGui.Begin("DockSpaceViewport_%08X", hostWindowFlags);
            ImGui.PopStyleVar(3);

            var dockspaceId = ImGui.GetID("DockSpace");
            ImGui.DockSpace(dockspaceId, Vector2.Zero, ImGuiDockNodeFlags.None);
        }

        public void End()
        {
            ImGui.End();
        }
    }
}