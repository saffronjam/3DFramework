#include "Window.h"

#include <GLFW/glfw3.h>

#include "GenericThrowMacros.h"

Window::Window(std::string title, int width, int height, bool activeContext)
        : m_title(std::move(title)),
          m_width(width),
          m_height(height),
          kbd(*this),
          mouse(*this)
{
    Engine::BindErrorCallback([](int code, const char *desc)
                              {
                                  throw Window::Exception(__LINE__, __FILE__, code, desc);
                              });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    m_glfwWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

    if (activeContext)
    {
        glfwMakeContextCurrent(m_glfwWindow);
    }
    SetupGLFWCallbacks();
    Engine::BindDefaultErrorCallback();
}

Window::~Window()
{
    glfwDestroyWindow(m_glfwWindow);
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(m_glfwWindow);
}

void Window::Close()
{
    glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
}

void Window::PushEvent(const Event &event)
{
    m_events.push_back(event);
}

void Window::PollAllEvents()
{
    for (auto &event : m_events)
    {
        if (m_handlers.find(event.type) != m_handlers.end())
        {
            auto range = m_handlers.equal_range(event.type);
            for (auto it = range.first; it != range.second; it++)
            {
                it->second->HandleEvent(event);
            }
        }
    }
    m_events.clear();
}

void Window::AddEventHandler(Event::Type type, IEventHandler *handler)
{
    // Make sure that the handler does not already handle this type
    if (m_handlers.find(type) != m_handlers.end())
    {
        auto range = m_handlers.equal_range(type);
        for (auto it = range.first; it != range.second; it++)
        {
            if (it->second == handler)
                return;
        }
    }
    m_handlers.emplace(std::make_pair(type, handler));
}

GLFWwindow *Window::GetCoreWindow() const noexcept
{
    return m_glfwWindow;
}

void Window::SetTitle(std::string title)
{
    m_title = std::move(title);
    glfwSetWindowTitle(m_glfwWindow, m_title.c_str());
}

void Window::SetupGLFWCallbacks()
{
    glfwSetWindowUserPointer(m_glfwWindow, this);
    glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        auto pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
        Event event{};
        event.type = Event::Type::Keyboard;
        event.keyboard = {static_cast<Keyboard::Action>(action), static_cast<Keyboard::Key>(key)};
        pWnd->PushEvent(event);
    });
    glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow *window, int button, int action, int mods)
    {
        auto pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
        Event event{};
        event.type = Event::Type::Mouse;
        event.mouse = {static_cast<Mouse::Action>(action), static_cast<Mouse::Button>(button), pWnd->mouse.GetPosition()};
        pWnd->PushEvent(event);
    });
    glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow *window, double xpos, double ypos)
    {
        auto pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
        Event event{};
        event.type = Event::Type::Mouse;
        event.mouse = {Mouse::Action::Move, Mouse::Button::Undefined, glm::vec2(xpos, ypos)};
        pWnd->PushEvent(event);
    });
    glfwSetCursorEnterCallback(m_glfwWindow, [](GLFWwindow *window, int enter)
    {
        auto pWnd = static_cast<Window *>(glfwGetWindowUserPointer(window));
        auto action = enter ? Mouse::Action::Enter : Mouse::Action::Leave;
        Event event{};
        event.type = Event::Type::Mouse;
        event.mouse = {action, Mouse::Button::Undefined, pWnd->mouse.GetPosition()};
        pWnd->PushEvent(event);
    });
}

Window::Exception::Exception(int line, const char *file, int errorCode, const char *errorString) noexcept
        : IException(line, file),
          errorCode(errorCode),
          errorString(errorString)
{
}

const char *Window::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Code] " << GetErrorCode() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char *Window::Exception::GetType() const noexcept
{
    return "Saffron Window Exception";
}

int Window::Exception::GetErrorCode() const noexcept
{
    return errorCode;
}

const char *Window::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}
