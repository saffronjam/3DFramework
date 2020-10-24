#include "SaffronPCH.h"

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Renderer/IndexBuffer.h"
#include "Saffron/Renderer/Pipeline.h"
#include "Saffron/Renderer/Renderer.h"
#include "Saffron/Renderer/Renderer2D.h"
#include "Saffron/Renderer/Shader.h"

namespace Se
{

///////////////////////////////////////////////////////////////////////////
/// Helper structs
///////////////////////////////////////////////////////////////////////////

struct QuadVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

struct LineVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
};

struct Renderer2DData
{
	static const Uint32 MaxQuads = 20000;
	static const Uint32 MaxVertices = MaxQuads * 4;
	static const Uint32 MaxIndices = MaxQuads * 6;
	static const Uint32 MaxTextureSlots = 32; // TODO: RenderCaps

	static const Uint32 MaxLines = 10000;
	static const Uint32 MaxLineVertices = MaxLines * 2;
	static const Uint32 MaxLineIndices = MaxLines * 6;

	Shared<Pipeline> QuadPipeline;
	Shared<VertexBuffer> QuadVertexBuffer;
	Shared<IndexBuffer> QuadIndexBuffer;

	Shared<Shader> TextureShader;
	Shared<Texture2D> WhiteTexture;

	Uint32 QuadIndexCount = 0;
	QuadVertex *QuadVertexBufferBase = nullptr;
	QuadVertex *QuadVertexBufferPtr = nullptr;

	std::array<Shared<Texture2D>, MaxTextureSlots> TextureSlots;
	Uint32 TextureSlotIndex = 1; // 0 = white texture

	glm::vec4 QuadVertexPositions[4]{};

	// Lines
	Shared<Pipeline> LinePipeline;
	Shared<VertexBuffer> LineVertexBuffer;
	Shared<IndexBuffer> LineIndexBuffer;

	Shared<Shader> LineShader;

	Uint32 LineIndexCount = 0;
	LineVertex *LineVertexBufferBase = nullptr;
	LineVertex *LineVertexBufferPtr = nullptr;

	glm::mat4 CameraViewProj{};
	bool DepthTest = true;

	Renderer2D::Statistics Stats;
};

static Renderer2DData s_Data;


///////////////////////////////////////////////////////////////////////////
/// Renderer2D
///////////////////////////////////////////////////////////////////////////

void Renderer2D::Init()
{
	{
		Pipeline::Specification Specification;
		Specification.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" }
		};
		s_Data.QuadPipeline = Pipeline::Create(Specification);

		s_Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

		auto *quadIndices = new Uint32[Renderer2DData::MaxIndices];

		Uint32 offset = 0;
		for ( Uint32 i = 0; i < Renderer2DData::MaxIndices; i += 6 )
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		s_Data.QuadIndexBuffer = IndexBuffer::Create(quadIndices, Renderer2DData::MaxIndices);
		delete[] quadIndices;
	}

	s_Data.WhiteTexture = Texture2D::Create(Texture::Format::RGBA, 1, 1);
	Uint32 whiteTextureData = 0xffffffff;
	s_Data.WhiteTexture->Lock();
	s_Data.WhiteTexture->GetWriteableBuffer().Write(&whiteTextureData, sizeof(Uint32));
	s_Data.WhiteTexture->Unlock();

	s_Data.TextureShader = Shader::Create("Assets/Shaders/Renderer2D.glsl");

	// Set all texture slots to 0
	s_Data.TextureSlots[0] = s_Data.WhiteTexture;

	s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
	s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
	s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

	// Lines
	{
		s_Data.LineShader = Shader::Create("Assets/Shaders/Renderer2D_Line.glsl");

		Pipeline::Specification Specification;
		Specification.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		s_Data.LinePipeline = Pipeline::Create(Specification);

		s_Data.LineVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxLineVertices * sizeof(LineVertex));
		s_Data.LineVertexBufferBase = new LineVertex[Renderer2DData::MaxLineVertices];

		auto *lineIndices = new Uint32[Renderer2DData::MaxLineIndices];
		for ( Uint32 i = 0; i < Renderer2DData::MaxLineIndices; i++ )
			lineIndices[i] = i;

		s_Data.LineIndexBuffer = IndexBuffer::Create(lineIndices, Renderer2DData::MaxLineIndices);
		delete[] lineIndices;
	}
}

void Renderer2D::Shutdown()
{
}

void Renderer2D::BeginScene(const glm::mat4 &viewProj, bool depthTest)
{
	s_Data.CameraViewProj = viewProj;
	s_Data.DepthTest = depthTest;

	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	s_Data.LineIndexCount = 0;
	s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

	s_Data.TextureSlotIndex = 1;
}

