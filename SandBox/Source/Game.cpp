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
        if (m_Game) m_Game->Update(deltaTime);

        // 2. Начало кадра
        NK::Renderer::BeginFrame();

        // 3. v0.3.2: Match3Game::Render рисует ВСЁ — game world (SpriteRenderSystem)
        // и UI (UIRenderSystem). Scene::OnRender удалён.
        if (m_Game) m_Game->Render();

        // 4. Завершающий кадр
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