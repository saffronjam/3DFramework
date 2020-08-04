#include "Saffron/SaffronPCH.h"

#include "Saffron/Renderer/RenderCommand.h"
#include "Saffron/Renderer/Renderer2D.h"
#include "Saffron/Renderer/VertexArray.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{

struct QuadVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

struct Renderer2DData
{
	static const Uint32 MaxQuads = 20000;
	static const Uint32 MaxVertices = MaxQuads * 4;
	static const Uint32 MaxIndices = MaxQuads * 6;
	static const Uint32 MaxTextureSlots = 32; // TODO: RenderCaps

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<Shader> TextureShader;
	Ref<Texture2D> WhiteTexture;

	Uint32 QuadIndexCount = 0;
	QuadVertex *QuadVertexBufferBase = nullptr;
	QuadVertex *QuadVertexBufferPtr = nullptr;

	std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
	Uint32 TextureSlotIndex = 1; // 0 = white texture

	glm::vec4 QuadVertexPositions[4] = {};

	Renderer2D::Statistics Stats;
};

static Renderer2DData m_sData;

void Renderer2D::Init()
{
	//SE_PROFILE_FUNCTION();

	m_sData.QuadVertexArray = VertexArray::Create();

	m_sData.QuadVertexBuffer = VertexBuffer::Create(m_sData.MaxVertices * sizeof(QuadVertex));
	m_sData.QuadVertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color" },
		{ ShaderDataType::Float2, "a_TexCoord" },
		{ ShaderDataType::Float, "a_TexIndex" },
		{ ShaderDataType::Float, "a_TilingFactor" }
										});
	m_sData.QuadVertexArray->AddVertexBuffer(m_sData.QuadVertexBuffer);

	m_sData.QuadVertexBufferBase = new QuadVertex[m_sData.MaxVertices];

	Uint32 *quadIndices = new Uint32[m_sData.MaxIndices];

	Uint32 offset = 0;
	for ( Uint32 i = 0; i < m_sData.MaxIndices; i += 6 )
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	const Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, m_sData.MaxIndices);
	m_sData.QuadVertexArray->SetIndexBuffer(quadIB);
	delete[] quadIndices;

	m_sData.WhiteTexture = Texture2D::Create(1, 1);
	Uint32 whiteTextureData = 0xffffffff;
	m_sData.WhiteTexture->SetData(&whiteTextureData, sizeof(Uint32));

	int32_t samplers[m_sData.MaxTextureSlots];
	for ( Uint32 i = 0; i < m_sData.MaxTextureSlots; i++ )
		samplers[i] = i;

	m_sData.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
	m_sData.TextureShader->Bind();
	m_sData.TextureShader->SetIntArray("u_Textures", samplers, m_sData.MaxTextureSlots);

	// Set all texture slots to 0
	m_sData.TextureSlots[0] = m_sData.WhiteTexture;

	m_sData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	m_sData.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
	m_sData.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
	m_sData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
}

void Renderer2D::Shutdown()
{
	//SE_PROFILE_FUNCTION();

	delete[] m_sData.QuadVertexBufferBase;
}

void Renderer2D::BeginScene(const Camera &camera)
{
	//SE_PROFILE_FUNCTION();

	const glm::mat4 viewProj = camera.GetViewProjectionMatrix();

	m_sData.TextureShader->Bind();
	m_sData.TextureShader->SetMat4("u_ViewProjection", viewProj);

	m_sData.QuadIndexCount = 0;
	m_sData.QuadVertexBufferPtr = m_sData.QuadVertexBufferBase;

	m_sData.TextureSlotIndex = 1;
}

void Renderer2D::EndScene()
{
	//SE_PROFILE_FUNCTION();

	const Uint32 dataSize = static_cast<Uint32>(reinterpret_cast<uint8_t *>(m_sData.QuadVertexBufferPtr) - reinterpret_cast<uint8_t *>(m_sData.QuadVertexBufferBase));
	m_sData.QuadVertexBuffer->SetData(m_sData.QuadVertexBufferBase, dataSize);

	Flush();
}

void Renderer2D::Flush()
{
	if ( m_sData.QuadIndexCount == 0 )
		return; // Nothing to draw

	// Bind textures
	for ( Uint32 i = 0; i < m_sData.TextureSlotIndex; i++ )
		m_sData.TextureSlots[i]->Bind(i);

	RenderCommand::DrawIndexed(m_sData.QuadVertexArray, m_sData.QuadIndexCount);
	m_sData.Stats.DrawCalls++;
}

void Renderer2D::FlushAndReset()
{
	EndScene();

	m_sData.QuadIndexCount = 0;
	m_sData.QuadVertexBufferPtr = m_sData.QuadVertexBufferBase;

	m_sData.TextureSlotIndex = 1;
}

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
{
	//SE_PROFILE_FUNCTION();

	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, color);
}

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	//SE_PROFILE_FUNCTION();

	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, texture, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color)
{
	//SE_PROFILE_FUNCTION();

	constexpr size_t quadVertexCount = 4;
	const float textureIndex = 0.0f; // White Texture
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
	const float tilingFactor = 1.0f;

	if ( m_sData.QuadIndexCount >= Renderer2DData::MaxIndices )
		FlushAndReset();

	for ( size_t i = 0; i < quadVertexCount; i++ )
	{
		m_sData.QuadVertexBufferPtr->Position = transform * m_sData.QuadVertexPositions[i];
		m_sData.QuadVertexBufferPtr->Color = color;
		m_sData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		m_sData.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_sData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		m_sData.QuadVertexBufferPtr++;
	}

	m_sData.QuadIndexCount += 6;

	m_sData.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	//SE_PROFILE_FUNCTION();

	constexpr size_t quadVertexCount = 4;
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

	if ( m_sData.QuadIndexCount >= Renderer2DData::MaxIndices )
		FlushAndReset();

	float textureIndex = 0.0f;
	for ( Uint32 i = 1; i < m_sData.TextureSlotIndex; i++ )
	{
		if ( *m_sData.TextureSlots[i] == *texture )
		{
			textureIndex = static_cast<float>(i);
			break;
		}
	}

	if ( textureIndex == 0.0f )
	{
		if ( m_sData.TextureSlotIndex >= Renderer2DData::MaxTextureSlots )
			FlushAndReset();

		textureIndex = static_cast<float>(m_sData.TextureSlotIndex);
		m_sData.TextureSlots[m_sData.TextureSlotIndex] = texture;
		m_sData.TextureSlotIndex++;
	}

	for ( size_t i = 0; i < quadVertexCount; i++ )
	{
		m_sData.QuadVertexBufferPtr->Position = transform * m_sData.QuadVertexPositions[i];
		m_sData.QuadVertexBufferPtr->Color = tintColor;
		m_sData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		m_sData.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_sData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		m_sData.QuadVertexBufferPtr++;
	}

	m_sData.QuadIndexCount += 6;

	m_sData.Stats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{
	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{
	//SE_PROFILE_FUNCTION();

	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Ref<Texture2D> &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	//SE_PROFILE_FUNCTION();

	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	DrawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::ResetStats()
{
	memset(&m_sData.Stats, 0, sizeof(Statistics));
}

Renderer2D::Statistics Renderer2D::GetStats()
{
	return m_sData.Stats;
}

}
