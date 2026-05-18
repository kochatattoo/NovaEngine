#include <Core/EntryPoint.h>
#include <Core/Log.h>
#include <Input/Input.h>        // старый ввод
#include <Renderer/Renderer.h>
#include <Event/Event.h>
#include <Renderer/Shader.h>
#include <Renderer/VertexArray.h>
#include <Renderer/VertexBuffer.h>
#include <Renderer/IndexBuffer.h>
#include <memory>

// Вершинный шейдер (позиция и цвет из вершин)
static const char* s_VertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 a_Position;  // позиция
layout(location = 1) in vec4 a_Color;     // цвет
out vec4 v_Color;                         // передаём во фрагментный шейдер
void main() {
    gl_Position = vec4(a_Position, 1.0);  // без матрицы, координаты в NDC
    v_Color = a_Color;
}
)";

// Фрагментный шейдер (просто выводит цвет)
static const char* s_FragmentShaderSrc = R"(
#version 330 core
in vec4 v_Color;
out vec4 FragColor;
uniform vec4 u_Color;   // дополнительный цвет для модификации
void main() {
    FragColor = v_Color * u_Color;
}
)";

class SandboxApp : public NK::Application {
public:
    void OnStart() override {
		// Вершины треугольника: позиция (x,y,z) + цвет (r,g,b,a)
		float vertices[] = {
			// позиция               цвет
			-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,  // красный
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,  // зелёный
			 0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f   // синий
		};
		// Индексы (необязательно, но для примера)
		uint32_t indices[] = { 0, 1, 2 };

		// Создаём шейдер
		m_Shader = std::make_unique<NK::Shader>(s_VertexShaderSrc, s_FragmentShaderSrc);

		// Создаём VAO и привязываем буферы
		m_VAO = std::make_shared<NK::VertexArray>();
		auto vb = std::make_shared<NK::VertexBuffer>(vertices, sizeof(vertices));
		auto ib = std::make_shared<NK::IndexBuffer>(indices, 3);

		m_VAO->AddVertexBuffer(vb);
		m_VAO->SetIndexBuffer(ib);

        NK_INFO("Sandbox started! Press SPACE to change color, ESC to exit.");
        NK::Renderer::SetClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    }

    void OnUpdate(float deltaTime) override {

		// Начинаем кадр (очистка)
		NK::Renderer::BeginFrame();

		// Активируем шейдер и устанавливаем uniform-цвет
		m_Shader->Bind();
		// По умолчанию u_Color = белый, чтобы цвета вершин не искажались
		m_Shader->SetUniform4f("u_Color", m_ColorR, m_ColorG, m_ColorB, 1.0f);

		// Рисуем треугольник
		m_VAO->Bind();
		glDrawElements(GL_TRIANGLES, m_VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

		// Завершаем кадр (пока пусто)
		NK::Renderer::EndFrame();
    }

    void OnEvent(NK::Event& e) override {
        NK::EventDispatcher dispatcher(e);

        // Обработка нажатий клавиш (аналог Input.GetKeyDown)
        dispatcher.Dispatch<NK::KeyPressedEvent>([this](NK::KeyPressedEvent& keyEvent) {
            if (keyEvent.KeyCode == VK_ESCAPE) {
                NK::Engine::Get().Shutdown();
                return true;   // событие обработано
            }
            if (keyEvent.KeyCode == VK_SPACE) {
				m_ColorR = (rand() % 1000) / 1000.0f;
				m_ColorG = (rand() % 1000) / 1000.0f;
				m_ColorB = (rand() % 1000) / 1000.0f;
                return true;
            }
            return false;   // не обработано
            });
    }

    void OnShutdown() override {
        NK_INFO("Sandbox shutting down.");
    }

private:
	std::unique_ptr<NK::Shader> m_Shader;
	std::shared_ptr<NK::VertexArray> m_VAO;
	float m_ColorR = 1.0f, m_ColorG = 1.0f, m_ColorB = 1.0f;
};

NK::Application* NK::CreateApplication() {
    return new SandboxApp();
}