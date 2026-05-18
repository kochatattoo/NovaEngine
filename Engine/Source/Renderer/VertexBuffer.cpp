#include "Renderer/VertexBuffer.h"

namespace NK {

	VertexBuffer::VertexBuffer(const void* data, uint32_t size) {
		// Генерируем один буфер
		glGenBuffers(1, &m_RendererID);
		// Привязываем как массив вершин
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		// Загружаем данные (GL_STATIC_DRAW – данные не будут часто меняться)
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	void VertexBuffer::Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void VertexBuffer::Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

} // namespace NK