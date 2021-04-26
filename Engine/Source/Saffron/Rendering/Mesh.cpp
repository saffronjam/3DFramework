#include "SaffronPCH.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "Saffron/Core/Global.h"
#include "Saffron/Resource/ResourceManager.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
////////////////////////////////////////////////////////////////////////
/// Helper Macros
////////////////////////////////////////////////////////////////////////

#define MESH_DEBUG_LOG 0
#if MESH_DEBUG_LOG
#define SE_MESH_LOG(...) Log::CoreTrace(__VA_ARGS__)
#else
#define SE_MESH_LOG(...)
#endif


////////////////////////////////////////////////////////////////////////
/// Helper functions
////////////////////////////////////////////////////////////////////////

Matrix4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix)
{
	Matrix4 result;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	result[0][0] = matrix.a1;
	result[1][0] = matrix.a2;
	result[2][0] = matrix.a3;
	result[3][0] = matrix.a4;
	result[0][1] = matrix.b1;
	result[1][1] = matrix.b2;
	result[2][1] = matrix.b3;
	result[3][1] = matrix.b4;
	result[0][2] = matrix.c1;
	result[1][2] = matrix.c2;
	result[2][2] = matrix.c3;
	result[3][2] = matrix.c4;
	result[0][3] = matrix.d1;
	result[1][3] = matrix.d2;
	result[2][3] = matrix.d3;
	result[3][3] = matrix.d4;
	return result;
}

// TODO: Remove this unused function
static String LevelToSpaces(uint level)
{
	String result;
	for (uint i = 0; i < level; i++) result += "--";
	return result;
}

////////////////////////////////////////////////////////////////////////
/// Helper Structs
////////////////////////////////////////////////////////////////////////

static const uint s_MeshImportFlags = aiProcess_CalcTangentSpace | // Create binormals/tangents just in case
	aiProcess_Triangulate | // Make sure we're triangles
	aiProcess_SortByPType | // Split meshes by primitive type
	aiProcess_GenNormals | // Make sure we have legit normals
	aiProcess_GenUVCoords | // Convert UVs if required 
	aiProcess_OptimizeMeshes | // Batch draws where possible
	aiProcess_ValidateDataStructure; // Validation

struct LogStream : Assimp::LogStream
{
	static void Initialize()
	{
		if (Assimp::DefaultLogger::isNullLogger())
		{
			Assimp::DefaultLogger::create("", Assimp::Logger::NORMAL);
			Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err /*| Assimp::Logger::Warn*/);
		}
	}

	void write(const char* message) override
	{
		Log::CoreWarn(Log::Fmt::OnCyan + Log::Fmt::Black + "Assimp Warning" + Log::Fmt::Warn + " {0}", message);
	}
};

////////////////////////////////////////////////////////////////////////
/// Animated Vertex
////////////////////////////////////////////////////////////////////////

void AnimatedVertex::AddBoneData(uint BoneID, float Weight)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (Weights[i] == 0.0)
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// TODO: Keep top weights
	Log::CoreWarn(
		"Vertex has more than four bones/weights affecting it, extra data will be discarded (BoneID={0}, Weight={1})",
		BoneID, Weight);
}


////////////////////////////////////////////////////////////////////////
/// Vertex Bone Data
////////////////////////////////////////////////////////////////////////

VertexBoneData::VertexBoneData()
{
	memset(IDs, 0, sizeof IDs);
	memset(Weights, 0, sizeof Weights);
}

void VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (Weights[i] == 0.0)
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	Debug::Break("Too many bones!");
}


////////////////////////////////////////////////////////////////////////
/// Mesh
////////////////////////////////////////////////////////////////////////

