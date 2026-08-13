#include "ECS/World.h"
#include "ECS/Components/NameComponent.h"

namespace NK::ECS {

    entt::entity World::CreateEntity()
    {
        return m_Registry.create();
    }

    entt::entity World::CreateEntity(const std::string& name)
    {
        entt::entity entity = m_Registry.create();
        RenameEntity(entity, name);
        return entity;
    }

    void World::DestroyEntity(entt::entity entity)
    {
        if (m_Registry.all_of<NameComponent>(entity))
        {
            const std::string& name = m_Registry.get<NameComponent>(entity).Name;
            m_NamedEntities.erase(name);
        }
        m_Registry.destroy(entity);
    }

    entt::entity World::GetEntityByName(const std::string& name) const
    {
        auto it = m_NamedEntities.find(name);
        if (it != m_NamedEntities.end())
        {
            return it->second;
        }
        return entt::null;
    }

    void World::RenameEntity(entt::entity entity, const std::string& newName)
    {
        if (!m_Registry.valid(entity))
        {
            return;
        }

        // Удаляем старое имя из map, если было
        if (m_Registry.all_of<NameComponent>(entity))
        {
            const std::string& oldName = m_Registry.get<NameComponent>(entity).Name;
            if (oldName != newName)
            {
                m_NamedEntities.erase(oldName);
            }
        }

        // Устанавливаем новое имя
        if (m_Registry.all_of<NameComponent>(entity))
        {
            m_Registry.get<NameComponent>(entity).Name = newName;
        }
        else
        {
            m_Registry.emplace<NameComponent>(entity, newName);
        }

        // Регистрируем в map (если имя не пустое)
        if (!newName.empty())
        {
            m_NamedEntities[newName] = entity;
        }
    }

    std::string World::GetEntityName(entt::entity entity) const
    {
        if (!m_Registry.valid(entity))
        {
            return "";
        }
        if (!m_Registry.all_of<NameComponent>(entity))
        {
            return "";
        }
        return m_Registry.get<NameComponent>(entity).Name;
    }

    void World::Clear()
    {
        m_NamedEntities.clear();
        m_Registry.clear();
    }

} // namespace NK::ECS
