#include "Renderer/GraphicsContext.h"
#include <Windows.h>
#include <glad/gl.h>        // типы, константы и объявления glad_* указателей
#include "Core/Log.h"

// Константы WGL
#define WGL_CONTEXT_MAJOR_VERSION_ARB      0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB      0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB       0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB   0x00000001

typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(
    HDC hDC, HGLRC hShareContext, const int* attribList
    );

namespace NK {

    // --------------------------------------------------------------------------
    // Универсальная функция получения адреса OpenGL-функции.
    // Сначала пробует wglGetProcAddress (для "новых" функций), 
    // при неудаче добирает из opengl32.dll (базовые функции 1.1).
    // --------------------------------------------------------------------------
    static void* GetAnyGLFuncAddress(const char* name) {
        void* p = (void*)wglGetProcAddress(name);
        if (p == 0 || p == (void*)0x1 || p == (void*)0x2 || p == (void*)0x3 || p == (void*)-1) {
            HMODULE module = LoadLibraryA("opengl32.dll");
            if (module) {
                p = (void*)GetProcAddress(module, name);
            }
        }
        return p;
    }

    // --------------------------------------------------------------------------
    // Внутренний класс – Windows OpenGL контекст
    // --------------------------------------------------------------------------
    class WindowsOpenGLContext : public GraphicsContext {
    public:
        explicit WindowsOpenGLContext(void* nativeWindow)
            : m_Hwnd(static_cast<HWND>(nativeWindow)) {
        }

        void Init(void* nativeWindow) override {
            m_Hwnd = static_cast<HWND>(nativeWindow);
            m_HDC = GetDC(m_Hwnd);

            // ----- Формат пикселей -----
            PIXELFORMATDESCRIPTOR pfd = {};
            pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 32;
            pfd.cDepthBits = 24;
            pfd.cStencilBits = 8;
            pfd.iLayerType = PFD_MAIN_PLANE;

            int pixelFormat = ChoosePixelFormat(m_HDC, &pfd);
            SetPixelFormat(m_HDC, pixelFormat, &pfd);

            // ----- Временный контекст -----
            HGLRC tempContext = wglCreateContext(m_HDC);
            if (!tempContext) {
                NK_CORE_ERROR("Failed to create temporary OpenGL context");
                return;
            }
            wglMakeCurrent(m_HDC, tempContext);

            auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
                wglGetProcAddress("wglCreateContextAttribsARB");

            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(tempContext);

            if (!wglCreateContextAttribsARB) {
                NK_CORE_ERROR("wglCreateContextAttribsARB not supported");
                return;
            }

            // ----- Постоянный контекст 3.3 Core -----
            const int attribs[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                0
            };
            m_GLRC = wglCreateContextAttribsARB(m_HDC, nullptr, attribs);
            if (!m_GLRC) {
                NK_CORE_ERROR("Failed to create OpenGL 3.3 Core context");
                return;
            }
            wglMakeCurrent(m_HDC, m_GLRC);

            // ----- Ручная загрузка ВСЕХ нужных функций OpenGL -----
            // Заполняем указатели glad_*, чтобы макросы (glEnable и т.д.) работали.
            // Список можно расширять по мере необходимости.

            // Функции, используемые в Renderer
            glad_glEnable = (PFNGLENABLEPROC)GetAnyGLFuncAddress("glEnable");
            glad_glDepthFunc = (PFNGLDEPTHFUNCPROC)GetAnyGLFuncAddress("glDepthFunc");
            glad_glCullFace = (PFNGLCULLFACEPROC)GetAnyGLFuncAddress("glCullFace");
            glad_glFrontFace = (PFNGLFRONTFACEPROC)GetAnyGLFuncAddress("glFrontFace");
            glad_glClearColor = (PFNGLCLEARCOLORPROC)GetAnyGLFuncAddress("glClearColor");
            glad_glClear = (PFNGLCLEARPROC)GetAnyGLFuncAddress("glClear");
            glad_glViewport = (PFNGLVIEWPORTPROC)GetAnyGLFuncAddress("glViewport");

            // Дополнительно: для возможной диагностики
            glad_glGetString = (PFNGLGETSTRINGPROC)GetAnyGLFuncAddress("glGetString");
            glad_glGetError = (PFNGLGETERRORPROC)GetAnyGLFuncAddress("glGetError");

            // Проверяем, что критически важные функции загружены
            if (!glad_glEnable || !glad_glClear || !glad_glClearColor || !glad_glViewport) {
                NK_CORE_ERROR("Failed to load essential OpenGL functions");
                return;
            }

            // Настройка viewport
            RECT rect;
            GetClientRect(m_Hwnd, &rect);
            glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);

            // Выводим версию OpenGL для подтверждения
            const char* version = (const char*)glGetString(GL_VERSION);
            NK_CORE_INFO("OpenGL context created successfully. Version: {0}", version ? version : "unknown");
        }

        void MakeCurrent() override {
            wglMakeCurrent(m_HDC, m_GLRC);
        }

        void SwapBuffers() override {
            ::SwapBuffers(m_HDC);
        }

        void Shutdown() override {
            if (m_GLRC) {
                wglMakeCurrent(nullptr, nullptr);
                wglDeleteContext(m_GLRC);
                m_GLRC = nullptr;
            }
            if (m_HDC) {
                ReleaseDC(m_Hwnd, m_HDC);
                m_HDC = nullptr;
            }
        }

    private:
        HWND m_Hwnd = nullptr;
        HDC m_HDC = nullptr;
        HGLRC m_GLRC = nullptr;
    };

    // Фабрика
    GraphicsContext* GraphicsContext::Create(void* nativeWindow) {
        return new WindowsOpenGLContext(nativeWindow);
    }

} // namespace NK