# Subsystem: Renderer (OpenGL 3.3, SpriteRenderer, TextRenderer, Camera)

> 🚧 **TODO:** страница-заготовка.

## Компоненты

- **`GraphicsContext`** — инициализация OpenGL через WGL.
- **`Shader`** — GLSL шейдеры, версия `#version 330 core`.
- **`Texture2D`** — загрузка PNG через `stb_image`, поддержка 1×1 solid color.
- **`SpriteRenderer`** — рисование одного спрайта (quad).
- **`TextRenderer`** — рисование текста через `stb_truetype`.
- **`OrthographicCamera`** — 2D-камера с `ViewProjection` матрицей.
- **`ResourceManager` / `ResourcePool`** — кэш текстур/шейдеров.

## Тонкости

- **glad загружается вручную** — каждая новая GL-функция требует регистрации в `glad/gl.h` + `GraphicsContext::Init`.
- **`SpriteRenderer::SetCustomSize` имеет 2 overloads** — в Lua-биндинге обязательно disambiguate через `glm::vec2(...)`.
- **DefaultSprite shader** использует `u_Color` (для Color tinting).
- **`Transform::SetScale` игнорируется в игровом мире** — для размера используй `SpriteRenderer::SetCustomSize`.

→ [полная документация](../docs/02_Подсистемы/04_Рендерер_и_камеры.md)
