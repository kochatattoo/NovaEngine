#pragma once
#include <string>

namespace NK::ECS {

    // v0.2.1: Простой компонент-имя для entity. Используется World для lookup по имени.
    // Хранится в unordered_map<string, entity> в World.
    struct NameComponent
    {
        std::string Name;
    };

} // namespace NK::ECS
