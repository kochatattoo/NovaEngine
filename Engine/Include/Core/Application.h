#pragma once

namespace NK {

    // Базовый класс, который должен наследовать движок
    // Аналог MonoBehaviour в Unity, только проще.
    class Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        // Вызывается один раз при старте
        virtual void OnStart() {}
        // Вызывается каждый кадр, deltaTime в секундах
        virtual void OnUpdate(float deltaTime) {}
        // Вызывается перед завершением
        virtual void OnShutdown() {}

        // v0.1.1: OnEvent УДАЛЁН. Сейчас события обрабатываются через InputSystem,
        // который доступен как синглтон. Прямой проброс событий в Application
        // запланирован на v0.2 (см. [06_План_разработки/03_Среднесрочный_план.md]).
        // Если нужна обработка ввода — подпишитесь в OnStart и опрашивайте
        // InputSystem::Get().GetKeyDown(...)/GetMouseButtonDown(...) каждый кадр.
    };

    // Функция, которую должен реализовать клиент (игра)
    // Возвращает указатель на свой экземпляр приложения.
    // Аналог того, как Unity создаёт Awake/Start, но создаётся только
    // один экземпляр приложения.
    extern Application* CreateApplication();

} // namespace NK