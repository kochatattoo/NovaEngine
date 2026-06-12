#include <Core/EntryPoint.h>
#include <Core/Log.h>
#include <Input/Input.h>        // старый ввод
#include <Renderer/Renderer.h>
#include <Event/Event.h>
#include "Game/Match3Game.h"

class SandboxApp : public NK::Application {
public:
    SandboxApp() = default;

    void OnStart() override {
        m_Game = std::make_unique<NK::Match3Game>();
        m_Game->Start();
    }

    void OnUpdate(float deltaTime) override {
		// 1. Обновляем логику всех объектов (включая Lua-скрипты)
		NK::Engine::Get().GetScene().OnUpdate(deltaTime);

        if (m_Game) m_Game->Update(deltaTime);

		// 2. Начинаем кадр
		NK::Renderer::BeginFrame();

		// 3. Рисуем все спрайты через компоненты SpriteRenderer
		NK::Engine::Get().GetScene().OnRender();

		// 4. Заканчиваем кадр
		NK::Renderer::EndFrame();
    }

    void OnEvent(NK::Event& e) override {
        NK::EventDispatcher dispatcher(e);

        // Обработка нажатий клавиш (аналог Input.GetKeyDown)
        dispatcher.Dispatch<NK::KeyPressedEvent>([this](NK::KeyPressedEvent& keyEvent) {
            if (keyEvent.KeyCode == VK_ESCAPE) {
                NK::Engine::Get().Shutdown();
                return true;   // событие обработано
            }
            if (keyEvent.KeyCode == VK_SPACE) {
				m_ColorR = (rand() % 1000) / 1000.0f;
				m_ColorG = (rand() % 1000) / 1000.0f;
				m_ColorB = (rand() % 1000) / 1000.0f;
                return true;
            }
            return false;   // не обработано
            });
    }

    void OnShutdown() override {
        NK_INFO("Sandbox shutting down.");
    }

private:
	float m_ColorR = 1.0f, m_ColorG = 1.0f, m_ColorB = 1.0f;
    std::unique_ptr<NK::Match3Game> m_Game;
};

NK::Application* NK::CreateApplication() {
    return new SandboxApp();
}