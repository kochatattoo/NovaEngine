# Roadmap

> 🚧 **TODO:** страница-заготовка.

## Текущий фокус: v0.2 — полная ECS-миграция Match3

| # | Цель | Статус |
|---|---|---|
| v0.2.1 | World API: name-based lookup | ✅ |
| v0.2.2 | `Match3TileComponent` | ✅ |
| v0.2.3 | `Match3System` (логика + 100 entities) | ✅ |
| v0.2.4 | `Match3Game` рефакторинг | ✅ |
| v0.2.5 | Lua биндинги (`GetBoard()`) | ✅ |
| v0.2.6 | `SpriteRenderSystem` | ⏳ |
| v0.2.7 | Удалить `Match3Board` (POD) | ⏳ |

## Следующие

- **v0.3** — UI на ECS (Anchor, Button как компоненты).
- **v0.4** — сериализация World (save/load).
- **v0.5** — удалить `Scene/GameObject/Component` (legacy).

## v0.6+ (долгосрочно)

- Звук (miniaudio).
- Камера-как-entity (CameraComponent).
- Пост-процессинг.
- Многопоточность (job system).
- Linux/macOS порт.
- ImGui editor.
- CMake (вместо vcxproj).
- CI/CD (GitHub Actions).

→ [полный roadmap в `docs/06_План_разработки/`](../docs/06_План_разработки/)