Mesh::Mesh(Filepath filepath) :
	_localTransform(1),
	_filepath(MeshesFolder + Move(filepath).string())
{
	LogStream::Initialize();

	Log::CoreInfo("Loading mesh: {0}", _filepath.string());

	_importer = std::make_unique<Assimp::Importer>();

	const auto fpStr = _filepath.string();
	const aiScene* scene = _importer->ReadFile(fpStr.c_str(), s_MeshImportFlags);
	if (!scene || !scene->HasMeshes())
	{
		Log::CoreError("Failed to load mesh file: {0}", fpStr);
		return;
	}

	_scene = scene;

	_isAnimated = scene->mAnimations != nullptr;

	const auto* meshShaderPath = _isAnimated ? "SaffronPBR_Anim" : "SaffronPBR_Static";
	_meshShader = Shader::Create(meshShaderPath);
	_baseMaterial = Material::Create(_meshShader);
	// _materialInstance = Shared<MaterialInstance>::Create(_baseMaterial);
	//_inverseTransform = Matrix4f(0);// glm::inverse(Mat4FromAssimpMat4(scene->mRootNode->mTransformation));

	uint vertexCount = 0;
	uint indexCount = 0;

	_submeshes.reserve(scene->mNumMeshes);
	for (size_t m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];

		Submesh& submesh = _submeshes.emplace_back();
		submesh.BaseVertex = vertexCount;
		submesh.BaseIndex = indexCount;
		submesh.MaterialIndex = mesh->mMaterialIndex;
		submesh.IndexCount = mesh->mNumFaces * 3;
		submesh.MeshName = mesh->mName.C_Str();

		vertexCount += mesh->mNumVertices;
		indexCount += submesh.IndexCount;

		Debug::Assert(mesh->HasPositions(), "Meshes require positions.");;
		Debug::Assert(mesh->HasNormals(), "Meshes require normals.");;

		// Vertices
		if (_isAnimated)
		{
			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				AnimatedVertex vertex;
				vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
				vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
					vertex.Binormal = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
				}

				if (mesh->HasTextureCoords(0))
					vertex.TexCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

				_animatedVertices.push_back(vertex);
			}
		}
		else
		{
			auto& aabb = submesh.BoundingBox;
			aabb.Min = {FLT_MAX, FLT_MAX, FLT_MAX};
			aabb.Max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex{};
				vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
				vertex.Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
				aabb.Min.x = glm::min(vertex.Position.x, aabb.Min.x);
				aabb.Min.y = glm::min(vertex.Position.y, aabb.Min.y);
				aabb.Min.z = glm::min(vertex.Position.z, aabb.Min.z);
				aabb.Max.x = glm::max(vertex.Position.x, aabb.Max.x);
				aabb.Max.y = glm::max(vertex.Position.y, aabb.Max.y);
				aabb.Max.z = glm::max(vertex.Position.z, aabb.Max.z);

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
					vertex.Binormal = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
				}

				if (mesh->HasTextureCoords(0))
					vertex.TexCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};

				_staticVertices.push_back(vertex);
			}
		}

		// Indices
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			const auto& face = mesh->mFaces[i];
			Debug::Assert(face.mNumIndices == 3, "Must have 3 indices.");;
			Index index = {face.mIndices[0], face.mIndices[1], face.mIndices[2]};
			_indices.push_back(index);

			if (!_isAnimated)
				_triangleCache[static_cast<uint>(m)].emplace_back(_staticVertices[index.V1 + submesh.BaseVertex],
				                                                  _staticVertices[index.V2 + submesh.BaseVertex],
				                                                  _staticVertices[index.V3 + submesh.BaseVertex]);
		}
	}

	TraverseNodes(scene->mRootNode);

	// Bones
	if (_isAnimated)
	{
		for (size_t m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];
			Submesh& submesh = _submeshes[m];

			for (size_t i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* bone = mesh->mBones[i];
				String boneName(bone->mName.data);
				int boneIndex = 0;

				if (_boneMapping.find(boneName) == _boneMapping.end())
				{
					// Allocate an index for a new bone
					boneIndex = _boneCount;
					_boneCount++;
					BoneInfo bi{};
					_boneInfo.push_back(bi);
					_boneInfo[boneIndex].BoneOffset = Mat4FromAssimpMat4(bone->mOffsetMatrix);
					_boneMapping[boneName] = boneIndex;
				}
				else
				{
					SE_MESH_LOG("Found existing bone in map");
					boneIndex = _boneMapping[boneName];
				}

				for (size_t j = 0; j < bone->mNumWeights; j++)
				{
					int VertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
					float Weight = bone->mWeights[j].mWeight;
					_animatedVertices[VertexID].AddBoneData(boneIndex, Weight);
				}
			}
		}
	}

	// Materials
	if (scene->HasMaterials())
	{
		SE_MESH_LOG("---- Materials - {0} ----", _filepath.string());

		_textures.resize(scene->mNumMaterials);
		_materials.resize(scene->mNumMaterials);
		for (uint i = 0; i < scene->mNumMaterials; i++)
		{
			auto* aiMaterial = scene->mMaterials[i];
			auto aiMaterialName = aiMaterial->GetName();

			auto mi = Shared<MaterialInstance>::Create(_baseMaterial, aiMaterialName.data);
			_materials[i] = mi;

			SE_MESH_LOG("  {0} (Index = {1})", aiMaterialName.data, i);
			aiString aiTexPath;
			uint textureCount = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
			SE_MESH_LOG("    TextureCount = {0}", textureCount);

			aiColor3D aiColor;
			aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);

			float shininess, metalness;
			if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS) shininess = 80.0f; // Default value

			if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != aiReturn_SUCCESS) metalness = 0.0f;

			float roughness = 1.0f - glm::sqrt(shininess / 100.0f);
			SE_MESH_LOG("    COLOR = {0}, {1}, {2}", aiColor.r, aiColor.g, aiColor.b);
			SE_MESH_LOG("    ROUGHNESS = {0}", roughness);
			bool hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
			if (hasAlbedoMap)
			{
				// TODO: Temp - this should be handled by Saffron's filesystem
				Filepath path = _filepath;
				auto parentPath = path.parent_path();
				parentPath /= String(aiTexPath.data);
				String texturePath = parentPath.string();
				SE_MESH_LOG("    Albedo map path = {0}", texturePath);
				auto texture = Texture2D::Create(texturePath, true);
				if (texture->Loaded())
				{
					_textures[i] = texture;
					mi->Set("u_AlbedoTexture", _textures[i]);
					mi->Set("u_AlbedoTexToggle", 1.0f);
				}
				else
				{
					Log::CoreError("Could not load texture: {0}", texturePath);
					// Fallback to albedo color
					mi->Set("u_AlbedoColor", Vector3{aiColor.r, aiColor.g, aiColor.b});
				}
			}
			else
			{
				mi->Set("u_AlbedoColor", Vector3{aiColor.r, aiColor.g, aiColor.b});
				SE_MESH_LOG("    No albedo map");
			}

			// Normal maps
			mi->Set("u_NormalTexToggle", 0.0f);
			if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS)
			{
				// TODO: Temp - this should be handled by Saffron's filesystem
				Filepath path = _filepath;
				auto parentPath = path.parent_path();
				parentPath /= String(aiTexPath.data);
				String texturePath = parentPath.string();
				SE_MESH_LOG("    Normal map path = {0}", texturePath);
				auto texture = Texture2D::Create(texturePath);
				if (texture->Loaded())
				{
					mi->Set("u_NormalTexture", texture);
					mi->Set("u_NormalTexToggle", 1.0f);
				}
				else
				{
					Log::CoreError("    Could not load texture: {0}", texturePath);
				}
			}
			else
			{
				SE_MESH_LOG("    No normal map");
			}

			// Roughness map
			// mi->Set("u_Roughness", 1.0f);
			// mi->Set("u_RoughnessTexToggle", 0.0f);
			if (aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS)
			{
				// TODO: Temp - this should be handled by Saffron's filesystem
				Filepath path = _filepath;
				auto parentPath = path.parent_path();
				parentPath /= String(aiTexPath.data);
				String texturePath = parentPath.string();
				SE_MESH_LOG("    Roughness map path = {0}", texturePath);
				auto texture = Texture2D::Create(texturePath);
				if (texture->Loaded())
				{
					mi->Set("u_RoughnessTexture", texture);
					mi->Set("u_RoughnessTexToggle", 1.0f);
				}
				else
				{
					Log::CoreError("    Could not load texture: {0}", texturePath);
				}
			}
			else
			{
				SE_MESH_LOG("    No roughness map");
				mi->Set("u_Roughness", roughness);
			}

