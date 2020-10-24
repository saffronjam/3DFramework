#include "SaffronPCH.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Renderer/Renderer.h"

namespace Se
{

////////////////////////////////////////////////////////////////////////
/// Helper Macros
////////////////////////////////////////////////////////////////////////

#define MESH_DEBUG_LOG 0
#if MESH_DEBUG_LOG
#define SE_MESH_LOG(...) SE_CORE_TRACE(__VA_ARGS__)
#else
#define SE_MESH_LOG(...)
#endif


////////////////////////////////////////////////////////////////////////
/// Helper functions
////////////////////////////////////////////////////////////////////////

glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4 &matrix)
{
	glm::mat4 result;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
	result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
	result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
	result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
	return result;
}

// TODO: Remove this unused function
static std::string LevelToSpaces(Uint32 level)
{
	std::string result;
	for ( Uint32 i = 0; i < level; i++ )
		result += "--";
	return result;
}

////////////////////////////////////////////////////////////////////////
/// Helper Structs
////////////////////////////////////////////////////////////////////////

static const Uint32 s_MeshImportFlags =
aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
aiProcess_Triangulate |             // Make sure we're triangles
aiProcess_SortByPType |             // Split meshes by primitive type
aiProcess_GenNormals |              // Make sure we have legit normals
aiProcess_GenUVCoords |             // Convert UVs if required 
aiProcess_OptimizeMeshes |          // Batch draws where possible
aiProcess_ValidateDataStructure;    // Validation

struct LogStream : Assimp::LogStream
{
	static void Initialize()
	{
		if ( Assimp::DefaultLogger::isNullLogger() )
		{
			Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
			Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
		}
	}

	void write(const char *message) override
	{
		SE_CORE_ERROR("Assimp error: {0}", message);
	}
};

////////////////////////////////////////////////////////////////////////
/// Animated Vertex
////////////////////////////////////////////////////////////////////////

