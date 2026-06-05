#include "Core/Engine.h"
#include "Core/Application.h"
#include "Core/Timer.h"
#include "Core/Transform.h"
#include "Core/ScriptComponent.h"
#include "Renderer/SpriteRenderer.h"
#include "Window/Window.h"
#include "Renderer/Renderer.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/Font.h"
#include "Input/Input.h"          // для опроса клавиш в Lua
#include "Core/LuaManager.h"
#include <sol/sol.hpp>
#include <Renderer/TextRenderer.h>
#include <UI/Button.h>
#include <UI/Anchor.h>
#include <Game/Match3Board.h>

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
			"SetPosition", [](Transform& t, double x, double y, double z) {
				t.SetPosition(glm::vec3((float)x, (float)y, (float)z));
			},
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
			"SetAlignment", [](SpriteRenderer& sr, double h, double v) { sr.SetAlignment((float)h, (float)v); },
			"SetIsUI", &SpriteRenderer::SetIsUI,
			"SetColor", [](SpriteRenderer& sr, double r, double g, double b, double a) {
				sr.SetColor((float)r, (float)g, (float)b, (float)a);
			},
			"SetUseColor", & SpriteRenderer::SetUseColor,
			sol::base_classes, sol::bases<Component>()
		);

		// GameObject
		L.new_usertype<GameObject>("GameObject",
			"AddComponent_Transform", [](GameObject& obj) -> Transform* {
				auto* t = obj.GetComponent<Transform>();
				if (t) return t;                              // возвращаем существующий
				return obj.AddComponent<Transform>();          // если нет – создаём
			},
			"AddComponent_SpriteRenderer", [](GameObject& obj) { return obj.AddComponent<SpriteRenderer>();},
			"AddComponent_TextRenderer", [](GameObject& obj) { return obj.AddComponent<TextRenderer>(); },
			"AddComponent_Button", [](GameObject& obj) { return obj.AddComponent<Button>(); },
			"AddComponent_Script", [](GameObject& obj, const std::string& path) { return obj.AddComponent<ScriptComponent>(path); },
			"AddComponent_Anchor", [](GameObject& obj) { return obj.AddComponent<Anchor>(); },
			"GetTransform", [](GameObject& obj) { return obj.GetComponent<Transform>(); },
			"GetSpriteRenderer", [](GameObject& obj) { return obj.GetComponent<SpriteRenderer>(); },
			"GetTextRenderer", [](GameObject& obj) { return obj.GetComponent<TextRenderer>(); },
			"GetButton", [](GameObject& obj) { return obj.GetComponent<Button>(); },
			"SetZOrder", & GameObject::SetZOrder,
			"GetZOrder", & GameObject::GetZOrder,
			"GetName", &GameObject::GetName,
			"OnStart", &GameObject::OnStart,
			"OnUpdate", &GameObject::OnUpdate
		);

		// Scene
		L.new_usertype<Scene>("Scene",
			"CreateGameObject", &Scene::CreateGameObject,
			"CreateUIObject", &Scene::CreateUIObject,
			"OnStart", &Scene::OnStart,
			"OnUpdate", &Scene::OnUpdate,
			"OnRender", &Scene::OnRender,
			"GetGameCamera", &Scene::GetGameCamera,   // <-- добавить
			"GetUICamera", &Scene::GetUICamera        // <-- добавить (на будущее)
		);

		L.new_usertype<OrthographicCamera>("OrthographicCamera",
			"GetLeft", &OrthographicCamera::GetLeft,
			"GetRight", &OrthographicCamera::GetRight,
			"GetBottom", &OrthographicCamera::GetBottom,
			"GetTop", &OrthographicCamera::GetTop
		);

		// Font
		L.new_usertype<Font>("Font",
			"CreateTextTexture", &Font::CreateTextTexture
		);

		// TextRenderer
		L.new_usertype<TextRenderer>("TextRenderer",
			"SetFont", &TextRenderer::SetFont,
			"SetText", &TextRenderer::SetText,
			"SetFontSize", [](TextRenderer& tr, double size) { tr.SetFontSize((float)size); },
			"SetAlignment", [](TextRenderer& tr, double h, double v) { tr.SetAlignment((float)h, (float)v); },
			"SetColor", [](TextRenderer& tr, int r, int g, int b, int a) {
				tr.SetColor((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
			},
			sol::base_classes, sol::bases<Component>()
		);

		// Button
		L.new_usertype<Button>("Button",
			"SetCallback", &Button::SetCallback,
			"SetOnClick", &Button::SetOnClick,
			"SetOnPointerDown", &Button::SetOnPointerDown,
			"SetOnPointerUp", &Button::SetOnPointerDown,
			"SetOnPointerExit", &Button::SetOnPointerExit,
			"SetOnPointerUp", &Button::SetOnPointerUp,
			"SetSize", [](Button& btn, double x, double y) {
				btn.SetSize((float)x, (float)y);
			},
			sol::base_classes, sol::bases<Component>()
		);

		// Anchor
		L.new_usertype<Anchor>("Anchor",
			"SetPreset", [](Anchor& a, int preset) { a.SetPreset(static_cast<AnchorPreset>(preset)); },
			"SetScreenAnchor", [](Anchor& a, double sx, double sy) { a.SetScreenAnchor((float)sx, (float)sy); },
			"SetObjectAnchor", [](Anchor& a, double ox, double oy) { a.SetObjectAnchor((float)ox, (float)oy); },
			"SetSize", [](Anchor& a, double w, double h) { a.SetSize(glm::vec2((float)w, (float)h)); },
			sol::base_classes, sol::bases<Component>()
		);

		L.new_usertype<Match3Board>("Match3Board",
			sol::constructors<Match3Board(int, int, double, double)>(),
			"FillRandom", &Match3Board::FillRandom,
			"GetTile", &Match3Board::GetTile,
			"SetTile", &Match3Board::SetTile,
			"Swap", &Match3Board::Swap,
			"FindMatches", &Match3Board::FindMatches,
			"RemoveTiles", &Match3Board::RemoveTiles,
			"ApplyGravity", &Match3Board::ApplyGravity,
			"FillEmpty", &Match3Board::FillEmpty,
			"GetCellPosition", [](Match3Board& board, int r, int c) -> std::tuple<float, float> {
				auto pos = board.GetCellPosition(r, c);
				return { pos.x, pos.y };
			},
			"GetRows", & Match3Board::GetRows,
			"GetCols", & Match3Board::GetCols,
			"OnTileChanged", & Match3Board::OnTileChanged
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

		L.set_function("LoadFont", [](const std::string& path) -> std::shared_ptr<Font> {
			try {
				auto font = std::make_shared<Font>(path);
				return font;
			}
			catch (...) {
				return nullptr;
			}
			});
	}

	void Engine::Shutdown() {
		m_Running = false;
		// Очистка окна, если было создано
		NK_CORE_INFO("Engine shutdown complete.");
	}
} // namespace NK