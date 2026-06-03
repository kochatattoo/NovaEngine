#pragma once
#include <glad/gl.h>
#include <cstdint>

namespace NK {

	class VertexBuffer {
	public:
		VertexBuffer(const void* data, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetID() const { return m_RendererID; }

	private:
		uint32_t m_RendererID;
	};

} // namespace NK