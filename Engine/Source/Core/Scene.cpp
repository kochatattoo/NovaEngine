#include "Core/Scene.h"
#include <Core/Log.h>
#include <Renderer/SpriteRenderer.h>
#include "Renderer/TextRenderer.h"
#include <Core/Engine.h>
#include <Window/Window.h>
#include <UI/Button.h>

namespace NK {

	GameObject* Scene::CreateGameObject(const std::string& name) {
		auto obj = std::make_unique<GameObject>(name);
		GameObject* raw = obj.get();
		m_Objects.push_back(std::move(obj));
		NK_CORE_INFO("GameObject '%s' created", name.c_str());
		if (m_Started) {
			raw->OnStart();
			NK_CORE_INFO("OnStart called for GameObject '%s'", name.c_str());
		}
		return raw;
	}

	GameObject* Scene::CreateUIObject(const std::string& name) {
		auto obj = std::make_unique<GameObject>(name);
		GameObject* raw = obj.get();
		m_UIObjects.push_back(std::move(obj));
		NK_CORE_INFO("UI Object '%s' created", name.c_str());
		if (m_Started) {
			raw->OnStart();
			NK_CORE_INFO("OnStart called for UI object '%s'", name.c_str());
		}
		return raw;
	}

	void Scene::RegisterAnchor(Anchor* anchor) {
		m_Anchors.push_back(anchor);
	}

	void Scene::RecalculateAnchors(uint32_t width, uint32_t height) {
		for (auto* anchor : m_Anchors) {
			anchor->UpdatePosition(width, height);
		}
	}

	void Scene::AddUIObject(GameObject* obj) {
		// Если объект уже принадлежит другому списку, нужно быть осторожным.
		// Пока предполагаем, что obj был создан через CreateGameObject и ещё не добавлен в UI.
		// Для простоты мы не будем реализовывать перенос владения, а просто добавим сырой указатель.
		// Но лучше использовать CreateUIObject изначально.
		// Оставим метод пустым или добавим предупреждение.
		NK_CORE_WARN("AddUIObject not fully implemented, use CreateUIObject instead");
	}

	void Scene::OnRender() {
		// Рендерим игровой мир
		const glm::mat4& gameViewProj = m_GameCamera.GetViewProjectionMatrix();
		for (auto& obj : m_Objects) {
			auto* sr = obj->GetComponent<SpriteRenderer>();
			if (sr) {
				sr->Render(gameViewProj);
			}
		}

		// Рендерим UI
		const glm::mat4& uiViewProj = m_UICamera.GetViewProjectionMatrix();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);

		for (auto& obj : m_UIObjects) {
			auto* tr = obj->GetComponent<TextRenderer>();
			if (tr) {
				// NK_CORE_INFO("Rendering TextRenderer");

				/*NK_CORE_INFO("UI ViewProj matrix: [0][0]=%.3f, [1][1]=%.3f, [3][0]=%.1f, [3][1]=%.1f",
					uiViewProj[0][0], uiViewProj[1][1], uiViewProj[3][0], uiViewProj[3][1]);*/

				tr->Render(uiViewProj);
			}
			auto* sr = obj->GetComponent<SpriteRenderer>();
			if (sr) {
				// NK_CORE_INFO("Rendering SpriteRenderer (UI)");
				sr->Render(uiViewProj);
			}
		}
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

	void Scene::OnStart() {
		NK_CORE_INFO("Scene OnStart");
		// Обновляем камеры под размеры окна
		auto* window = Engine::Get().GetWindow();
		uint32_t w = window->GetWidth();
		uint32_t h = window->GetHeight();

		m_GameCamera.OnWindowResized(w, h);
		m_UICamera.SetProjection(0.0f, (float)w, (float)h, 0.0f);

		// Запускаем все объекты
		for (auto& obj : m_Objects) {
			obj->OnStart();
		}
		for (auto& obj : m_UIObjects) {
			obj->OnStart();
		}
		m_Started = true;
	}

	void Scene::OnUpdate(float deltaTime) {
		for (auto& obj : m_Objects) {
			obj->OnUpdate(deltaTime);
		}
		for (auto& obj : m_UIObjects) {
			obj->OnUpdate(deltaTime);
		}
	}

} // namespace NK