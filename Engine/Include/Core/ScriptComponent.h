#pragma once
#include "Core/Component.h"
#include <string>

namespace NK {

	class ScriptComponent : public Component {
	public:
		ScriptComponent(GameObject* owner, const std::string& scriptPath);

		void OnStart() override;
		void OnUpdate(float deltaTime) override;

	private:
		std::string m_ScriptPath;
		bool m_Loaded = false;
	};

} // namespace NK