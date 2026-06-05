#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Core/Log.h"

namespace NK {

	template<typename T>
	class ResourcePool {
	public:
		// Получить ресурс по ключу. Если нет – создать, используя переданные аргументы.
		template<typename... Args>
		std::shared_ptr<T> Get(const std::string& key, Args&&... args) {
			auto it = m_Resources.find(key);
			if (it != m_Resources.end()) {
				NK_CORE_TRACE("ResourcePool: returning cached '%s'", key);
				return it->second;
			}

			NK_CORE_INFO("ResourcePool: loading '%s'", key.c_str());
			auto resource = std::make_shared<T>(std::forward<Args>(args)...);
			m_Resources[key] = resource;
			return resource;
		}

		std::shared_ptr<T> GetCached(const std::string& key) {
			auto it = m_Resources.find(key);
			return (it != m_Resources.end()) ? it->second : nullptr;
		}

		void Put(const std::string& key, std::shared_ptr<T> resource) {
			m_Resources[key] = resource;
		}

		// Удалить из кэша (ресурс освободится, если на него нет других ссылок)
		void Remove(const std::string& key) {
			m_Resources.erase(key);
		}

		// Очистить весь кэш
		void Clear() {
			m_Resources.clear();
			NK_CORE_INFO("ResourcePool: cleared all resources");
		}

		// Проверить, есть ли в кэше
		bool Contains(const std::string& key) const {
			return m_Resources.find(key) != m_Resources.end();
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<T>> m_Resources;
	};

} // namespace NK