#if 0
			// Metalness map (or is it??)
			if ( aiMaterial->Instance("$raw.ReflectionFactor|file", aiPTI_String, 0, aiTexPath) == AI_SUCCESS )
			{
				// TODO: Temp - this should be handled by Saffron's filesystem
				Filepath path = _filepath;
				auto parentPath = path.parent_path();
				parentPath /= String(aiTexPath.data);
				String texturePath = parentPath.string();

				auto texture = Texture2D::Create(texturePath);
				if ( texture->Loaded() )
				{
					SE_MESH_LOG("    Metalness map path = {0}", texturePath);
					mi->Set("u_MetalnessTexture", texture);
					mi->Set("u_MetalnessTexToggle", 1.0f);
				}
				else
				{
					Log::CoreError("Could not load texture: {0}", texturePath);
				}
			}
			else
			{
				SE_MESH_LOG("    No metalness texture");
				mi->Set("u_Metalness", metalness);
			}
#endif

			bool metalnessTextureFound = false;
			for (uint i = 0; i < aiMaterial->mNumProperties; i++)
			{
				auto* prop = aiMaterial->mProperties[i];

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

				if (prop->mType == aiPTI_String)
				{
					uint strLength = *reinterpret_cast<uint*>(prop->mData);
					String str(prop->mData + 4, strLength);

					String key = prop->mKey.data;
					if (key == "$raw.ReflectionFactor|file")
					{
						metalnessTextureFound = true;

						// TODO: Temp - this should be handled by Saffron's filesystem
						Filepath path = _filepath;
						auto parentPath = path.parent_path();
						parentPath /= str;
						String texturePath = parentPath.string();
						SE_MESH_LOG("    Metalness map path = {0}", texturePath);
						auto texture = Texture2D::Create(texturePath);
						if (texture->Loaded())
						{
							mi->Set("u_MetalnessTexture", texture);
							mi->Set("u_MetalnessTexToggle", 1.0f);
						}
						else
						{
							Log::CoreError("    Could not load texture: {0}", texturePath);
							mi->Set("u_Metalness", metalness);
							mi->Set("u_MetalnessTexToggle", 0.0f);
						}
						break;
					}
				}
			}

			if (!metalnessTextureFound)
			{
				SE_MESH_LOG("    No metalness map");

				mi->Set("u_Metalness", metalness);
				mi->Set("u_MetalnessTexToggle", 0.0f);
			}
		}
		SE_MESH_LOG("------------------------");

		SuccessfulLoad();
	}

	VertexBufferLayout vertexLayout;
	if (_isAnimated)
	{
		_vertexBuffer = VertexBuffer::Create(_animatedVertices.data(),
		                                     static_cast<uint>(_animatedVertices.size() * sizeof(AnimatedVertex)));
		vertexLayout = {
			{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"},
			{ShaderDataType::Float3, "a_Tangent"}, {ShaderDataType::Float3, "a_Binormal"},
			{ShaderDataType::Float2, "a_TexCoord"}, {ShaderDataType::Int4, "a_BoneIDs"},
			{ShaderDataType::Float4, "a_BoneWeights"},
		};
	}
	else
	{
		_vertexBuffer = VertexBuffer::Create(_staticVertices.data(),
		                                     static_cast<uint>(_staticVertices.size() * sizeof(Vertex)));
		vertexLayout = {
			{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"},
			{ShaderDataType::Float3, "a_Tangent"}, {ShaderDataType::Float3, "a_Binormal"},
			{ShaderDataType::Float2, "a_TexCoord"},
		};
	}

	_indexBuffer = IndexBuffer::Create(_indices.data(), static_cast<uint>(_indices.size() * sizeof(Index)));

	PipelineSpecification pipelineSpecification;
	pipelineSpecification.Layout = vertexLayout;
	_pipeline = Pipeline::Create(pipelineSpecification);
}

void Mesh::OnUpdate()
{
	const auto ts = Global::Timer::GetStep();
	if (_isAnimated)
	{
		if (_animationPlaying)
		{
			_worldTime += ts.sec();

			const double base = _scene->mAnimations[0]->mTicksPerSecond != 0
				                    ? _scene->mAnimations[0]->mTicksPerSecond
				                    : 25.0;
			const float ticksPerSecond = static_cast<float>(base) * _timeMultiplier;
			_animationTime += (ts * ticksPerSecond).sec();
			_animationTime = fmod(_animationTime, static_cast<float>(_scene->mAnimations[0]->mDuration));
		}

		// TODO: We only need to recalc bones if rendering has been requested at the current animation frame
		BoneTransform(_animationTime);
	}
}

void Mesh::DumpVertexBuffer()
{
	SE_MESH_LOG("------------------------------------------------------");
	SE_MESH_LOG("Vertex Buffer Dump");
	SE_MESH_LOG("Mesh: {0}", _filePath);
	if (_isAnimated)
	{
		for (auto& animatedVertex : _animatedVertices)
		{
			const auto& av = animatedVertex;
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
		for (auto& staticVertex : _staticVertices)
		{
			const auto& sv = staticVertex;
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

ArrayList<Submesh>& Mesh::GetSubmeshes()
{
	return _submeshes;
}

const ArrayList<Submesh>& Mesh::GetSubmeshes() const
{
	return _submeshes;
}

const Matrix4& Mesh::GetLocalTransform() const
{
	return _localTransform;
}

void Mesh::SetLocalTransform(Matrix4 localTransform)
{
	_localTransform = Move(localTransform);
}

Shared<Shader> Mesh::GetMeshShader() const
{
	return _meshShader;
}

Shared<Material> Mesh::GetMaterial() const
{
	return _baseMaterial;
}

ArrayList<Shared<MaterialInstance>> Mesh::GetMaterials() const
{
	return _materials;
}

const ArrayList<Shared<Texture2D>>& Mesh::GetTextures() const
{
	return _textures;
}

const Filepath& Mesh::GetFilepath() const
{
	return _filepath;
}

ArrayList<Triangle> Mesh::GetTriangleCache(uint index) const
{
	return _triangleCache.at(index);
}

Shared<Mesh> Mesh::Create(Filepath filepath)
{
	auto mesh = Shared<Mesh>::Create(Move(filepath));
	if (!mesh->IsLoaded())
	{
		return ResourceManager::GetFallback("CubeMesh");
	}
	return mesh;
}

ArrayList<AABB> Mesh::GetBoundingBoxes(const Matrix4& transform)
{
	ArrayList<AABB> ret;
	for (const auto& submesh : GetSubmeshes())
	{
		const auto& aabb = submesh.BoundingBox;
		auto aabbTransform = transform * GetLocalTransform() * submesh.Transform;
		ret.push_back(aabb);
	}
	return ret;
}

void Mesh::BoneTransform(float time)
{
	ReadNodeHierarchy(time, _scene->mRootNode, Matrix4(1.0f));
	_boneTransforms.resize(_boneCount);
	for (size_t i = 0; i < _boneCount; i++) _boneTransforms[i] = _boneInfo[i].FinalTransformation;
}

void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4& parentTransform)
{
	const String name(pNode->mName.data);
	const aiAnimation* animation = _scene->mAnimations[0];
	Matrix4 nodeTransform(Mat4FromAssimpMat4(pNode->mTransformation));
	const aiNodeAnim* nodeAnim = FindNodeAnim(animation, name);

	if (nodeAnim)
	{
		const Vector3 translation = InterpolateTranslation(AnimationTime, nodeAnim);
		const Matrix4 translationMatrix = translate(Matrix4(1.0f),
		                                            Vector3(translation.x, translation.y, translation.z));

		const glm::quat rotation = InterpolateRotation(AnimationTime, nodeAnim);
		const Matrix4 rotationMatrix = toMat4(rotation);

		const Vector3 scale = InterpolateScale(AnimationTime, nodeAnim);
		const Matrix4 scaleMatrix = glm::scale(Matrix4(1.0f), Vector3(scale.x, scale.y, scale.z));

		nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
	}

	const Matrix4 transform = parentTransform * nodeTransform;

	if (_boneMapping.find(name) != _boneMapping.end())
	{
		const uint BoneIndex = _boneMapping[name];
		_boneInfo[BoneIndex].FinalTransformation = _inverseTransform * transform * _boneInfo[BoneIndex].BoneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], transform);
}

void Mesh::TraverseNodes(aiNode* node, const Matrix4& parentTransform, uint level)
{
	const Matrix4 transform = parentTransform * Mat4FromAssimpMat4(node->mTransformation);
	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		const uint mesh = node->mMeshes[i];
		auto& submesh = _submeshes[mesh];
		submesh.NodeName = node->mName.C_Str();
		submesh.Transform = transform;
	}

	// SE_MESH_LOG("{0} {1}", LevelToSpaces(level), node->mName.C_Str());

	for (uint i = 0; i < node->mNumChildren; i++) TraverseNodes(node->mChildren[i], transform, level + 1);
}

uint Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < static_cast<float>(pNodeAnim->mPositionKeys[i + 1].mTime)) return i;
	}

	return 0;
}

