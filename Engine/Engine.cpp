#include "Core/Engine.h"
#include "Core/Application.h"
#include "Core/Timer.h"
#include "Window/Window.h" 
#include "Renderer/Renderer.h" 

namespace NK {

    Engine* Engine::s_Instance = nullptr;

    Engine::Engine(const EngineConfig& config)
        : m_Config(config) {
        NK_CORE_INFO("Engine constructor");
        s_Instance = this;
    }

    Engine::~Engine() {
        NK_CORE_INFO("Engine destructor");
        s_Instance = nullptr;
    }

    void Engine::Initialize() {
        NK_CORE_INFO("Initializing subsystems...");
        // Создаём окно с параметрами из конфига
        m_Window = std::make_unique<Window>(WindowProperties{
            m_Config.Title,
            m_Config.Width,
            m_Config.Height
            });

        // Инициализируем рендерер, передавая нативное окно
        Renderer::Init(m_Window->GetNativeWindow());

        NK_CORE_INFO("Engine initialized.");
    }

    void Engine::Run(Application* app) {
        m_App = app;
        Initialize();

        m_App->OnStart();
        m_Running = true;

        Timer timer;
        while (m_Running) {
            float dt = timer.Tick();

            m_Window->OnUpdate();
            if (m_Window->ShouldClose()) {
                m_Running = false;
            }

            // Очищаем экран текущим цветом
            Renderer::Clear();

            m_App->OnUpdate(dt);
        }

        m_App->OnShutdown();
        Shutdown();
    }

    void Engine::Shutdown() {
        m_Running = false;
        // Очистка окна, если было создано
        NK_CORE_INFO("Engine shutdown complete.");
    }

} // namespace NK
