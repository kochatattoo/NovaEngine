#pragma once
#include "Core/Engine.h"
// Этот заголовок должен включаться **только один раз** в проекте игры,
// чтобы не было множественного определения main.

#ifdef _WIN32  // Пока только Windows

// Объявляем, что Sandbox предоставит свою реализацию CreateApplication
extern NK::Application* NK::CreateApplication();

int main(int argc, char** argv) {
    // Сначала инициализируем движок
    NK::EngineConfig config;
    config.Title = "NovaEngine Sandbox";
    config.Width = 1280;
    config.Height = 720;

    NK::Engine engine(config);      // Создаём ядро движка

    auto app = NK::CreateApplication(); // Создаём игру

    engine.Run(app);                // Запускаем главный цикл

    delete app;                     // Очищаем память игры
    return 0;
}

#endif // NK_PLATFORM_WINDOWS