void AnimatedVertex::AddBoneData(Uint32 BoneID, float Weight)
{
	for ( size_t i = 0; i < 4; i++ )
	{
		if ( Weights[i] == 0.0 )
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// TODO: Keep top weights
	SE_CORE_WARN(
		"Vertex has more than four bones/weights affecting it, extra data will be discarded (BoneID={0}, Weight={1})",
		BoneID, Weight);
}


////////////////////////////////////////////////////////////////////////
/// Vertex Bone Data
////////////////////////////////////////////////////////////////////////

VertexBoneData::VertexBoneData()
{
	memset(IDs, 0, sizeof(IDs));
	memset(Weights, 0, sizeof(Weights));
}

void VertexBoneData::AddBoneData(Uint32 BoneID, float Weight)
{
	for ( size_t i = 0; i < 4; i++ )
	{
		if ( Weights[i] == 0.0 )
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	SE_CORE_ASSERT(false, "Too many bones!");
}


////////////////////////////////////////////////////////////////////////
/// Mesh
////////////////////////////////////////////////////////////////////////

Mesh::Mesh(std::string filename)
	: m_Filepath(std::move(filename))
{
	LogStream::Initialize();

	SE_CORE_INFO("Loading mesh: {0}", m_Filepath.c_str());

	m_Importer = std::make_unique<Assimp::Importer>();

	const aiScene *scene = m_Importer->ReadFile(m_Filepath, s_MeshImportFlags);
	if ( !scene || !scene->HasMeshes() )
		SE_CORE_ERROR("Failed to load mesh file: {0}", m_Filepath);

	m_Scene = scene;

	m_IsAnimated = scene->mAnimations != nullptr;
	m_MeshShader = m_IsAnimated ? Renderer::GetShaderLibrary()->Get("SaffronPBR_Anim") : Renderer::GetShaderLibrary()->Get("SaffronPBR_Static");
	m_BaseMaterial = Shared<Material>::Create(m_MeshShader);
	// m_MaterialInstance = Shared<MaterialInstance>::Create(m_BaseMaterial);
	m_InverseTransform = glm::inverse(Mat4FromAssimpMat4(scene->mRootNode->mTransformation));

	Uint32 vertexCount = 0;
	Uint32 indexCount = 0;

	m_Submeshes.reserve(scene->mNumMeshes);
	for ( size_t m = 0; m < scene->mNumMeshes; m++ )
	{
		aiMesh *mesh = scene->mMeshes[m];

		Submesh &submesh = m_Submeshes.emplace_back();
		submesh.BaseVertex = vertexCount;
		submesh.BaseIndex = indexCount;
		submesh.MaterialIndex = mesh->mMaterialIndex;
		submesh.IndexCount = mesh->mNumFaces * 3;
		submesh.MeshName = mesh->mName.C_Str();

		vertexCount += mesh->mNumVertices;
		indexCount += submesh.IndexCount;

		SE_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
		SE_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

		// Vertices
		if ( m_IsAnimated )
		{
			for ( size_t i = 0; i < mesh->mNumVertices; i++ )
			{
				AnimatedVertex vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

				if ( mesh->HasTangentsAndBitangents() )
				{
					vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
					vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				}

				if ( mesh->HasTextureCoords(0) )
					vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

				m_AnimatedVertices.push_back(vertex);
			}
		}
		else
		{
			auto &aabb = submesh.BoundingBox;
			aabb.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
			aabb.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for ( size_t i = 0; i < mesh->mNumVertices; i++ )
			{
				Vertex vertex{};
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
				aabb.Min.x = glm::min(vertex.Position.x, aabb.Min.x);
				aabb.Min.y = glm::min(vertex.Position.y, aabb.Min.y);
				aabb.Min.z = glm::min(vertex.Position.z, aabb.Min.z);
				aabb.Max.x = glm::max(vertex.Position.x, aabb.Max.x);
				aabb.Max.y = glm::max(vertex.Position.y, aabb.Max.y);
				aabb.Max.z = glm::max(vertex.Position.z, aabb.Max.z);

				if ( mesh->HasTangentsAndBitangents() )
				{
					vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
					vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				}

				if ( mesh->HasTextureCoords(0) )
					vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

				m_StaticVertices.push_back(vertex);
			}
		}

		// Indices
		for ( size_t i = 0; i < mesh->mNumFaces; i++ )
		{
			const auto &face = mesh->mFaces[i];
			SE_CORE_ASSERT(face.mNumIndices == 3, "Must have 3 indices.");
			Index index = { face.mIndices[0], face.mIndices[1], face.mIndices[2] };
			m_Indices.push_back(index);

			if ( !m_IsAnimated )
				m_TriangleCache[static_cast<Uint32>(m)].emplace_back(m_StaticVertices[index.V1 + submesh.BaseVertex],
																	 m_StaticVertices[index.V2 + submesh.BaseVertex],
																	 m_StaticVertices[index.V3 + submesh.BaseVertex]
				);
		}


	}

	TraverseNodes(scene->mRootNode);

	// Bones
	if ( m_IsAnimated )
	{
		for ( size_t m = 0; m < scene->mNumMeshes; m++ )
		{
			aiMesh *mesh = scene->mMeshes[m];
			Submesh &submesh = m_Submeshes[m];

			for ( size_t i = 0; i < mesh->mNumBones; i++ )
			{
				aiBone *bone = mesh->mBones[i];
				std::string boneName(bone->mName.data);
				int boneIndex = 0;

				if ( m_BoneMapping.find(boneName) == m_BoneMapping.end() )
				{
					// Allocate an index for a new bone
					boneIndex = m_BoneCount;
					m_BoneCount++;
					BoneInfo bi{};
					m_BoneInfo.push_back(bi);
					m_BoneInfo[boneIndex].BoneOffset = Mat4FromAssimpMat4(bone->mOffsetMatrix);
					m_BoneMapping[boneName] = boneIndex;
				}
				else
				{
					SE_MESH_LOG("Found existing bone in map");
					boneIndex = m_BoneMapping[boneName];
				}

				for ( size_t j = 0; j < bone->mNumWeights; j++ )
				{
					int VertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
					float Weight = bone->mWeights[j].mWeight;
					m_AnimatedVertices[VertexID].AddBoneData(boneIndex, Weight);
				}
			}
		}
	}

	// Materials
	if ( scene->HasMaterials() )
	{
		SE_MESH_LOG("---- Materials - {0} ----", m_Filepath);

		m_Textures.resize(scene->mNumMaterials);
		m_Materials.resize(scene->mNumMaterials);
		for ( Uint32 i = 0; i < scene->mNumMaterials; i++ )
		{
			auto *aiMaterial = scene->mMaterials[i];
			auto aiMaterialName = aiMaterial->GetName();

			auto mi = Shared<MaterialInstance>::Create(m_BaseMaterial, aiMaterialName.data);
			m_Materials[i] = mi;

			SE_MESH_LOG("  {0} (Index = {1})", aiMaterialName.data, i);
			aiString aiTexPath;
			Uint32 textureCount = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
			SE_MESH_LOG("    TextureCount = {0}", textureCount);

			aiColor3D aiColor;
			aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);

			float shininess, metalness;
			if ( aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS )
				shininess = 80.0f; // Default value

			if ( aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != aiReturn_SUCCESS )
				metalness = 0.0f;

			float roughness = 1.0f - glm::sqrt(shininess / 100.0f);
			SE_MESH_LOG("    COLOR = {0}, {1}, {2}", aiColor.r, aiColor.g, aiColor.b);
			SE_MESH_LOG("    ROUGHNESS = {0}", roughness);
			bool hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
			if ( hasAlbedoMap )
			{
				// TODO: Temp - this should be handled by Saffron's filesystem
				std::filesystem::path path = m_Filepath;
				auto parentPath = path.parent_path();
				parentPath /= std::string(aiTexPath.data);
				std::string texturePath = parentPath.string();
				SE_MESH_LOG("    Albedo map path = {0}", texturePath);
				auto texture = Texture2D::Create(texturePath, true);
				if ( texture->Loaded() )
				{
					m_Textures[i] = texture;
					mi->Set("u_AlbedoTexture", m_Textures[i]);
					mi->Set("u_AlbedoTexToggle", 1.0f);
				}
				else
				{
					SE_CORE_ERROR("Could not load texture: {0}", texturePath);
					// Fallback to albedo color
					mi->Set("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });
				}
			}
			else
			{
				mi->Set("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });
				SE_MESH_LOG("    No albedo map");
			}

			// Normal maps
			mi->Set("u_NormalTexToggle", 0.0f);
			if ( aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS )
			{
				// TODO: Temp - this should be handled by Saffron's filesystem
				std::filesystem::path path = m_Filepath;
				auto parentPath = path.parent_path();
				parentPath /= std::string(aiTexPath.data);
				std::string texturePath = parentPath.string();
				SE_MESH_LOG("    Normal map path = {0}", texturePath);
				auto texture = Texture2D::Create(texturePath);
				if ( texture->Loaded() )
				{
					mi->Set("u_NormalTexture", texture);
					mi->Set("u_NormalTexToggle", 1.0f);
				}
				else
				{
					SE_CORE_ERROR("    Could not load texture: {0}", texturePath);
				}
			}
			else
			{
				SE_MESH_LOG("    No normal map");
			}

			// Roughness map
			// mi->Set("u_Roughness", 1.0f);
			// mi->Set("u_RoughnessTexToggle", 0.0f);
			if ( aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS )
			{
				// TODO: Temp - this should be handled by Saffron's filesystem
				std::filesystem::path path = m_Filepath;
				auto parentPath = path.parent_path();
				parentPath /= std::string(aiTexPath.data);
				std::string texturePath = parentPath.string();
				SE_MESH_LOG("    Roughness map path = {0}", texturePath);
				auto texture = Texture2D::Create(texturePath);
				if ( texture->Loaded() )
				{
					mi->Set("u_RoughnessTexture", texture);
					mi->Set("u_RoughnessTexToggle", 1.0f);
				}
				else
				{
					SE_CORE_ERROR("    Could not load texture: {0}", texturePath);
				}
			}
			else
			{
				SE_MESH_LOG("    No roughness map");
				mi->Set("u_Roughness", roughness);
			}

#if 0
			// Metalness map (or is it??)
			if ( aiMaterial->Get("$raw.ReflectionFactor|file", aiPTI_String, 0, aiTexPath) == AI_SUCCESS )
			{
				// TODO: Temp - this should be handled by Saffron's filesystem
				std::filesystem::path path = m_Filepath;
				auto parentPath = path.parent_path();
				parentPath /= std::string(aiTexPath.data);
				std::string texturePath = parentPath.string();

				auto texture = Texture2D::Create(texturePath);
				if ( texture->Loaded() )
				{
					SE_MESH_LOG("    Metalness map path = {0}", texturePath);
					mi->Set("u_MetalnessTexture", texture);
					mi->Set("u_MetalnessTexToggle", 1.0f);
				}
				else
				{
					SE_CORE_ERROR("Could not load texture: {0}", texturePath);
				}
			}
			else
			{
				SE_MESH_LOG("    No metalness texture");
				mi->Set("u_Metalness", metalness);
			}
#endif

			bool metalnessTextureFound = false;
			for ( Uint32 i = 0; i < aiMaterial->mNumProperties; i++ )
			{
				auto *prop = aiMaterial->mProperties[i];

#if DEBUG_PRINT_ALL_PROPS
				SE_MESH_LOG("Material Property:");
				SE_MESH_LOG("  Name = {0}", prop->mKey.data);
				// SE_MESH_LOG("  Type = {0}", prop->mType);
				// SE_MESH_LOG("  Size = {0}", prop->mDataLength);
				float data = *(float *)prop->mData;
				SE_MESH_LOG("  Value = {0}", data);

				switch ( prop->mSemantic )
				{
				case aiTextureType_NONE:
					SE_MESH_LOG("  Semantic = aiTextureType_NONE");
					break;
				case aiTextureType_DIFFUSE:
					SE_MESH_LOG("  Semantic = aiTextureType_DIFFUSE");
					break;
				case aiTextureType_SPECULAR:
					SE_MESH_LOG("  Semantic = aiTextureType_SPECULAR");
					break;
				case aiTextureType_AMBIENT:
					SE_MESH_LOG("  Semantic = aiTextureType_AMBIENT");
					break;
				case aiTextureType_EMISSIVE:
					SE_MESH_LOG("  Semantic = aiTextureType_EMISSIVE");
					break;
				case aiTextureType_HEIGHT:
					SE_MESH_LOG("  Semantic = aiTextureType_HEIGHT");
					break;
				case aiTextureType_NORMALS:
					SE_MESH_LOG("  Semantic = aiTextureType_NORMALS");
					break;
				case aiTextureType_SHININESS:
					SE_MESH_LOG("  Semantic = aiTextureType_SHININESS");
					break;
				case aiTextureType_OPACITY:
					SE_MESH_LOG("  Semantic = aiTextureType_OPACITY");
					break;
				case aiTextureType_DISPLACEMENT:
					SE_MESH_LOG("  Semantic = aiTextureType_DISPLACEMENT");
					break;
				case aiTextureType_LIGHTMAP:
					SE_MESH_LOG("  Semantic = aiTextureType_LIGHTMAP");
					break;
				case aiTextureType_REFLECTION:
					SE_MESH_LOG("  Semantic = aiTextureType_REFLECTION");
					break;
				case aiTextureType_UNKNOWN:
					SE_MESH_LOG("  Semantic = aiTextureType_UNKNOWN");
					break;
				}
#endif

				if ( prop->mType == aiPTI_String )
				{
					Uint32 strLength = *reinterpret_cast<Uint32 *>(prop->mData);
					std::string str(prop->mData + 4, strLength);

					std::string key = prop->mKey.data;
					if ( key == "$raw.ReflectionFactor|file" )
					{
						metalnessTextureFound = true;

						// TODO: Temp - this should be handled by Saffron's filesystem
						std::filesystem::path path = m_Filepath;
						auto parentPath = path.parent_path();
						parentPath /= str;
						std::string texturePath = parentPath.string();
						SE_MESH_LOG("    Metalness map path = {0}", texturePath);
						auto texture = Texture2D::Create(texturePath);
						if ( texture->Loaded() )
						{
							mi->Set("u_MetalnessTexture", texture);
							mi->Set("u_MetalnessTexToggle", 1.0f);
						}
						else
						{
							SE_CORE_ERROR("    Could not load texture: {0}", texturePath);
							mi->Set("u_Metalness", metalness);
							mi->Set("u_MetalnessTexToggle", 0.0f);
						}
						break;
					}
				}
			}

			if ( !metalnessTextureFound )
			{
				SE_MESH_LOG("    No metalness map");

				mi->Set("u_Metalness", metalness);
				mi->Set("u_MetalnessTexToggle", 0.0f);
			}
		}
		SE_MESH_LOG("------------------------");
	}

	VertexBuffer::Layout vertexLayout;
	if ( m_IsAnimated )
	{
		m_VertexBuffer = VertexBuffer::Create(m_AnimatedVertices.data(), static_cast<Uint32>(m_AnimatedVertices.size() * sizeof(AnimatedVertex)));
		vertexLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Binormal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Int4, "a_BoneIDs" },
			{ ShaderDataType::Float4, "a_BoneWeights" },
		};
	}
	else
	{
		m_VertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), static_cast<Uint32>(m_StaticVertices.size() * sizeof(Vertex)));
		vertexLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Binormal" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		};
	}

	m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), static_cast<Uint32>(m_Indices.size() * sizeof(Index)));

	Pipeline::Specification pipelineSpecification;
	pipelineSpecification.Layout = vertexLayout;
	m_Pipeline = Pipeline::Create(pipelineSpecification);
}