uint Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	Debug::Assert(pNodeAnim->mNumRotationKeys > 0);;

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < static_cast<float>(pNodeAnim->mRotationKeys[i + 1].mTime)) return i;
	}

	return 0;
}

const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* animation, const String& nodeName)
{
	for (uint i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];
		if (String(nodeAnim->mNodeName.data) == nodeName) return nodeAnim;
	}
	return nullptr;
}

uint Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	Debug::Assert(pNodeAnim->mNumScalingKeys > 0);;

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (AnimationTime < static_cast<float>(pNodeAnim->mScalingKeys[i + 1].mTime)) return i;
	}

	return 0;
}

Vector3 Mesh::InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumPositionKeys == 1)
	{
		// No interpolation necessary for single value
		const auto v = nodeAnim->mPositionKeys[0].mValue;
		return {v.x, v.y, v.z};
	}

	const uint PositionIndex = FindPosition(animationTime, nodeAnim);
	const uint NextPositionIndex = PositionIndex + 1;
	Debug::Assert(NextPositionIndex < nodeAnim->mNumPositionKeys);;
	const float DeltaTime = static_cast<float>(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->
		mPositionKeys[PositionIndex].mTime);
	float Factor = (animationTime - static_cast<float>(nodeAnim->mPositionKeys[PositionIndex].mTime)) / DeltaTime;
	Debug::Assert(Factor <= 1.0f, "Factor must be below 1.0f");;
	Factor = glm::clamp(Factor, 0.0f, 1.0f);
	const aiVector3D& Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = nodeAnim->mPositionKeys[NextPositionIndex].mValue;
	const aiVector3D Delta = End - Start;
	const auto aiVec = Start + Factor * Delta;
	return {aiVec.x, aiVec.y, aiVec.z};
}


