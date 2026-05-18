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
		m_Window = std::make_unique<Window>(WindowProperties{
			m_Config.Title, m_Config.Width, m_Config.Height
			});
		// Инициализируем рендерер (после того, как контекст уже создан внутри Window)
		Renderer::Init();
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

			m_Window->OnUpdate();                   // обработка сообщений
			if (m_Window->ShouldClose())
			{
				NK_CORE_INFO("Window should close, breaking loop");
				m_Running = false;
			}

			// Извлекаем все накопившиеся события и отправляем в приложение
			{
				auto event = m_Window->PollEvent();
				while (event) {
					NK_TRACE("Event: {0}", event->GetName()); // отладочный вывод (можно убрать)
					m_App->OnEvent(*event);
					event = m_Window->PollEvent();
				}
			}

			// Активируем контекст OpenGL (обычно уже активен, но для надёжности)
			if (auto* ctx = m_Window->GetGraphicsContext())
				ctx->MakeCurrent();
			Renderer::BeginFrame();                 // очистка

			m_App->OnUpdate(dt);                    // игровая логика

			Renderer::EndFrame();
			if (auto* ctx = m_Window->GetGraphicsContext())
				ctx->SwapBuffers();                 // показываем кадр
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