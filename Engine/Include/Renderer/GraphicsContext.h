#pragma once

namespace NK {

    // јбстрактный интерфейс графического контекста
    class GraphicsContext {
    public:
        virtual ~GraphicsContext() = default;

        virtual void Init(void* nativeWindow) = 0;   // инициализаци€, прив€зка к окну
        virtual void MakeCurrent() = 0;              // сделать контекст активным
        virtual void SwapBuffers() = 0;              // показать нарисованный кадр
        virtual void Shutdown() = 0;                 // освободить ресурсы

        // ‘абрика Ц создаЄт платформенно-зависимый контекст (сейчас Windows)
        static GraphicsContext* Create(void* nativeWindow);
    };

} // namespace NK