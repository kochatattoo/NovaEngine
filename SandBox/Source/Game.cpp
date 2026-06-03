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
#include <Renderer/Texture2D.h>

// Вершинный шейдер (позиция и цвет из вершин)
static const char* s_VertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 a_Position;  // позиция (2D)
layout(location = 1) in vec2 a_TexCoord;  // UV
uniform mat4 u_ViewProjection;
out vec2 v_TexCoord;
void main() {
    gl_Position = u_ViewProjection * vec4(a_Position, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
}
)";

// Фрагментный шейдер (просто выводит цвет)
static const char* s_FragmentShaderSrc = R"(
#version 330 core
in vec2 v_TexCoord;
out vec4 FragColor;
uniform sampler2D u_Texture;
void main() {
    FragColor = texture(u_Texture, v_TexCoord);
}
)";

class SandboxApp : public NK::Application {
public:
    void OnStart() override {
		// Загружаем и выполняем скрипт
		NK::Engine::Get().GetLuaManager().RunScript("assets/scripts/game.lua");

		// Вызываем Lua-функцию OnStart
		NK::Engine::Get().GetLuaManager().CallFunction("OnStart");

		// Получаем шейдер через ресурс-менеджер
		m_Shader = NK::Engine::Get().GetResourceManager().GetShader("SpriteShader", s_VertexShaderSrc, s_FragmentShaderSrc);
		// Получаем текстуру
		m_Texture = NK::Engine::Get().GetResourceManager().GetTexture("assets/textures/test.png");

		// Прямоугольник (квад) размером 1x1 (будет масштабироваться через матрицу камеры или Model)
		float vertices[] = {
			// позиция (x,y)    UV (u,v)
			-0.5f, -0.5f,       0.0f, 0.0f,  // левый нижний
			 0.5f, -0.5f,       1.0f, 0.0f,  // правый нижний
			 0.5f,  0.5f,       1.0f, 1.0f,  // правый верхний
			-0.5f,  0.5f,       0.0f, 1.0f   // левый верхний
		};
		// Индексы (без них можно glDrawArrays)
		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		// Создаём VAO и VBO
		glGenVertexArrays(1, &m_QuadVAO);
		glBindVertexArray(m_QuadVAO);

		glGenBuffers(1, &m_QuadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Настраиваем атрибуты
		// Позиция (location = 0)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		// UV (location = 1)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		// Индексный буфер (можно без него, но сделаем для единообразия)
		uint32_t ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// VAO запоминает индексный буфер
		glBindVertexArray(0); // отключаем

		// Камера ортографическая (для 2D)
		// Но у нас пока нет ортокамеры, используем перспективу, но с подходящим размером.
		// Позже в Части 12 сделаем OrthoCamera. Пока просто подвинем камеру назад и сделаем объект маленьким.
		m_Camera = std::make_unique<NK::Camera>(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
		m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
		m_Camera->SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));
		m_Camera->Update();
    }

    void OnUpdate(float deltaTime) override {
		NK::Engine::Get().GetLuaManager().CallFunction("OnUpdate", deltaTime);

		NK::Renderer::BeginFrame();

		m_Shader->Bind();
		m_Texture->Bind(0); // текстура на слоте 0
		// Передаём матрицу камеры (пока без Model)
		m_Shader->SetUniformMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
		// Устанавливаем uniform для текстуры (слот 0)
		m_Shader->SetUniform1i("u_Texture", 0);

		glBindVertexArray(m_QuadVAO);
		// Рисуем с индексами или без
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // 6 индексов
		// glDrawArrays(GL_TRIANGLES, 0, 4);

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
	std::shared_ptr<NK::Shader> m_Shader;
	std::shared_ptr<NK::Texture2D> m_Texture;
	std::shared_ptr<NK::VertexArray> m_VAO;
	std::unique_ptr<NK::Camera> m_Camera;
	float m_ColorR = 1.0f, m_ColorG = 1.0f, m_ColorB = 1.0f;

	uint32_t m_QuadVAO;    // VAO для прямоугольника
	uint32_t m_QuadVBO;    // VBO для прямоугольника
};

NK::Application* NK::CreateApplication() {
    return new SandboxApp();
}