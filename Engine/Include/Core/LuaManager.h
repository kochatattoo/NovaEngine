#pragma once
#include <string>
#include <memory>
#include <sol/sol.hpp>
#include "Core/Log.h"

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
				NK_CORE_ERROR("Lua function '%s' not found", funcName.c_str());
				return decltype(func())();
			}
			sol::protected_function_result result = func(std::forward<Args>(args)...);
			if (!result.valid()) {
				sol::error err = result;
				NK_CORE_ERROR("Error in Lua function '%s': %s", funcName.c_str(), err.what());
			}
			return result;
		}

		// Экспортировать C++-класс или функцию в Lua
		template<typename T, typename... Args>
		void BindClass(const std::string& name, Args&&... args) {
			// std::forward правильно прокинет все пары "Имя", &Метод внутрь sol2
			m_State.new_usertype<T>(name, std::forward<Args>(args)...);
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