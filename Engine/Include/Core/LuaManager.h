#pragma once
#include <string>
#include <memory>
#include <sol/sol.hpp>

namespace NK {

	class LuaManager {
	public:
		LuaManager();
		~LuaManager();

		// Загрузить и выполнить скрипт из файла
		bool RunScript(const std::string& filepath);

		// Получить доступ к состоянию Lua (для продвинутых операций)
		sol::state& GetState() { return m_State; }

		// Вызвать глобальную Lua-функцию
		template<typename... Args>
		auto CallFunction(const std::string& funcName, Args&&... args) -> decltype(auto) {
			sol::protected_function func = m_State[funcName];
			if (!func.valid()) {
				// NK_CORE_ERROR("Lua function '{0}' not found", funcName);
				return decltype(func())();
			}
			return func(std::forward<Args>(args)...);
		}

		// Экспортировать C++-класс или функцию в Lua
		template<typename T, typename... Args>
		void RegisterClass(const std::string& name) {
			m_State.new_usertype<T>(name, Args()...);
		}

		// Простейшая регистрация функции
		template<typename Func>
		void RegisterFunction(const std::string& name, Func&& func) {
			m_State.set_function(name, std::forward<Func>(func));
		}

	private:
		sol::state m_State;
	};

} // namespace NK