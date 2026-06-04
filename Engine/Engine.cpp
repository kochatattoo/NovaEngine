#include "Core/Engine.h"
#include "Core/Application.h"
#include "Core/Timer.h"
#include "Core/Transform.h"
#include "Core/ScriptComponent.h"
#include "Renderer/SpriteRenderer.h"
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

		m_Window->ResizeCallback = [this](uint32_t w, uint32_t h) {
			m_Scene.GetCamera().OnWindowResized(w, h);
			// Также обновим viewport OpenGL
			glViewport(0, 0, w, h);
			};
		// Инициализируем рендерер (после того, как контекст уже создан внутри Window)
		Renderer::Init();

		// Настройка биндингов Lua (должна быть после создания LuaManager, который уже создан в конструкторе Engine)
		SetupLuaBindings();

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
		sol::state& L = m_LuaManager.GetState();

		// Регистрируем Transform
		L.new_usertype<Transform>("Transform",
			"SetPosition", sol::overload(
				[](Transform& t, float x, float y, float z) {
					t.SetPosition(glm::vec3(x, y, z));
				},
				[](Transform& t, const glm::vec3& pos) {
					t.SetPosition(pos);
				}
			),
			"GetPosition", [](Transform& t) -> std::tuple<float, float, float> {
				auto& p = t.GetPosition();
				return { p.x, p.y, p.z };
			},
			"SetRotation", &Transform::SetRotation,
			"GetRotationDegrees", &Transform::GetRotationDegrees,
			"SetScale", [](Transform& t, float x, float y) { t.SetScale(glm::vec2(x, y)); },
			"GetScale", [](Transform& t) -> std::tuple<float, float> {
				auto& s = t.GetScale();
				return { s.x, s.y };
			},
			sol::base_classes, sol::bases<Component>()
		);

		// SpriteRenderer
		L.new_usertype<SpriteRenderer>("SpriteRenderer",
			"SetTexture", &SpriteRenderer::SetTexture,
			"SetShader", &SpriteRenderer::SetShader,
			sol::base_classes, sol::bases<Component>()
		);

		// GameObject
		L.new_usertype<GameObject>("GameObject",
			"AddComponent_Transform", [](GameObject& obj) { return obj.AddComponent<Transform>(); },
			"AddComponent_SpriteRenderer", [](GameObject& obj) { return obj.AddComponent<SpriteRenderer>(); },
			"AddComponent_Script", [](GameObject& obj, const std::string& path) { return obj.AddComponent<ScriptComponent>(path); },
			"GetTransform", [](GameObject& obj) { return obj.GetComponent<Transform>(); },
			"GetSpriteRenderer", [](GameObject& obj) { return obj.GetComponent<SpriteRenderer>(); },
			"GetName", &GameObject::GetName,
			"OnStart", &GameObject::OnStart,
			"OnUpdate", &GameObject::OnUpdate
		);

		// Scene
		L.new_usertype<Scene>("Scene",
			"CreateGameObject", &Scene::CreateGameObject,
			"OnStart", &Scene::OnStart,
			"OnUpdate", &Scene::OnUpdate
		);

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

		// Предоставим доступ к сцене из Lua
		L.set_function("GetScene", [this]() -> Scene& { return m_Scene; });

		L.set_function("GetTexture", [](const std::string& path) {
			return Engine::Get().GetResourceManager().GetTexture(path);
			});

		L.set_function("GetShader", [](const std::string& name, const std::string& vSrc, const std::string& fSrc) {
			return Engine::Get().GetResourceManager().GetShader(name, vSrc, fSrc);
			});
	}

	void Engine::Shutdown() {
		m_Running = false;
		// Очистка окна, если было создано
		NK_CORE_INFO("Engine shutdown complete.");
	}
} // namespace NK