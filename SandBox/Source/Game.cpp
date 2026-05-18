#include <Core/EntryPoint.h>
#include <Core/Log.h>
#include <Input/Input.h>        // старый ввод
#include <Renderer/Renderer.h>
#include <Event/Event.h>

class SandboxApp : public NK::Application {
public:
    void OnStart() override {
        NK_INFO("Sandbox started! Press SPACE to change color, ESC to exit.");
        NK::Renderer::SetClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    }

    void OnUpdate(float deltaTime) override {
        // Выход по Escape (старый опрос)
        if (NK::Input::IsKeyDown(VK_ESCAPE)) {
            NK::Engine::Get().Shutdown();
        }

        // Меняем цвет фона при удержании пробела
        if (NK::Input::IsKeyDown(VK_SPACE)) {
            static float r = 0.1f, g = 0.2f, b = 0.3f;
            r += 0.01f; if (r > 1.0f) r = 0.0f;
            g += 0.02f; if (g > 1.0f) g = 0.0f;
            b += 0.03f; if (b > 1.0f) b = 0.0f;
            NK::Renderer::SetClearColor(r, g, b, 1.0f);
        }

        // Логирование мыши раз в секунду (для проверки)
        static float timer = 0.0f;
        timer += deltaTime;
        if (timer >= 1.0f) {
            int32_t mouseX, mouseY;
            NK::Input::GetMousePosition(mouseX, mouseY);
            NK_TRACE("Mouse position: {0}, {1}", mouseX, mouseY);
            timer = 0.0f;
        }
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
                // Меняем цвет фона при каждом нажатии
                static float r = 0.1f, g = 0.2f, b = 0.3f;
                r += 0.1f; if (r > 1.0f) r = 0.0f;
                g += 0.2f; if (g > 1.0f) g = 0.0f;
                b += 0.3f; if (b > 1.0f) b = 0.0f;
                NK::Renderer::SetClearColor(r, g, b, 1.0f);
                return true;
            }
            return false;   // не обработано
            });

        // Можно добавить обработку движения мыши и колёсика
        dispatcher.Dispatch<NK::MouseMovedEvent>([this](NK::MouseMovedEvent& mouseEvent) {
            // Выводим позицию в лог раз в секунду? Лучше в OnUpdate через статическую переменную.
            // Здесь можно просто заглушку.
            return false;
            });
    }

    void OnShutdown() override {
        NK_INFO("Sandbox shutting down.");
    }
};

NK::Application* NK::CreateApplication() {
    return new SandboxApp();
}