#include <Core/EntryPoint.h>
#include <Core/Log.h>
#include <Input/Input.h>        // старый ввод
#include <Renderer/Renderer.h>
#include <Event/Event.h>

class SandboxApp : public NK::Application {
public:
    void OnStart() override {
		// Загружаем и выполняем скрипт
		auto& lua = NK::Engine::Get().GetLuaManager();
		if (lua.RunScript("assets/scripts/game_match3.lua")) {
			lua.CallFunction("OnStart");
		}
        // Scene::OnStart() вызывается внутри Engine::Run
    }

    void OnUpdate(float deltaTime) override {
		// 1. Обновляем логику всех объектов (включая Lua-скрипты)
		NK::Engine::Get().GetScene().OnUpdate(deltaTime);

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
};

NK::Application* NK::CreateApplication() {
    return new SandboxApp();
}