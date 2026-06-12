#include "Scene/ScriptComponent.h"
#include "Scene/GameObject.h"
#include "Lua/LuaManager.h"
#include "Core/Engine.h"

namespace NK {

	ScriptComponent::ScriptComponent(GameObject* owner, const std::string& scriptPath)
		: Component(owner), m_ScriptPath(scriptPath) {
	}

	void ScriptComponent::OnStart() {
		auto& lua = Engine::Get().GetLuaManager();
		// Загружаем скрипт
		if (!lua.RunScript(m_ScriptPath)) {
			NK_CORE_ERROR("ScriptComponent: failed to run script '%s'", m_ScriptPath.c_str());
			return;
		}
		m_Loaded = true;

		// Вызываем OnStart в скрипте, передавая ссылку на GameObject
		lua.GetState()["gameObject"] = m_Owner; // передадим текущий объект
		lua.CallFunction("OnStart");
	}

	void ScriptComponent::OnUpdate(float deltaTime) {
		if (!m_Loaded) return;
		auto& lua = Engine::Get().GetLuaManager();
		lua.GetState()["gameObject"] = m_Owner;
		lua.CallFunction("OnUpdate", deltaTime);
	}

} // namespace NK