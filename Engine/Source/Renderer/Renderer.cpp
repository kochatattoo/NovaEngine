#include "Renderer/Renderer.h"
#include <glad/gl.h>

namespace NK {

    float Renderer::s_ClearColor[4] = { 0.1f, 0.1f, 0.15f, 1.0f };

    void Renderer::Init() {
        // ¬ключаем стандартные состо€ни€
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    void Renderer::BeginFrame() {
        glClearColor(s_ClearColor[0], s_ClearColor[1], s_ClearColor[2], s_ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::EndFrame() {
        // в будущем сюда можно вставить сброс состо€ний
    }

    void Renderer::SetClearColor(float r, float g, float b, float a) {
        s_ClearColor[0] = r;
        s_ClearColor[1] = g;
        s_ClearColor[2] = b;
        s_ClearColor[3] = a;
    }

    void Renderer::Shutdown() {
        // освобождение ресурсов, если будут
    }

} // namespace NK