Mesh::~Mesh() = default;

void Mesh::OnUpdate(Time ts)
{
	if ( m_IsAnimated )
	{
		if ( m_AnimationPlaying )
		{
			m_WorldTime += ts.sec();

			const double base = m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0;
			const float ticksPerSecond = static_cast<float>(base) * m_TimeMultiplier;
			m_AnimationTime += (ts * ticksPerSecond).sec();
			m_AnimationTime = fmod(m_AnimationTime, static_cast<float>(m_Scene->mAnimations[0]->mDuration));
		}

		// TODO: We only need to recalc bones if rendering has been requested at the current animation frame
		BoneTransform(m_AnimationTime);
	}
}

void Mesh::DumpVertexBuffer()
{
	// TODO: Convert to ImGui
	SE_MESH_LOG("------------------------------------------------------");
	SE_MESH_LOG("Vertex Buffer Dump");
	SE_MESH_LOG("Mesh: {0}", m_FilePath);
	if ( m_IsAnimated )
	{
		for ( auto &animatedVertex : m_AnimatedVertices )
		{
			const auto &av = animatedVertex;
			SE_MESH_LOG("Vertex: {0}", i);
			SE_MESH_LOG("Position: {0}, {1}, {2}", av.Position.x, av.Position.y, av.Position.z);
			SE_MESH_LOG("Normal: {0}, {1}, {2}", av.Normal.x, av.Normal.y, av.Normal.z);
			SE_MESH_LOG("Binormal: {0}, {1}, {2}", av.Binormal.x, av.Binormal.y, av.Binormal.z);
			SE_MESH_LOG("Tangent: {0}, {1}, {2}", av.Tangent.x, av.Tangent.y, av.Tangent.z);
			SE_MESH_LOG("TexCoord: {0}, {1}", av.Texcoord.x, av.Texcoord.y);
			SE_MESH_LOG("--");
		}
	}
	else
	{
		for ( auto &staticVertex : m_StaticVertices )
		{
			const auto &sv = staticVertex;
			SE_MESH_LOG("Vertex: {0}", i);
			SE_MESH_LOG("Position: {0}, {1}, {2}", sv.Position.x, sv.Position.y, sv.Position.z);
			SE_MESH_LOG("Normal: {0}, {1}, {2}", sv.Normal.x, sv.Normal.y, sv.Normal.z);
			SE_MESH_LOG("Binormal: {0}, {1}, {2}", sv.Binormal.x, sv.Binormal.y, sv.Binormal.z);
			SE_MESH_LOG("Tangent: {0}, {1}, {2}", sv.Tangent.x, sv.Tangent.y, sv.Tangent.z);
			SE_MESH_LOG("TexCoord: {0}, {1}", sv.Texcoord.x, sv.Texcoord.y);
			SE_MESH_LOG("--");
		}
	}
	SE_MESH_LOG("------------------------------------------------------");
}


