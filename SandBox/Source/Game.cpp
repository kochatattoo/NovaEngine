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
#include <Renderer/Camera.h>

// Вершинный шейдер (позиция и цвет из вершин)
static const char* s_VertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;   // матрица Проекция * Вид

out vec4 v_Color;
void main() {
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
    v_Color = a_Color;
}
)";

// Фрагментный шейдер (просто выводит цвет)
static const char* s_FragmentShaderSrc = R"(
#version 330 core
in vec4 v_Color;
out vec4 FragColor;
void main() {
    FragColor = v_Color;
}
)";

class SandboxApp : public NK::Application {
public:
    void OnStart() override {
		// Вершины треугольника: позиция (x,y,z) + цвет (r,g,b,a)
		float vertices[] = {
			// позиция (x,y,z)        цвет (r,g,b,a)
		// Передняя грань (z = 0.5)
		-0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f, 1.0f, // красный
		 0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, 1.0f, // зелёный
		 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f, 1.0f, // синий
		-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 1.0f, // жёлтый
		// Задняя грань (z = -0.5)
		-0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f, 1.0f, // фиолетовый
		 0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f, 1.0f, // циан
		 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f, 1.0f, // красный
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f, 1.0f, // зелёный
		};
		// Индексы (необязательно, но для примера)
		uint32_t indices[] = {
			// Передняя грань (0,1,2, 0,2,3)
			0, 1, 2, 2, 3, 0,
			// Задняя грань (4,5,6, 4,6,7) – обход против часовой, если смотреть снаружи
			4, 6, 5, 4, 7, 6,
			// Верхняя грань (3,2,6, 3,6,7)
			3, 2, 6, 3, 6, 7,
			// Нижняя грань (0,4,5, 0,5,1)
			0, 4, 5, 0, 5, 1,
			// Левая грань (0,3,7, 0,7,4)
			0, 3, 7, 0, 7, 4,
			// Правая грань (1,5,6, 1,6,2)
			1, 5, 6, 1, 6, 2
		};

		// Создаём шейдер
		m_Shader = std::make_unique<NK::Shader>(s_VertexShaderSrc, s_FragmentShaderSrc);

		// Создаём VAO и привязываем буферы
		m_VAO = std::make_shared<NK::VertexArray>();
		auto vb = std::make_shared<NK::VertexBuffer>(vertices, sizeof(vertices));
		auto ib = std::make_shared<NK::IndexBuffer>(indices, 36);

		m_VAO->AddVertexBuffer(vb);
		m_VAO->SetIndexBuffer(ib);

		float aspect = (float)1280 / 720; // пока хардкод, можно брать из окна
		m_Camera = std::make_unique<NK::Camera>(45.0f, aspect, 0.1f, 100.0f);

        NK_INFO("Sandbox started! Press SPACE to change color, ESC to exit.");
        NK::Renderer::SetClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    }

    void OnUpdate(float deltaTime) override {

		// Обновим камеру (можно двигать через клавиши, но пока просто вращаем объект)
	// Для вращения объекта нам нужна модельная матрица. Мы можем передать её в шейдер.
	// У нас пока нет u_Model, поэтому добавим uniform u_Model и будем передавать полную MVP.
	// Чтобы не усложнять, в этом примере просто будем вращать саму камеру вокруг начала координат.

		static float angle = 0.0f;
		angle += deltaTime * 0.5f; // вращаемся со скоростью 0.5 рад/с
		float radius = 3.0f;
		glm::vec3 pos = glm::vec3(glm::cos(angle) * radius, 1.0f, glm::sin(angle) * radius);
		m_Camera->SetPosition(pos);
		m_Camera->SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));
		m_Camera->Update();

		// Используем шейдер и передаём матрицу
		m_Shader->Bind();
		m_Shader->SetUniformMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());

		m_VAO->Bind();
		glDrawElements(GL_TRIANGLES, m_VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

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
	std::unique_ptr<NK::Camera> m_Camera;
	float m_ColorR = 1.0f, m_ColorG = 1.0f, m_ColorB = 1.0f;
};

NK::Application* NK::CreateApplication() {
    return new SandboxApp();
}