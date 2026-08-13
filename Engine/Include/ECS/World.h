#pragma once
#include <entt/entt/entt.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace NK::ECS {

    // v0.2: World — обёртка над entt::registry.
    // Скрывает entt::entity (32-битный ID) за типизированными методами.
    // v0.2.1: добавлен name-based lookup через NameComponent.
    class World
    {
    public:
        World() = default;
        ~World() = default;

        // Создание / удаление entities
        entt::entity CreateEntity();
        entt::entity CreateEntity(const std::string& name);
        void DestroyEntity(entt::entity entity);
        void Clear();

        // Компоненты
        template<typename T, typename... Args>
        T& AddComponent(entt::entity entity, Args&&... args)
        {
            return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename T>
        const T& GetComponent(entt::entity entity) const
        {
            return m_Registry.get<T>(entity);
        }

        template<typename T>
        T& GetComponent(entt::entity entity)
        {
            return m_Registry.get<T>(entity);
        }

        template<typename T>
        bool HasComponent(entt::entity entity) const
        {
            return m_Registry.all_of<T>(entity);
        }

        template<typename T>
        void RemoveComponent(entt::entity entity)
        {
            m_Registry.remove<T>(entity);
        }

        // Итерация
        template<typename... Components, typename Func>
        void ForEach(Func&& func)
        {
            auto view = m_Registry.view<Components...>();
            view.each(std::forward<Func>(func));
        }

        // Прямой доступ к registry
        entt::registry& GetRegistry() { return m_Registry; }
        const entt::registry& GetRegistry() const { return m_Registry; }

        // Именованные entities
        entt::entity GetEntityByName(const std::string& name) const;       // entt::null если не найдено
        void RenameEntity(entt::entity entity, const std::string& newName);
        std::string GetEntityName(entt::entity entity) const;              // "" если нет NameComponent

    private:
        entt::registry m_Registry;
        std::unordered_map<std::string, entt::entity> m_NamedEntities;
    };

} // namespace NK::ECS
