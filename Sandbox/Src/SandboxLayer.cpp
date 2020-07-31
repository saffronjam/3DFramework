#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(const Se::Keyboard &keyboard, const Se::Mouse &mouse)
	:
	Layer("Sandbox"),
	m_Keyboard(keyboard),
	m_Mouse(mouse),
	m_CameraController(16.0f / 9.0f, true)
{
	m_VertexArray = Se::VertexArray::Create();

	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};

	Se::Ref<Se::VertexBuffer> vertexBuffer = Se::VertexBuffer::Create(vertices, sizeof(vertices));
	const Se::BufferLayout layout = {
		{ Se::ShaderDataType::Float3, "a_Position" },
		{ Se::ShaderDataType::Float4, "a_Color" }
	};
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[3] = { 0, 1, 2 };
	const Se::Ref<Se::IndexBuffer> indexBuffer = Se::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_SquareVA = Se::VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	Se::Ref<Se::VertexBuffer> squareVB = Se::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
		{ Se::ShaderDataType::Float3, "a_Position" },
		{ Se::ShaderDataType::Float2, "a_TexCoord" }
						});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	const Se::Ref<Se::IndexBuffer> squareIB = Se::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);

	const std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

	const std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

	m_Shader = Se::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

	const std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

	const std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			
			uniform vec3 u_Color;
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

	m_FlatColorShader = Se::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

}

void SandboxLayer::OnAttach()
{
}

void SandboxLayer::OnDetach()
{
}

void SandboxLayer::OnUpdate(Se::Time ts)
{
	// Update
	m_CameraController.OnUpdate(m_Keyboard, m_Mouse, ts);

	// Render
	Se::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Se::RenderCommand::Clear();

	Se::Renderer::BeginScene(m_CameraController.GetCamera());

	const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	m_FlatColorShader->Bind();
	m_FlatColorShader->SetFloat3("u_Color", m_SquareColor);

	for ( int y = 0; y < 20; y++ )
	{
		for ( int x = 0; x < 20; x++ )
		{
			glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Se::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
		}
	}

	// Triangle
	Se::Renderer::Submit(m_Shader, m_VertexArray);

	Se::Renderer::EndScene();
}

void SandboxLayer::OnImGuiRender()
{
}

void SandboxLayer::OnEvent(const Se::Event &event)
{
	m_CameraController.OnEvent(event);
}
