#pragma once
#include <vector>
#include <memory>
#include "Core/GameObject.h"
#include <Renderer/OrthographicCamera.h>
#include <UI/Anchor.h>

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

		void RegisterAnchor(Anchor* anchor);
		void RecalculateAnchors(uint32_t width, uint32_t height);

		const std::vector<std::unique_ptr<GameObject>>& GetObjects() const { return m_Objects; }
		const std::vector<std::unique_ptr<GameObject>>& GetUIObjects() const { return m_UIObjects; }

	private:
		// Игровой мир
		std::vector<std::unique_ptr<GameObject>> m_Objects;
		OrthographicCamera m_GameCamera{ -5.0f, 5.0f, -5.0f, 5.0f };

		// UI
		std::vector<std::unique_ptr<GameObject>> m_UIObjects;
		OrthographicCamera m_UICamera{ 0.0f, 1280.0f, 720.0f, 0.0f }; // top = 720, bottom = 0 → Y вниз

		std::vector<Anchor*> m_Anchors;

		bool m_Started = false;
	};

} // namespace NK