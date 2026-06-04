#include "Core/Scene.h"
#include <Core/Log.h>
#include <Renderer/SpriteRenderer.h>
#include <Core/Engine.h>
#include <Window/Window.h>

namespace NK {

	GameObject* Scene::CreateGameObject(const std::string& name) {
		auto obj = std::make_unique<GameObject>(name);
		GameObject* raw = obj.get();
		m_Objects.push_back(std::move(obj));
		return raw;
	}

	void Scene::OnRender() {
		const glm::mat4& viewProj = m_Camera.GetViewProjectionMatrix();
		for (auto& obj : m_Objects) {
			auto* sr = obj->GetComponent<SpriteRenderer>();
			if (sr) {
				sr->Render(viewProj); // передаём матрицу
			}
		}
	}

	void Scene::OnStart() {
		NK_CORE_INFO("Scene OnStart");
		auto* window = Engine::Get().GetWindow();
		m_Camera.OnWindowResized(window->GetWidth(), window->GetHeight());

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