glm::quat Mesh::InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumRotationKeys == 1)
	{
		// No interpolation necessary for single value
		const auto v = nodeAnim->mRotationKeys[0].mValue;
		return glm::quat(v.w, v.x, v.y, v.z);
	}

	const uint RotationIndex = FindRotation(animationTime, nodeAnim);
	const uint NextRotationIndex = RotationIndex + 1;
	Debug::Assert(NextRotationIndex < nodeAnim->mNumRotationKeys);;
	const float DeltaTime = static_cast<float>(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->
		mRotationKeys[RotationIndex].mTime);
	float Factor = (animationTime - static_cast<float>(nodeAnim->mRotationKeys[RotationIndex].mTime)) / DeltaTime;
	Debug::Assert(Factor <= 1.0f, "Factor must be below 1.0f");;
	Factor = glm::clamp(Factor, 0.0f, 1.0f);
	const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
	auto q = aiQuaternion();
	aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
	q = q.Normalize();
	return glm::quat(q.w, q.x, q.y, q.z);
}


Vector3 Mesh::InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumScalingKeys == 1)
	{
		// No interpolation necessary for single value
		const auto v = nodeAnim->mScalingKeys[0].mValue;
		return {v.x, v.y, v.z};
	}

	const uint index = FindScaling(animationTime, nodeAnim);
	const uint nextIndex = index + 1;
	Debug::Assert(nextIndex < nodeAnim->mNumScalingKeys);;
	const float deltaTime = static_cast<float>(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].
		mTime);
	float factor = (animationTime - static_cast<float>(nodeAnim->mScalingKeys[index].mTime)) / deltaTime;
	Debug::Assert(factor <= 1.0f, "Factor must be below 1.0f");;
	factor = glm::clamp(factor, 0.0f, 1.0f);
	const auto& start = nodeAnim->mScalingKeys[index].mValue;
	const auto& end = nodeAnim->mScalingKeys[nextIndex].mValue;
	const auto delta = end - start;
	const auto aiVec = start + factor * delta;
	return {aiVec.x, aiVec.y, aiVec.z};
}
}
