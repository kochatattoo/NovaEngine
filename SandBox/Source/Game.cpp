#include <Core/EntryPoint.h>   // Здесь определён main()
#include <Core/Log.h>          // Логи
#include <Core/Application.h>  // Класс приложения
#include <Input/Input.h>
#include <Renderer/Renderer.h>

// Наше игровое приложение
class SandboxApp : public NK::Application {
public:
    void OnStart() override {
        NK_INFO("Sandbox started!");
    }

    void OnUpdate(float deltaTime) override {
        // Выход по Escape
        if (NK::Input::IsKeyDown(VK_ESCAPE)) {
            NK::Engine::Get().Shutdown();
        }

        // Меняем цвет фона по пробелу (каждый кадр, пока нажат)
        if (NK::Input::IsKeyDown(VK_SPACE)) {
            // Генерируем случайный цвет
            static float r = 0.1f, g = 0.2f, b = 0.3f;
            r += 0.01f; if (r > 1.0f) r = 0.0f;
            g += 0.02f; if (g > 1.0f) g = 0.0f;
            b += 0.03f; if (b > 1.0f) b = 0.0f;
            NK::Renderer::SetClearColor(r, g, b, 1.0f);
        }

        // Выводим позицию мыши раз в секунду
        static float timer = 0.0f;
        timer += deltaTime;
        if (timer >= 1.0f) {
            int32_t mouseX, mouseY;
            NK::Input::GetMousePosition(mouseX, mouseY);
            NK_TRACE("Mouse: {0}, {1}", mouseX, mouseY);
            timer = 0.0f;
        }
    }

    void OnShutdown() override {
        NK_INFO("Sandbox shutting down.");
    }
};

// Обязательное определение фабричной функции
NK::Application* NK::CreateApplication() {
    return new SandboxApp();
}