void Mesh::BoneTransform(float time)
{
	ReadNodeHierarchy(time, m_Scene->mRootNode, glm::mat4(1.0f));
	m_BoneTransforms.resize(m_BoneCount);
	for ( size_t i = 0; i < m_BoneCount; i++ )
		m_BoneTransforms[i] = m_BoneInfo[i].FinalTransformation;
}

void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const glm::mat4 &parentTransform)
{
	const std::string name(pNode->mName.data);
	const aiAnimation *animation = m_Scene->mAnimations[0];
	glm::mat4 nodeTransform(Mat4FromAssimpMat4(pNode->mTransformation));
	const aiNodeAnim *nodeAnim = FindNodeAnim(animation, name);

	if ( nodeAnim )
	{
		const glm::vec3 translation = InterpolateTranslation(AnimationTime, nodeAnim);
		const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

		const glm::quat rotation = InterpolateRotation(AnimationTime, nodeAnim);
		const glm::mat4 rotationMatrix = glm::toMat4(rotation);

		const glm::vec3 scale = InterpolateScale(AnimationTime, nodeAnim);
		const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

		nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
	}

	const glm::mat4 transform = parentTransform * nodeTransform;

	if ( m_BoneMapping.find(name) != m_BoneMapping.end() )
	{
		const Uint32 BoneIndex = m_BoneMapping[name];
		m_BoneInfo[BoneIndex].FinalTransformation = m_InverseTransform * transform * m_BoneInfo[BoneIndex].BoneOffset;
	}

	for ( Uint32 i = 0; i < pNode->mNumChildren; i++ )
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], transform);
}

