#pragma once
#include <cstdio>    // printf, vprintf
#include <cstdarg>   // va_list, va_start, va_end

// Префикс NK_ для избежания конфликтов имён
namespace NK {

    // Уровни важности сообщений
    enum class LogLevel {
        Trace,      // максимально подробные, для отладки
        Info,       // обычная информация
        Warn,       // предупреждения
        Error,      // ошибки, но программа продолжает работу
        Critical    // критические ошибки, обычно перед крашем
    };

    // Класс, который отвечает за вывод одного "логгера" (движка или игры)
    class Logger {
    public:
        // Установить минимальный уровень: сообщения ниже не будут печататься
        void SetLevel(LogLevel level) { m_MinLevel = level; }

        // Методы для каждого уровня. Используем синтаксис "...", как params в C#
        void Trace(const char* fmt, ...);
        void Info(const char* fmt, ...);
        void Warn(const char* fmt, ...);
        void Error(const char* fmt, ...);
        void Critical(const char* fmt, ...);

    private:
        LogLevel m_MinLevel = LogLevel::Trace; // по умолчанию выводим всё

        // Вспомогательная функция, принимает va_list (аналог перегрузки с params object[] в C#)
        void PrintLog(LogLevel level, const char* fmt, va_list args);
    };

    // Статический класс-держатель двух логгеров: для ядра движка и для игры
    class Log {
    public:
        static Logger& GetCoreLogger() { return s_CoreLogger; }
        static Logger& GetClientLogger() { return s_ClientLogger; }

    private:
        static Logger s_CoreLogger;
        static Logger s_ClientLogger;
    };

} // namespace NK

// Макросы, чтобы не писать длинные вызовы каждый раз
#define NK_CORE_TRACE(...)    ::NK::Log::GetCoreLogger().Trace(__VA_ARGS__)
#define NK_CORE_INFO(...)     ::NK::Log::GetCoreLogger().Info(__VA_ARGS__)
#define NK_CORE_WARN(...)     ::NK::Log::GetCoreLogger().Warn(__VA_ARGS__)
#define NK_CORE_ERROR(...)    ::NK::Log::GetCoreLogger().Error(__VA_ARGS__)
#define NK_CORE_CRITICAL(...) ::NK::Log::GetCoreLogger().Critical(__VA_ARGS__)

#define NK_TRACE(...)         ::NK::Log::GetClientLogger().Trace(__VA_ARGS__)
#define NK_INFO(...)          ::NK::Log::GetClientLogger().Info(__VA_ARGS__)
#define NK_WARN(...)          ::NK::Log::GetClientLogger().Warn(__VA_ARGS__)
#define NK_ERROR(...)         ::NK::Log::GetClientLogger().Error(__VA_ARGS__)
#define NK_CRITICAL(...)      ::NK::Log::GetClientLogger().Critical(__VA_ARGS__)