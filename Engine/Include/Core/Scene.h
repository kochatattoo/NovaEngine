#pragma once
#include <vector>
#include <memory>
#include "Core/GameObject.h"

namespace NK {

	class Scene {
	public:
		Scene() = default;

		GameObject* CreateGameObject(const std::string& name = "GameObject");

		void OnRender();
		void OnStart();
		void OnUpdate(float deltaTime);

		const std::vector<std::unique_ptr<GameObject>>& GetObjects() const { return m_Objects; }

	private:
		std::vector<std::unique_ptr<GameObject>> m_Objects;
	};

} // namespace NK