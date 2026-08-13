#pragma once
#include <Renderer/OrthographicCamera.h>

namespace NK {

    // v0.3.2: Scene упрощена до обёртки вокруг двух камер (game + UI).
    // v0.5.1: game objects больше не рендерятся здесь (SpriteRenderSystem).
    // v0.5.2: m_UIObjects, GameObject, Component, Transform, ScriptComponent удалены.
    //
    // Scene также owns `m_Started` флаг для Engine::OnStart.
    class Scene
    {
    public:
        Scene() = default;

        void OnStart();
        void OnWindowResized(uint32_t width, uint32_t height);

        OrthographicCamera& GetGameCamera() { return m_GameCamera; }
        OrthographicCamera& GetUICamera() { return m_UICamera; }

    private:
        // Игровой мир (ortho, нормализованные координаты)
        OrthographicCamera m_GameCamera{ -5.0f, 5.0f, -5.0f, 5.0f };

        // UI (ortho, screen coords 0..W × 0..H, Y вверх)
        OrthographicCamera m_UICamera{ 0.0f, 1280.0f, 0.0f, 720.0f };

        bool m_Started = false;
    };

} // namespace NK
