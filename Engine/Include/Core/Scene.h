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

		void OnRender();
		void OnStart();
		void OnUpdate(float deltaTime);

		OrthographicCamera& GetCamera() { return m_Camera; }

		const std::vector<std::unique_ptr<GameObject>>& GetObjects() const { return m_Objects; }

	private:
		std::vector<std::unique_ptr<GameObject>> m_Objects;
		OrthographicCamera m_Camera{ -5.0f, 5.0f, -5.0f, 5.0f }; // начальные границы (будут пересчитаны при OnStart/Resize)
	};

} // namespace NK