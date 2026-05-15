#pragma once

namespace NK {

    // Абстрактный интерфейс графического контекста
    class GraphicsContext {
    public:
        virtual ~GraphicsContext() = default;

        // Инициализация с привязкой к окну (HWND)
        virtual void Init(void* nativeWindow) = 0;
        // Сделать контекст текущим в потоке
        virtual void MakeCurrent() = 0;
        // Показать нарисованный кадр (SwapBuffers)
        virtual void SwapBuffers() = 0;
        // Освободить ресурсы
        virtual void Shutdown() = 0;

        // Фабрика для создания платформенного контекста (пока Windows)
        static GraphicsContext* Create(void* nativeWindow);
    };

} // namespace NK