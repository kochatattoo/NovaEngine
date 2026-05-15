#pragma once

namespace NK {

    class Renderer {
    public:
        static void Init();        // настройка состояний OpenGL
        static void BeginFrame();  // очистка экрана
        static void EndFrame();    // завершение кадра (пока ничего)
        static void SetClearColor(float r, float g, float b, float a);
        static void Shutdown();

    private:
        static float s_ClearColor[4];
    };

} // namespace NK