#pragma once
#include <glad/gl.h>
#include <memory>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <vector>

namespace NK {

	class VertexArray {
	public:
		VertexArray();
		~VertexArray();

		void Bind() const;
		void Unbind() const;

		// Подключить вершинный буфер с описанием атрибутов
		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vb);
		// Установить индексный буфер (необязательно, если рисуем через DrawArrays)
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ib);
		std::shared_ptr<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};

} // namespace NK