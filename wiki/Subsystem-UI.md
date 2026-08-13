# Subsystem: UI (Button, Anchor)

> 🚧 **TODO:** страница-заготовка.

## Классы

- **`Anchor`** — рассчитывает позицию UI-элемента от окна.
- **`Button`** — кнопка с hover/click, callback `m_OnClick`.

## Привязка

```cpp
button->SetScreenAnchor(0.5f, 0.5f);   // центр экрана
button->SetObjectAnchor(0.5f, 0.5f);   // центр объекта
button->SetSize(120, 40);              // в пикселях
```

Итоговая позиция = `ScreenAnchor * (windowW, windowH) - ObjectAnchor * (w, h)`.

## TODO

- UI-текст (TextRenderer на UI-камере) — ✅ есть.
- Drag & drop — ⏳.
- Layouts (Vertical, Horizontal) — ⏳.
- Миграция на ECS (v0.3).

→ [полная документация](../docs/02_Подсистемы/08_UI.md)
