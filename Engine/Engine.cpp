#include "Core/Engine.h"
#include "Core/Application.h"
#include "Core/Timer.h"
#include "Window/Window.h"
#include "Renderer/Renderer.h"
#include "Renderer/GraphicsContext.h"
#include "Input/Input.h"          // для опроса клавиш в Lua
#include "Core/LuaManager.h"
#include <sol/sol.hpp>

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

		// Настройка биндингов Lua (должна быть после создания LuaManager, который уже создан в конструкторе Engine)
		SetupLuaBindings();

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

	void Engine::SetupLuaBindings() {
		sol::state& L = m_LuaManager.GetState();

		// Регистрируем функцию логирования
		L.set_function("Log", [](const std::string& msg) {
			NK_INFO("%s", msg.c_str());
			});

		// Регистрируем опрос клавиш (через старый Input)
		L.set_function("IsKeyDown", [](int key) -> bool {
			return Input::IsKeyDown(key);
			});

		// Установка цвета очистки фона
		L.set_function("SetClearColor", [](float r, float g, float b, float a) {
			Renderer::SetClearColor(r, g, b, a);
			});

		// Регистрируем получение Engine
		L.set_function("GetEngine", [this]() -> Engine& {
			return *this;
			});

		// Позже можно экспортировать классы Renderer, Application и т.д.
	}

	void Engine::Shutdown() {
		m_Running = false;
		// Очистка окна, если было создано
		NK_CORE_INFO("Engine shutdown complete.");
	}
} // namespace NK