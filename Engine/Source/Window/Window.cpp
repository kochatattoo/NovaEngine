#include "Window/Window.h"
#include "Input/InputSystem.h"

namespace NK {

    Window::Window(const WindowProperties& props)
        : m_Data{ props.Title, props.Width, props.Height } {
        Init();
    }

    Window::~Window() {
        Shutdown();
    }

    void Window::Init() {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        const wchar_t CLASS_NAME[] = L"NK_WINDOW_CLASS";

        // Регистрируем класс окна
        WNDCLASS wc = {};
        wc.lpfnWndProc = WndProc;          // Указываем нашу функцию обработки
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.style = CS_HREDRAW | CS_VREDRAW; // Перерисовывать при изменении размера
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // Стандартный курсор
        RegisterClass(&wc);

        // Преобразуем заголовок из UTF-8 в UTF-16 (Windows использует wide strings)
        int len = MultiByteToWideChar(CP_UTF8, 0, m_Data.Title.c_str(), -1, nullptr, 0);
        std::wstring wTitle(len, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, m_Data.Title.c_str(), -1, &wTitle[0], len);

        // Создаём окно
        m_Hwnd = CreateWindowEx(
            0,                              // Расширенный стиль
            CLASS_NAME,                     // Имя класса
            wTitle.c_str(),                 // Заголовок
            WS_OVERLAPPEDWINDOW,            // Обычное окно с рамкой и кнопками
            CW_USEDEFAULT, CW_USEDEFAULT,   // Позиция по умолчанию
            m_Data.Width, m_Data.Height,
            nullptr, nullptr,
            hInstance,
            this                            // Передаём указатель на объект Window
        );

        // Показываем окно
        ShowWindow(m_Hwnd, SW_SHOW);

        // Создаём и инициализируем графический контекст
        m_GraphicsContext.reset(GraphicsContext::Create(m_Hwnd));
        m_GraphicsContext->Init(m_Hwnd);

        NK_CORE_INFO("Window created:  %dx%d", m_Data.Width, m_Data.Height);
    }

    void Window::Shutdown() {
        if (m_Hwnd) {

            if (m_GraphicsContext)
                m_GraphicsContext->Shutdown();

            DestroyWindow(m_Hwnd);
            m_Hwnd = nullptr;
        }
    }

	void Window::GetMouseClientPosition(int& outX, int& outY) {
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_Hwnd, &pt);
		outX = pt.x;
		outY = pt.y;
	}

    void Window::OnUpdate() {
        MSG msg;
        // Обрабатываем все накопившиеся сообщения
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Статическая функция обратного вызова
    LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        Window* window = nullptr;
        // При создании окна (WM_NCCREATE) извлекаем переданный this
        if (msg == WM_NCCREATE) {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            window = static_cast<Window*>(cs->lpCreateParams);
            // Сохраняем указатель в пользовательских данных окна
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        }
        else {
            // Для остальных сообщений получаем его из GWLP_USERDATA
            window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        // Если указатель есть, обрабатываем
        if (window) {
            switch (msg)
            {
                case WM_CLOSE:
                  window->m_ShouldClose = true;
                    NK_CORE_INFO("Window close requested");
                    return 0; // Сообщение обработано

                case WM_SIZE: {
                    uint32_t width = LOWORD(lParam);
                    uint32_t height = HIWORD(lParam);
                    window->m_Data.Width = width;
                    window->m_Data.Height = height;
                    NK_CORE_TRACE("Window resized to  %dx%d", width, height);

					if (window->ResizeCallback)
						window->ResizeCallback(width, height);

                    return 0;
                }

                case WM_KEYDOWN: {
                    int keyCode = static_cast<int>(wParam);
                    bool repeat = (lParam & 0x40000000) != 0;
                    InputSystem::Get().PushEvent(std::make_unique<KeyPressedEvent>(keyCode, repeat));
                    return 0;
                }
                case WM_KEYUP: {
                    int keyCode = static_cast<int>(wParam);
                    InputSystem::Get().PushEvent(std::make_unique<KeyReleasedEvent>(keyCode));
                    return 0;
                }
                case WM_MOUSEMOVE: {
                    float x = static_cast<float>(LOWORD(lParam));
                    float y = static_cast<float>(HIWORD(lParam));
                    InputSystem::Get().PushEvent(std::make_unique<MouseMovedEvent>(x, y));
                    return 0;
                }
                case WM_LBUTTONDOWN:
                    InputSystem::Get().PushEvent(std::make_unique<MouseButtonPressedEvent>(MouseButton::Left));
                    return 0;
                case WM_LBUTTONUP:
                    InputSystem::Get().PushEvent(std::make_unique<MouseButtonReleasedEvent>(MouseButton::Left));
                    return 0;
                case WM_RBUTTONDOWN:
                    InputSystem::Get().PushEvent(std::make_unique<MouseButtonPressedEvent>(MouseButton::Right));
                    return 0;
                case WM_RBUTTONUP:
                    InputSystem::Get().PushEvent(std::make_unique<MouseButtonReleasedEvent>(MouseButton::Right));
                    return 0;
                case WM_MBUTTONDOWN:
                    InputSystem::Get().PushEvent(std::make_unique<MouseButtonPressedEvent>(MouseButton::Middle));
                    return 0;
                case WM_MBUTTONUP:
                    InputSystem::Get().PushEvent(std::make_unique<MouseButtonReleasedEvent>(MouseButton::Middle));
                    return 0;
                case WM_MOUSEWHEEL: {
                    float delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / (float)WHEEL_DELTA;
                    InputSystem::Get().PushEvent(std::make_unique<MouseScrolledEvent>(0.0f, delta));
                    return 0;
                }
            }
        }
        // Для всех остальных сообщений вызываем стандартную обработку
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

} // namespace NK