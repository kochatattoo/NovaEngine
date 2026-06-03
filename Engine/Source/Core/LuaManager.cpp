#include "Core/LuaManager.h"
#include "Core/Log.h"
#include <cstdio>
#include <sol/sol.hpp>

namespace NK {

	LuaManager::LuaManager() {
		// —оздаЄм состо€ние Lua с открытыми стандартными библиотеками
		m_State.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table);
		// NK_CORE_INFO("LuaManager initialized");
		printf("LuaManager initialized\n");
	}

	LuaManager::~LuaManager() {
		// sol::state автоматически закрывает Lua при разрушении
		// NK_CORE_INFO("LuaManager destroyed");
		printf("LuaManager destroyed\n");
	}

	bool LuaManager::RunScript(const std::string& filepath) {
		try {
			sol::protected_function_result result = m_State.script_file(filepath);
			if (!result.valid()) {
				sol::error err = result;
				NK_CORE_ERROR("Lua script error: {0}", err.what());
				return false;
			}
			return true;
		}
		catch (const std::exception& e) {
			NK_CORE_ERROR("Lua exception: {0}", e.what());
			return false;
		}
	}

} // namespace NK