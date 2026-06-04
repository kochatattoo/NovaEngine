#include "Core/Scene.h"
#include <Core/Log.h>
#include <Renderer/SpriteRenderer.h>

namespace NK {

	GameObject* Scene::CreateGameObject(const std::string& name) {
		auto obj = std::make_unique<GameObject>(name);
		GameObject* raw = obj.get();
		m_Objects.push_back(std::move(obj));
		return raw;
	}

	void Scene::OnRender() {
		for (auto& obj : m_Objects) {
			auto* sr = obj->GetComponent<SpriteRenderer>();
			if (sr) sr->Render();
		}
	}

	void Scene::OnStart() {
		NK_CORE_INFO("Scene OnStart");
		for (auto& obj : m_Objects) {
			obj->OnStart();
		}
	}

	void Scene::OnUpdate(float deltaTime) {
		for (auto& obj : m_Objects) {
			obj->OnUpdate(deltaTime);
		}
	}

} // namespace NK