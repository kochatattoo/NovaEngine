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

			// --- Шейдеры ---
			glad_glCreateShader = (PFNGLCREATESHADERPROC)GetAnyGLFuncAddress("glCreateShader");
			glad_glShaderSource = (PFNGLSHADERSOURCEPROC)GetAnyGLFuncAddress("glShaderSource");
			glad_glCompileShader = (PFNGLCOMPILESHADERPROC)GetAnyGLFuncAddress("glCompileShader");
			glad_glGetShaderiv = (PFNGLGETSHADERIVPROC)GetAnyGLFuncAddress("glGetShaderiv");
			glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GetAnyGLFuncAddress("glGetShaderInfoLog");
			glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC)GetAnyGLFuncAddress("glCreateProgram");
			glad_glAttachShader = (PFNGLATTACHSHADERPROC)GetAnyGLFuncAddress("glAttachShader");
            glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)GetAnyGLFuncAddress("glGetProgramiv");
			glad_glLinkProgram = (PFNGLLINKPROGRAMPROC)GetAnyGLFuncAddress("glLinkProgram");
            glad_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)GetAnyGLFuncAddress("glValidateProgram");
			glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)GetAnyGLFuncAddress("glGetProgramiv");
			glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)GetAnyGLFuncAddress("glGetProgramInfoLog");
			glad_glDeleteShader = (PFNGLDELETESHADERPROC)GetAnyGLFuncAddress("glDeleteShader");
			glad_glUseProgram = (PFNGLUSEPROGRAMPROC)GetAnyGLFuncAddress("glUseProgram");
			glad_glUniform4f = (PFNGLUNIFORM4FPROC)GetAnyGLFuncAddress("glUniform4f");
			glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)GetAnyGLFuncAddress("glGetUniformLocation");
            glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)GetAnyGLFuncAddress("glDeleteProgram");

			// --- VAO/VBO/IBO ---
			glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)GetAnyGLFuncAddress("glGenVertexArrays");
			glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)GetAnyGLFuncAddress("glBindVertexArray");
            glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)GetAnyGLFuncAddress("glDeleteVertexArrays");
			glad_glGenBuffers = (PFNGLGENBUFFERSPROC)GetAnyGLFuncAddress("glGenBuffers");
			glad_glBindBuffer = (PFNGLBINDBUFFERPROC)GetAnyGLFuncAddress("glBindBuffer");
            glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GetAnyGLFuncAddress("glDeleteBuffers");
			glad_glBufferData = (PFNGLBUFFERDATAPROC)GetAnyGLFuncAddress("glBufferData");
			glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GetAnyGLFuncAddress("glVertexAttribPointer");
			glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetAnyGLFuncAddress("glEnableVertexAttribArray");
			glad_glDrawArrays = (PFNGLDRAWARRAYSPROC)GetAnyGLFuncAddress("glDrawArrays");
			glad_glDrawElements = (PFNGLDRAWELEMENTSPROC)GetAnyGLFuncAddress("glDrawElements");

			// Для uniform-переменных (передача цвета)
			glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)GetAnyGLFuncAddress("glGetUniformLocation");
			glad_glUniform4f = (PFNGLUNIFORM4FPROC)GetAnyGLFuncAddress("glUniform4f");
            glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)GetAnyGLFuncAddress("glUniformMatrix4fv");
            glad_glUniform1i = (PFNGLUNIFORM1IPROC)GetAnyGLFuncAddress("glUniform1i");

			// Текстурные функции
			glad_glGenTextures = (PFNGLGENTEXTURESPROC)GetAnyGLFuncAddress("glGenTextures");
			glad_glBindTexture = (PFNGLBINDTEXTUREPROC)GetAnyGLFuncAddress("glBindTexture");
			glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC)GetAnyGLFuncAddress("glTexImage2D");
			glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC)GetAnyGLFuncAddress("glTexParameteri");
			glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)GetAnyGLFuncAddress("glGenerateMipmap");
			glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC)GetAnyGLFuncAddress("glActiveTexture");
            glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC)GetAnyGLFuncAddress("glDeleteTextures");

			// Проверка ВСЕХ загруженных функций (добавь к уже существующим проверкам)
			if (!glad_glCreateShader) { NK_CORE_ERROR("glCreateShader not loaded"); return; }
			if (!glad_glShaderSource) { NK_CORE_ERROR("glShaderSource not loaded"); return; }
			if (!glad_glCompileShader) { NK_CORE_ERROR("glCompileShader not loaded"); return; }
			if (!glad_glGetShaderiv) { NK_CORE_ERROR("glGetShaderiv not loaded"); return; }
			if (!glad_glGetShaderInfoLog) { NK_CORE_ERROR("glGetShaderInfoLog not loaded"); return; }
			if (!glad_glCreateProgram) { NK_CORE_ERROR("glCreateProgram not loaded"); return; }
			if (!glad_glAttachShader) { NK_CORE_ERROR("glAttachShader not loaded"); return; }
			if (!glad_glLinkProgram) { NK_CORE_ERROR("glLinkProgram not loaded"); return; }
            if (!glad_glValidateProgram) { NK_CORE_ERROR("glValidateProgram not loaded"); return; }
			if (!glad_glGetProgramiv) { NK_CORE_ERROR("glGetProgramiv not loaded"); return; }
			if (!glad_glGetProgramInfoLog) { NK_CORE_ERROR("glGetProgramInfoLog not loaded"); return; }
			if (!glad_glDeleteShader) { NK_CORE_ERROR("glDeleteShader not loaded"); return; }
			if (!glad_glUseProgram) { NK_CORE_ERROR("glUseProgram not loaded"); return; }
			if (!glad_glUniform4f) { NK_CORE_ERROR("glUniform4f not loaded"); return; }
			if (!glad_glGetUniformLocation) { NK_CORE_ERROR("glGetUniformLocation not loaded"); return; }
			if (!glad_glGenVertexArrays) { NK_CORE_ERROR("glGenVertexArrays not loaded"); return; }
			if (!glad_glBindVertexArray) { NK_CORE_ERROR("glBindVertexArray not loaded"); return; }
			if (!glad_glGenBuffers) { NK_CORE_ERROR("glGenBuffers not loaded"); return; }
			if (!glad_glBindBuffer) { NK_CORE_ERROR("glBindBuffer not loaded"); return; }
			if (!glad_glBufferData) { NK_CORE_ERROR("glBufferData not loaded"); return; }
			if (!glad_glVertexAttribPointer) { NK_CORE_ERROR("glVertexAttribPointer not loaded"); return; }
			if (!glad_glEnableVertexAttribArray) { NK_CORE_ERROR("glEnableVertexAttribArray not loaded"); return; }
			if (!glad_glDrawElements) { NK_CORE_ERROR("glDrawElements not loaded"); return; }
			// glDrawArrays используем редко, но тоже можно проверить
			if (!glad_glDrawArrays) { NK_CORE_ERROR("glDrawArrays not loaded"); return; }
			if (!glad_glDeleteVertexArrays) { NK_CORE_ERROR("glDeleteVertexArrays not loaded"); return; }
			if (!glad_glDeleteBuffers) { NK_CORE_ERROR("glDeleteBuffers not loaded"); return; }
            if (!glad_glGetProgramiv) { NK_CORE_ERROR("glGetProgramiv not loaded"); return; }
            if (!glad_glDeleteProgram) { NK_CORE_ERROR("glDeleteProgram not loaded"); return; }

			// Проверка, что все шейдерные функции загружены
			if (!glad_glCreateShader || !glad_glShaderSource || !glad_glCompileShader ||
				!glad_glCreateProgram || !glad_glLinkProgram || !glad_glUseProgram ||
				!glad_glGenVertexArrays || !glad_glBindVertexArray || !glad_glGenBuffers ||
				!glad_glBufferData || !glad_glVertexAttribPointer || !glad_glDrawArrays) {
				NK_CORE_ERROR("Failed to load essential shader/buffer functions");
				return;
			}

            if (!glad_glUniformMatrix4fv) { NK_CORE_ERROR("glUniformMatrix4fv not loaded"); return; }
            if (!glad_glUniform1i) { NK_CORE_ERROR("glUniform1i not loaded"); return; }

            if (!glad_glGenTextures || !glad_glBindTexture || !glad_glTexImage2D ||
                !glad_glTexParameteri || !glad_glGenerateMipmap || !glad_glActiveTexture) {
                NK_CORE_ERROR("Texture functions not loaded");
                return;
            }

            if (!glad_glDeleteTextures) { NK_CORE_ERROR("glDeleteTextures not loaded"); return; }

            // Настройка viewport
            RECT rect;
            GetClientRect(m_Hwnd, &rect);
            glViewport(0, 0, rect.right - rect.left, rect.bottom - rect.top);

            // Выводим версию OpenGL для подтверждения
            const char* version = (const char*)glGetString(GL_VERSION);
            NK_CORE_INFO("OpenGL context created successfully. Version: %s", version ? version : "unknown");
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