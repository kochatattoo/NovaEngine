#pragma once
#include <EnTT/entt/entt.hpp>
#include <memory>
#include <functional>

namespace NK::ECS {

    class World {
    public:
        World() = default;
        ~World() = default;

        // Создать сущность (пустую)
        entt::entity CreateEntity();

        // Уничтожить сущность
        void DestroyEntity(entt::entity entity);

        // Добавить компонент к сущности (если его ещё нет)
        template<typename T, typename... Args>
        T& AddComponent(entt::entity entity, Args&&... args) {
            return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        // Получить компонент (только чтение)
        template<typename T>
        const T& GetComponent(entt::entity entity) const {
            return m_Registry.get<T>(entity);
        }

        // Получить компонент (для изменения)
        template<typename T>
        T& GetComponent(entt::entity entity) {
            return m_Registry.get<T>(entity);
        }

        // Проверить, есть ли компонент
        template<typename T>
        bool HasComponent(entt::entity entity) const {
            return m_Registry.all_of<T>(entity);
        }

        // Удалить компонент
        template<typename T>
        void RemoveComponent(entt::entity entity) {
            m_Registry.remove<T>(entity);
        }

        // Выполнить функцию для всех сущностей, обладающих указанными компонентами
        template<typename... Components, typename Func>
        void ForEach(Func&& func) {
            auto view = m_Registry.view<Components...>();
            view.each(std::forward<Func>(func));
        }

        // Получить сырой registry (для продвинутых операций)
        entt::registry& GetRegistry() { return m_Registry; }
        const entt::registry& GetRegistry() const { return m_Registry; }

    private:
        entt::registry m_Registry;
    };

} // namespace NK::ECS