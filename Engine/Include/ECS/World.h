#pragma once
#include <EnTT/entt/entt.hpp>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>

namespace NK::ECS {

    class World {
    public:
        World() = default;
        ~World() = default;

        // Создать пустую entity
        entt::entity CreateEntity();
        // Создать entity с именем (для lookup из Lua / editor)
        entt::entity CreateEntity(const std::string& name);

        // Удалить entity
        void DestroyEntity(entt::entity entity);

        // Получить компонент в entity (если его нет — создать)
        template<typename T, typename... Args>
        T& AddComponent(entt::entity entity, Args&&... args) {
            return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        // Получить компонент (только чтение)
        template<typename T>
        const T& GetComponent(entt::entity entity) const {
            return m_Registry.get<T>(entity);
        }

        // Получить компонент (мутабельный)
        template<typename T>
        T& GetComponent(entt::entity entity) {
            return m_Registry.get<T>(entity);
        }

        // Проверить, есть ли компонент
        template<typename T>
        bool HasComponent(entt::entity entity) const {
            return m_Registry.all_of<T>(entity);
        }

        // Убрать компонент
        template<typename T>
        void RemoveComponent(entt::entity entity) {
            m_Registry.remove<T>(entity);
        }

        // Итерация для всех entities с заданным набором компонентов
        template<typename... Components, typename Func>
        void ForEach(Func&& func) {
            auto view = m_Registry.view<Components...>();
            view.each(std::forward<Func>(func));
        }

        // Прямой доступ к registry (для продвинутых сценариев)
        entt::registry& GetRegistry() { return m_Registry; }
        const entt::registry& GetRegistry() const { return m_Registry; }

        // === Именованные entities (v0.2) ===

        // Найти entity по имени. Возвращает entt::null если не найдено.
        entt::entity GetEntityByName(const std::string& name) const;

        // Переименовать entity. Если имя уже занято — перезаписывает.
        void RenameEntity(entt::entity entity, const std::string& newName);

        // Получить имя entity. Возвращает "" если entity не имеет компонента NameComponent.
        std::string GetEntityName(entt::entity entity) const;

        // Удалить все entities
        void Clear();

    private:
        entt::registry m_Registry;
        std::unordered_map<std::string, entt::entity> m_NamedEntities;
    };

} // namespace NK::ECS
