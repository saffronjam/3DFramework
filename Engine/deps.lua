local deps = {}

function RequireModule(name)
    local dep = require("ThirdParty." .. name .. ".premake5")
    deps[dep.Project] = dep
end

RequireModule("glm")
RequireModule("glfw")
RequireModule("Vulkan")
RequireModule("imgui")
RequireModule("assimp")
RequireModule("entt")
RequireModule("json")

return deps