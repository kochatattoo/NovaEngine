#include "Resource/ResourceManager.h"

namespace NK {

	std::shared_ptr<Texture2D> ResourceManager::GetTexture(const std::string& filepath) {
		return m_Textures.Get(filepath, filepath); // ключ = путь, аргумент конструктора = путь
	}

	std::shared_ptr<Shader> ResourceManager::GetShader(const std::string& name,
		const std::string& vertexSrc,
		const std::string& fragmentSrc) {
		std::string key = name + "_" + std::to_string(std::hash<std::string>{}(vertexSrc + fragmentSrc));
		return m_Shaders.Get(key, vertexSrc, fragmentSrc);
	}

	std::shared_ptr<Shader> ResourceManager::GetShader(const std::string& name) {
		return m_Shaders.GetCached(name);
	}

	void ResourceManager::ClearAll() {
		m_Textures.Clear();
		m_Shaders.Clear();
	}

} // namespace NKс