#include "Core/Log.h"

namespace NK {

    // Определяем статические объекты (как статические поля в C#)
    Logger Log::s_CoreLogger;
    Logger Log::s_ClientLogger;

    void Logger::Trace(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);            // начало работы с переменным числом аргументов
        PrintLog(LogLevel::Trace, fmt, args);
        va_end(args);                   // завершение
    }

    void Logger::Info(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        PrintLog(LogLevel::Info, fmt, args);
        va_end(args);
    }

    void Logger::Warn(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        PrintLog(LogLevel::Warn, fmt, args);
        va_end(args);
    }

    void Logger::Error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        PrintLog(LogLevel::Error, fmt, args);
        va_end(args);
    }

    void Logger::Critical(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        PrintLog(LogLevel::Critical, fmt, args);
        va_end(args);
    }

    void Logger::PrintLog(LogLevel level, const char* fmt, va_list args) {
        // Если уровень сообщения ниже минимального — пропускаем
        if (level < m_MinLevel)
            return;

        // Префикс для наглядности
        const char* prefix = "";
        switch (level) {
        case LogLevel::Trace:    prefix = "[TRACE] "; break;
        case LogLevel::Info:     prefix = "[INFO]  "; break;
        case LogLevel::Warn:     prefix = "[WARN]  "; break;
        case LogLevel::Error:    prefix = "[ERROR] "; break;
        case LogLevel::Critical: prefix = "[CRIT]  "; break;
        }

        printf("%s", prefix);   // выводим префикс
        vprintf(fmt, args);     // выводим форматированное сообщение
        printf("\n");           // перевод строки
    }

} // namespace NK