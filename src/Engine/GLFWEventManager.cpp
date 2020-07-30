#include "GLFWEventManager.h"
#include "Engine.hpp"
#include "InputManager.h"

void Input::GLFWEventManager::Init(GLFWwindow* window)
{
}

void Input::GLFWEventManager::TrackWindow(GLFWwindow* window)
{
    glfwSetWindowPosCallback(window, WindowPosCallback);
}

void Input::GLFWEventManager::WindowPosCallback(GLFWwindow* window, int x, int y)
{
    auto& engine = RedEngine::Engine::get();
    GLFWEvent event;
    event.Type = GLFWEventType::WindowMoved;

    EventQueue.Emplace(event);
}

void Input::GLFWEventManager::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
}

void Input::GLFWEventManager::WindowCloseCallback(GLFWwindow* window)
{
}

void Input::GLFWEventManager::WindowRefreshCallback(GLFWwindow* window)
{
}

void Input::GLFWEventManager::WindowFocusCallback(GLFWwindow* window, int focused)
{
}

void Input::GLFWEventManager::WindowIconifyCallback(GLFWwindow* window, int iconified)
{
}

void Input::GLFWEventManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
}

void Input::GLFWEventManager::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void Input::GLFWEventManager::CursorPositionCallback(GLFWwindow* window, double x, double y)
{
}

void Input::GLFWEventManager::CursorEnterCallback(GLFWwindow* window, int entered)
{
}

void Input::GLFWEventManager::MouseScrollCallback(GLFWwindow* window, double x, double y)
{
}

void Input::GLFWEventManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void Input::GLFWEventManager::CharCallback(GLFWwindow* window, unsigned codepoint)
{
}

void Input::GLFWEventManager::MonitorCallback(GLFWwindow* window, int action)
{
}

void Input::GLFWEventManager::FileDropCallback(GLFWwindow* window, int count, const char** paths)
{
}

void Input::GLFWEventManager::JoystickCallback(int jid, int action)
{
}

void Input::GLFWEventManager::WindowMaximiseCallback(GLFWwindow* window, int maximised)
{
}

void Input::GLFWEventManager::WindowContentScaleCallback(GLFWwindow* window, float xscale, float yscale)
{
}
