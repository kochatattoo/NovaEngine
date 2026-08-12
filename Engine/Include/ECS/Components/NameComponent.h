#pragma once
#include <string>

namespace NK::ECS {

    // Простой компонент-имя для entity. Используется World для lookup по имени.
    struct NameComponent {
        std::string Name;
    };

} // namespace NK::ECS
