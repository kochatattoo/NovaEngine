#pragma once
#include <string>
#include <memory>   // для std::unique_ptr (Scope)
#include "Core/Log.h"
#include "Core/Application.h"

namespace NK {

    // Пока минимальная конфигурация
    struct EngineConfig {
        std::string Title = "NovaEngine";
        uint32_t Width = 1280;
        uint32_t Height = 720;
    };

    // Главный класс движка
    class Engine {
    public:
        Engine(const EngineConfig& config);
        ~Engine();

        // Запустить главный цикл с переданным приложением
        void Run(Application* app);
        // Остановить цикл (можно вызвать из игры)
        void Shutdown();

        // Получить ссылку на окно (понадобится позже)
        class Window* GetWindow() { return m_Window.get(); }
        static Engine& Get() { return *s_Instance; }

    private:
        void Initialize();  // Инициализация подсистем
        void MainLoop();    // Главный цикл (будет вынесен из Run для гибкости)

        bool m_Running = false;
        std::unique_ptr<class Window> m_Window; // Умный указатель, как в C# объекты управляются автоматически
        Application* m_App = nullptr;           // Сырой указатель, владение у игры
        EngineConfig m_Config;

        static Engine* s_Instance;  // Синглтон
    };

} // namespace NK