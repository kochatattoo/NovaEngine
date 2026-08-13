#include "Scene/Scene.h"
#include "Core/Log.h"
#include "Core/Engine.h"
#include "Window/Window.h"

namespace NK {

    void Scene::OnStart()
    {
        NK_CORE_INFO("Scene OnStart");

        auto* window = Engine::Get().GetWindow();
        const uint32_t w = window->GetWidth();
        const uint32_t h = window->GetHeight();
        OnWindowResized(w, h);

        m_Started = true;
    }

    void Scene::OnWindowResized(uint32_t width, uint32_t height)
    {
        m_GameCamera.OnWindowResized(width, height);
        m_UICamera.SetProjection(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    }

} // namespace NK
