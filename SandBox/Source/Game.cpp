#include <Core/EntryPoint.h>
#include <Core/Log.h>
#include <Input/InputSystem.h>
#include <Renderer/Renderer.h>

#include "Game/Match3Game.h"

class SandboxApp : public NK::Application {
public:
    SandboxApp() = default;

    void OnStart() override {
        m_Game = std::make_unique<NK::Match3Game>();
        m_Game->Start();
    }

    void OnUpdate(float deltaTime) override {
        // 1. Обновить все объекты сцены (вызывают Lua-скрипты через ScriptComponent)
        NK::Engine::Get().GetScene().OnUpdate(deltaTime);

        if (m_Game) m_Game->Update(deltaTime);

        // 2. Начало кадра
        NK::Renderer::BeginFrame();

        // 3. v0.2.6: Рисуем ECS-entities (плитки Match3) ДО Scene::OnRender,
        // чтобы UI (который рисуется в Scene::OnRender) был поверх.
        if (m_Game) m_Game->Render();

        // 4. Рисуем game objects (старые, через SpriteRenderer) + UI
        NK::Engine::Get().GetScene().OnRender();

        // 5. Завершающий кадр
        NK::Renderer::EndFrame();
    }

    void OnShutdown() override {
        NK_INFO("Sandbox shutting down.");
    }

private:
    std::unique_ptr<NK::Match3Game> m_Game;
};

NK::Application* NK::CreateApplication() {
    return new SandboxApp();
}