void Renderer2D::EndScene()
{
	auto dataSize = static_cast<Uint32>(s_Data.QuadVertexBufferPtr - s_Data.QuadVertexBufferBase);
	if ( dataSize )
	{
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
		s_Data.QuadVertexBuffer->Bind();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", s_Data.CameraViewProj);

		for ( Uint32 i = 0; i < s_Data.TextureSlotIndex; i++ )
			s_Data.TextureSlots[i]->Bind(i);

		s_Data.QuadPipeline->Bind();
		s_Data.QuadIndexBuffer->Bind();
		Renderer::DrawIndexed(s_Data.QuadIndexCount, PrimitiveType::Triangles, s_Data.DepthTest);
		s_Data.Stats.DrawCalls++;
	}

	dataSize = static_cast<Uint32>(s_Data.LineVertexBufferPtr - s_Data.LineVertexBufferBase);
	if ( dataSize )
	{
		s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);
		s_Data.LineVertexBuffer->Bind();

		s_Data.LineShader->Bind();
		s_Data.LineShader->SetMat4("u_ViewProjection", s_Data.CameraViewProj);

		s_Data.LinePipeline->Bind();
		s_Data.LineIndexBuffer->Bind();
		Renderer::SetLineThickness(20.0f);
		Renderer::DrawIndexed(s_Data.LineIndexCount, PrimitiveType::Lines, s_Data.DepthTest);
		s_Data.Stats.DrawCalls++;
	}
}

void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color)
{
	constexpr size_t quadVertexCount = 4;
	const float textureIndex = 0.0f; // White Texture
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
	const float tilingFactor = 1.0f;

	if ( s_Data.QuadIndexCount >= Renderer2DData::MaxIndices )
		FlushAndReset();

	for ( size_t i = 0; i < quadVertexCount; i++ )
	{
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::mat4 &transform, const Shared<Texture2D> &texture, float tilingFactor,
						  const glm::vec4 &tintColor)
{
	constexpr size_t quadVertexCount = 4;
	constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

	if ( s_Data.QuadIndexCount >= Renderer2DData::MaxIndices )
		FlushAndReset();

	float textureIndex = 0.0f;
	for ( Uint32 i = 1; i < s_Data.TextureSlotIndex; i++ )
	{
		if ( *s_Data.TextureSlots[i].Raw() == *texture.Raw() )
		{
			textureIndex = static_cast<float>(i);
			break;
		}
	}

	if ( textureIndex == 0.0f )
	{
		if ( s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots )
			FlushAndReset();

		textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	for ( size_t i = 0; i < quadVertexCount; i++ )
	{
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, color);

}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
{
	if ( s_Data.QuadIndexCount >= Renderer2DData::MaxIndices )
		FlushAndReset();

	const float textureIndex = 0.0f; // White Texture
	const float tilingFactor = 1.0f;

	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Shared<Texture2D> &texture,
						  float tilingFactor, const glm::vec4 &tintColor)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);

}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Shared<Texture2D> &texture,
						  float tilingFactor, const glm::vec4 &tintColor)
{
	if ( s_Data.QuadIndexCount >= Renderer2DData::MaxIndices )
		FlushAndReset();

	constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	float textureIndex = 0.0f;
	for ( Uint32 i = 1; i < s_Data.TextureSlotIndex; i++ )
	{
		if ( *s_Data.TextureSlots[i].Raw() == *texture.Raw() )
		{
			textureIndex = static_cast<float>(i);
			break;
		}
	}

	if ( textureIndex == 0.0f )
	{
		textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
								 const glm::vec4 &color)
{
	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);

}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
								 const glm::vec4 &color)
{
	if ( s_Data.QuadIndexCount >= Renderer2DData::MaxIndices )
		FlushAndReset();

	const float textureIndex = 0.0f; // White Texture
	const float tilingFactor = 1.0f;

	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
								 const Shared<Texture2D> &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);

}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
								 const Shared<Texture2D> &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	if ( s_Data.QuadIndexCount >= Renderer2DData::MaxIndices )
		FlushAndReset();

	constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	float textureIndex = 0.0f;
	for ( Uint32 i = 1; i < s_Data.TextureSlotIndex; i++ )
	{
		if ( *s_Data.TextureSlots[i].Raw() == *texture.Raw() )
		{
			textureIndex = static_cast<float>(i);
			break;
		}
	}

	if ( textureIndex == 0.0f )
	{
		textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data.QuadVertexBufferPtr++;

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
	if ( s_Data.LineIndexCount >= Renderer2DData::MaxLineIndices )
		FlushAndResetLines();

	s_Data.LineVertexBufferPtr->Position = p0;
	s_Data.LineVertexBufferPtr->Color = color;
	s_Data.LineVertexBufferPtr++;

	s_Data.LineVertexBufferPtr->Position = p1;
	s_Data.LineVertexBufferPtr->Color = color;
	s_Data.LineVertexBufferPtr++;

	s_Data.LineIndexCount += 2;

	s_Data.Stats.LineCount++;
}

void Renderer2D::ResetStats()
{
	memset(&s_Data.Stats, 0, sizeof(Statistics));
}

const Renderer2D::Statistics &Renderer2D::GetStats()
{
	return s_Data.Stats;
}

void Renderer2D::FlushAndReset()
{
	EndScene();

	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	s_Data.TextureSlotIndex = 1;
}

void Renderer2D::FlushAndResetLines()
{
	EndScene();

	s_Data.LineIndexCount = 0;
	s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
}
}
