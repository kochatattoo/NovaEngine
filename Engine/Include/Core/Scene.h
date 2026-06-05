#pragma once
#include <vector>
#include <memory>
#include "Core/GameObject.h"
#include <Renderer/OrthographicCamera.h>

namespace NK {

	class Scene {
	public:
		Scene() = default;

		GameObject* CreateGameObject(const std::string& name = "GameObject");
		// UI-объекты (рисуются в экранных координатах)
		GameObject* CreateUIObject(const std::string& name = "UIObject");
		void AddUIObject(GameObject* obj); // если объект создан вне сцены

		void OnRender();
		void OnStart();
		void OnUpdate(float deltaTime);

		OrthographicCamera& GetGameCamera() { return m_GameCamera; }
		OrthographicCamera& GetUICamera() { return m_UICamera; }

		const std::vector<std::unique_ptr<GameObject>>& GetObjects() const { return m_Objects; }
		const std::vector<std::unique_ptr<GameObject>>& GetUIObjects() const { return m_UIObjects; }

	private:
		// Игровой мир
		std::vector<std::unique_ptr<GameObject>> m_Objects;
		OrthographicCamera m_GameCamera{ -5.0f, 5.0f, -5.0f, 5.0f };

		// UI
		std::vector<std::unique_ptr<GameObject>> m_UIObjects;
		OrthographicCamera m_UICamera{ 0.0f, 1280.0f, 720.0f, 0.0f }; // top = 720, bottom = 0 → Y вниз

		bool m_Started = false;
	};

} // namespace NK