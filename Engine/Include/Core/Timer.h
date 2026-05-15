#pragma once
#include <chrono>

namespace NK {

    class Timer {
    public:
        Timer() {
            m_LastTime = std::chrono::high_resolution_clock::now();
        }

        // Возвращает время в секундах, прошедшее с предыдущего вызова Tick
        float Tick() {
            auto now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> duration = now - m_LastTime;
            m_LastTime = now;
            return duration.count();
        }

    private:
        std::chrono::steady_clock::time_point m_LastTime;
    };

} // namespace NK
