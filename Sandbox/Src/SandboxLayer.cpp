#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(const Se::Keyboard &keyboard, const Se::Mouse &mouse)
	:
	Layer("Sandbox"),
	m_Keyboard(keyboard),
	m_Mouse(mouse),
	m_CameraController2D(16.0f / 9.0f, true),
	m_CameraController3D(16.0f / 9.0f)
{
	m_SquareVA = Se::VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	Se::Ref<Se::VertexBuffer> squareVB = Se::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({ { Se::ShaderDataType::Float3, "a_Position" },
						  { Se::ShaderDataType::Float2, "a_TexCoord" } });
	m_SquareVA->AddVertexBuffer(squareVB);

	Se::Uint32 squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	const Se::Ref<Se::IndexBuffer> squareIB = Se::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(Se::Uint32));
	m_SquareVA->SetIndexBuffer(squareIB);

	m_ShaderLibrary.Load("Assets/Shaders/TextureBasic.glsl");
	m_Texture = Se::Texture2D::Create("Assets/Textures/sample_image.png");
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
	if ( m_Keyboard.IsPressed(SE_KEY_2) )
		m_PerspectiveOn = false;
	else if ( m_Keyboard.IsPressed(SE_KEY_3) )
		m_PerspectiveOn = true;

	if ( m_PerspectiveOn )
		m_CameraController3D.OnUpdate(m_Keyboard, m_Mouse, ts);
	else
		m_CameraController2D.OnUpdate(m_Keyboard, m_Mouse, ts);

	// Render
	Se::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Se::RenderCommand::Clear();

	if ( m_PerspectiveOn )
		Se::Renderer::BeginScene(m_CameraController3D.GetCamera());
	else
		Se::Renderer::BeginScene(m_CameraController2D.GetCamera());

	const auto textureShader = m_ShaderLibrary.Get("TextureBasic");

	m_Texture->Bind(0);
	Se::Renderer::Submit(textureShader, m_SquareVA);

	Se::Renderer::EndScene();
}

void SandboxLayer::OnImGuiRender()
{
}

void SandboxLayer::OnEvent(const Se::Event &event)
{
	if ( m_PerspectiveOn )
		m_CameraController3D.OnEvent(event);
	else
		m_CameraController2D.OnEvent(event);
}