void Mesh::TraverseNodes(aiNode *node, const glm::mat4 &parentTransform, Uint32 level)
{
	const glm::mat4 transform = parentTransform * Mat4FromAssimpMat4(node->mTransformation);
	for ( Uint32 i = 0; i < node->mNumMeshes; i++ )
	{
		const Uint32 mesh = node->mMeshes[i];
		auto &submesh = m_Submeshes[mesh];
		submesh.NodeName = node->mName.C_Str();
		submesh.Transform = transform;
	}

	// SE_MESH_LOG("{0} {1}", LevelToSpaces(level), node->mName.C_Str());

	for ( Uint32 i = 0; i < node->mNumChildren; i++ )
		TraverseNodes(node->mChildren[i], transform, level + 1);
}

Uint32 Mesh::FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
	for ( Uint32 i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++ )
	{
		if ( AnimationTime < static_cast<float>(pNodeAnim->mPositionKeys[i + 1].mTime) )
			return i;
	}

	return 0;
}

Uint32 Mesh::FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
	SE_CORE_ASSERT(pNodeAnim->mNumRotationKeys > 0);

	for ( Uint32 i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++ )
	{
		if ( AnimationTime < static_cast<float>(pNodeAnim->mRotationKeys[i + 1].mTime) )
			return i;
	}

	return 0;
}

