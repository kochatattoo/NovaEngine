#include "ECS/World.h"

namespace NK::ECS {

    entt::entity World::CreateEntity() {
        return m_Registry.create();
    }

    void World::DestroyEntity(entt::entity entity) {
        m_Registry.destroy(entity);
    }

} // namespace NK::ECS