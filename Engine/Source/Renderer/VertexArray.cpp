#include "Renderer/VertexArray.h"

namespace NK {

	VertexArray::VertexArray() {
		glGenVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Bind() const {
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const {
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vb) {
		Bind();                // активируем VAO
		vb->Bind();            // привязываем VBO

		// Пока жёстко задаём layout: позиция (3 float), цвет (4 float)
		// Позже можно будет параметризовать.
		// Позиция: location = 0
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
		// Цвет: location = 1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

		m_VertexBuffers.push_back(vb);
	}

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ib) {
		Bind();
		ib->Bind();
		m_IndexBuffer = ib;
	}

} // namespace NK