const aiNodeAnim *Mesh::FindNodeAnim(const aiAnimation *animation, const std::string &nodeName)
{
	for ( Uint32 i = 0; i < animation->mNumChannels; i++ )
	{
		const aiNodeAnim *nodeAnim = animation->mChannels[i];
		if ( std::string(nodeAnim->mNodeName.data) == nodeName )
			return nodeAnim;
	}
	return nullptr;
}

Uint32 Mesh::FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
	SE_CORE_ASSERT(pNodeAnim->mNumScalingKeys > 0);

	for ( Uint32 i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++ )
	{
		if ( AnimationTime < static_cast<float>(pNodeAnim->mScalingKeys[i + 1].mTime) )
			return i;
	}

	return 0;
}

glm::vec3 Mesh::InterpolateTranslation(float animationTime, const aiNodeAnim *nodeAnim)
{
	if ( nodeAnim->mNumPositionKeys == 1 )
	{
		// No interpolation necessary for single value
		const auto v = nodeAnim->mPositionKeys[0].mValue;
		return { v.x, v.y, v.z };
	}

	const Uint32 PositionIndex = FindPosition(animationTime, nodeAnim);
	const Uint32 NextPositionIndex = (PositionIndex + 1);
	SE_CORE_ASSERT(NextPositionIndex < nodeAnim->mNumPositionKeys);
	const float DeltaTime = static_cast<float>(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (animationTime - static_cast<float>(nodeAnim->mPositionKeys[PositionIndex].mTime)) / DeltaTime;
	SE_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");
	Factor = glm::clamp(Factor, 0.0f, 1.0f);
	const aiVector3D &Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D &End = nodeAnim->mPositionKeys[NextPositionIndex].mValue;
	const aiVector3D Delta = End - Start;
	const auto aiVec = Start + Factor * Delta;
	return { aiVec.x, aiVec.y, aiVec.z };
}


glm::quat Mesh::InterpolateRotation(float animationTime, const aiNodeAnim *nodeAnim)
{
	if ( nodeAnim->mNumRotationKeys == 1 )
	{
		// No interpolation necessary for single value
		const auto v = nodeAnim->mRotationKeys[0].mValue;
		return glm::quat(v.w, v.x, v.y, v.z);
	}

	const Uint32 RotationIndex = FindRotation(animationTime, nodeAnim);
	const Uint32 NextRotationIndex = (RotationIndex + 1);
	SE_CORE_ASSERT(NextRotationIndex < nodeAnim->mNumRotationKeys);
	const float DeltaTime = static_cast<float>(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (animationTime - static_cast<float>(nodeAnim->mRotationKeys[RotationIndex].mTime)) / DeltaTime;
	SE_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");
	Factor = glm::clamp(Factor, 0.0f, 1.0f);
	const aiQuaternion &StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion &EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
	auto q = aiQuaternion();
	aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
	q = q.Normalize();
	return glm::quat(q.w, q.x, q.y, q.z);
}


glm::vec3 Mesh::InterpolateScale(float animationTime, const aiNodeAnim *nodeAnim)
{
	if ( nodeAnim->mNumScalingKeys == 1 )
	{
		// No interpolation necessary for single value
		const auto v = nodeAnim->mScalingKeys[0].mValue;
		return { v.x, v.y, v.z };
	}

	const Uint32 index = FindScaling(animationTime, nodeAnim);
	const Uint32 nextIndex = (index + 1);
	SE_CORE_ASSERT(nextIndex < nodeAnim->mNumScalingKeys);
	const float deltaTime = static_cast<float>(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
	float factor = (animationTime - static_cast<float>(nodeAnim->mScalingKeys[index].mTime)) / deltaTime;
	SE_CORE_ASSERT(factor <= 1.0f, "Factor must be below 1.0f");
	factor = glm::clamp(factor, 0.0f, 1.0f);
	const auto &start = nodeAnim->mScalingKeys[index].mValue;
	const auto &end = nodeAnim->mScalingKeys[nextIndex].mValue;
	const auto delta = end - start;
	const auto aiVec = start + factor * delta;
	return { aiVec.x, aiVec.y, aiVec.z };
}

}