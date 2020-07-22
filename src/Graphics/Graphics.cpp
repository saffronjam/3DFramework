#include "Graphics.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GenericThrowMacros.h"
#include "GLCheck.h"

Graphics::Graphics(Window &window)
        : m_wnd(window),
          m_guiMgr(m_wnd)
{
    glewInit();
    int display_w, display_h;
    glfwGetFramebufferSize(m_wnd.GetCoreWindow(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
}

void Graphics::BeginFrame(float r, float g, float b, float a)
{
    m_guiMgr.BeginFrame();
    glCheck(glClearColor(r, g, b, a));
    glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Graphics::EndFrame()
{
    m_guiMgr.EndFrame();
    glfwSwapBuffers(m_wnd.GetCoreWindow());
    glfwPollEvents();
}

void Graphics::DrawArrays()
{
    glCheck(glDrawArrays(GL_TRIANGLES, 0, 3));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCheck(glBindVertexArray(0));
    glCheck(glUseProgram(0));
}

void Graphics::DrawIndexed(const std::vector<unsigned int> &indicies)
{
    glCheck(glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, nullptr));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    glCheck(glBindVertexArray(0));
    glCheck(glUseProgram(0));
}
