#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include <Core/Log.h>

namespace NK {

	GameObject::GameObject(const std::string& name) : m_Name(name) {
		// Каждый объект обязательно имеет Transform
		AddComponent<Transform>();
		NK_CORE_INFO("GameObject '%s' created", name.c_str());
	}

	void GameObject::OnStart() {
		for (auto& c : m_Components) {
			c->OnStart();
		}
		m_Started = true;
	}

	void GameObject::OnUpdate(float deltaTime) {
		for (auto& c : m_Components) {
			c->OnUpdate(deltaTime);
		}
	}

} // namespace NK