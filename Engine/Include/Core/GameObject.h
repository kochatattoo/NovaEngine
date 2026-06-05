#pragma once
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include "Core/Component.h"

namespace NK {

	class GameObject {
	public:
		GameObject(const std::string& name = "GameObject");
		~GameObject() = default;

		const std::string& GetName() const { return m_Name; }

		// Добавить компонент (создаёт экземпляр T с передачей аргументов)
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args) {
			static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
			auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
			T* rawPtr = component.get();
			m_Components.push_back(std::move(component));
			if (m_Started) rawPtr->OnStart();
			return rawPtr;
		}

		// Получить компонент по типу (первый найденный)
		template<typename T>
		T* GetComponent() {
			for (auto& c : m_Components) {
				T* casted = dynamic_cast<T*>(c.get());
				if (casted) return casted;
			}
			return nullptr;
		}

		void OnStart();   // вызывает OnStart для всех компонентов
		void OnUpdate(float deltaTime);

		bool IsStarted() const { return m_Started; }

	private:
		std::string m_Name;
		std::vector<std::unique_ptr<Component>> m_Components;
		bool m_Started = false;   // <-- новое
	};

} // namespace NK
