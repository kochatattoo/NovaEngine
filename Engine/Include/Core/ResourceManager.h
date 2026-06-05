#pragma once
#include <string>
#include <memory>
#include "Core/ResourcePool.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Shader.h"

namespace NK {

	class ResourceManager {
	public:
		// Получить текстуру по пути
		std::shared_ptr<Texture2D> GetTexture(const std::string& filepath);

		// Получить шейдер по имени и исходникам
		std::shared_ptr<Shader> GetShader(const std::string& name,
			const std::string& vertexSrc,
			const std::string& fragmentSrc);

		std::shared_ptr<Shader> GetShader(const std::string& name);

		ResourcePool<Shader>& GetShaderPool() { return m_Shaders; }

		// Очистить все ресурсы (например, при смене сцены)
		void ClearAll();

		// Можно добавить методы для других типов ресурсов (Sound, Model и т.д.)

	private:
		ResourcePool<Texture2D> m_Textures;
		ResourcePool<Shader>    m_Shaders;
	};

} // namespace NK