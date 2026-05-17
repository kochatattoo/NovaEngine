#pragma once

namespace NK {

    // Предварительное объявление, чтобы компилятор знал, что Event — это класс
    class Event;

    // Абстрактный класс, который должна реализовать игра
    // Аналог MonoBehaviour в Unity, только глобальный
    class Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        // Вызывается один раз после инициализации движка
        virtual void OnStart() {}
        // Вызывается каждый кадр, deltaTime в секундах
        virtual void OnUpdate(float deltaTime) {}
        // Вызывается перед завершением
        virtual void OnShutdown() {}
        virtual void OnEvent(Event& event) {}
    };

    // Функция, которую обязан определить клиент (игра)
    // Возвращает указатель на свой экземпляр приложения.
    // Подобно тому, как Unity вызывает Awake/Start, мы вызовем эту функцию
    // для создания приложения.
    extern Application* CreateApplication();

} // namespace NK