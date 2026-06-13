#include "Core/Engine.h"
#include "Core/Application.h"
#include "Core/Timer.h"
#include "Scene/Transform.h"
#include "Scene/ScriptComponent.h"
#include "Renderer/SpriteRenderer.h"
#include "Window/Window.h"
#include "Renderer/Renderer.h"
#include "Renderer/GraphicsContext.h"
#include "Lua/LuaManager.h"
#include <Renderer/TextRenderer.h>
#include <Lua/LuaBindings.h>

#include "Input/InputManager.h"

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
		SetConsoleOutputCP(CP_UTF8);
		NK_CORE_INFO("Initializing subsystems...");
		m_Window = std::make_unique<Window>(WindowProperties{
			m_Config.Title, m_Config.Width, m_Config.Height
			});

		m_Window->ResizeCallback = [this](uint32_t w, uint32_t h) {
			m_Scene.GetGameCamera().OnWindowResized(w, h);
			m_Scene.GetUICamera().SetProjection(0.0f, (float)w, (float)h, 0.0f);
			// Также обновим viewport OpenGL
			glViewport(0, 0, w, h);
			m_Scene.RecalculateAnchors(w, h);
			};
		
		InputManager::Get().SetWindowHandle(static_cast<HWND>(m_Window->GetNativeWindow()));
		
		// Инициализируем рендерер (после того, как контекст уже создан внутри Window)
		Renderer::Init();

		// Настройка биндингов Lua (должна быть после создания LuaManager, который уже создан в конструкторе Engine)
		SetupLuaBindings();

		std::string defaultV = R"(#version 330 core
						layout(location = 0) in vec2 a_Position;
						layout(location = 1) in vec2 a_TexCoord;
						uniform mat4 u_ViewProjection;
						uniform mat4 u_Model;
						out vec2 v_TexCoord;
				void main() {
					gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 0.0, 1.0);
					v_TexCoord = a_TexCoord;
				}
			)";
		std::string defaultF = R"(#version 330 core
						in vec2 v_TexCoord;
						out vec4 FragColor;
						uniform sampler2D u_Texture;
				void main() {
					FragColor = texture(u_Texture, v_TexCoord);
				}
			)";

		auto defaultShader = std::make_shared<Shader>(defaultV, defaultF);
		m_ResourceManager.GetShaderPool().Put("DefaultSprite", defaultShader); // нужен метод Put в ResourcePool

		NK_CORE_INFO("Engine initialized.");
	}

	void Engine::Run(Application* app) {
		m_App = app;
		Initialize();
		m_Scene.OnStart();
		m_App->OnStart();
		m_Running = true;

		Timer timer;
		while (m_Running) {
			float dt = timer.Tick();

			m_Window->OnUpdate();                   // обработка сообщений
			InputManager::Get().Update(); 
			if (m_Window->ShouldClose())
			{
				NK_CORE_INFO("Window should close, breaking loop");
				m_Running = false;
			}
			
			// Активируем контекст OpenGL (обычно уже активен, но для надёжности)
			if (auto* ctx = m_Window->GetGraphicsContext())
				ctx->MakeCurrent();
			Renderer::BeginFrame();                 // очистка

			m_Scene.OnUpdate(dt);
			m_App->OnUpdate(dt);                    // игровая логика

			Renderer::EndFrame();
			if (auto* ctx = m_Window->GetGraphicsContext())
				ctx->SwapBuffers();                 // показываем кадр
		}

		m_App->OnShutdown();
		Shutdown();
	}

	void Engine::SetupLuaBindings() {
		LuaBindings::RegisterAll(m_LuaManager);
	}

	void Engine::Shutdown() {
		m_Running = false;
		// Очистка окна, если было создано
		NK_CORE_INFO("Engine shutdown complete.");
	}
} // namespace NK