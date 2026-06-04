#pragma once
#include <string>

namespace NK {

	class GameObject; // forward declaration

	class Component {
	public:
		Component(GameObject* owner) : m_Owner(owner) {}
		virtual ~Component() = default;

		// Вызывается при старте сцены (после добавления всех компонент)
		virtual void OnStart() {}
		// Вызывается каждый кадр
		virtual void OnUpdate(float deltaTime) {}

		GameObject& GetOwner() const { return *m_Owner; }

	protected:
		GameObject* m_Owner;
	};

} // namespace NK
