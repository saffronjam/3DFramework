#pragma once

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Core/Time.h"
#include "Saffron/Core/Math/AABB.h"
#include "Saffron/Renderer/Material.h"
#include "Saffron/Renderer/IndexBuffer.h"
#include "Saffron/Renderer/Pipeline.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/Renderer/VertexBuffer.h"

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Assimp
{
class Importer;
}

namespace Se
{

////////////////////////////////////////////////////////////////////////
/// Vertex
////////////////////////////////////////////////////////////////////////

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Binormal;
	glm::vec2 TexCoord;
};


////////////////////////////////////////////////////////////////////////
/// Animated Vertex
////////////////////////////////////////////////////////////////////////

struct AnimatedVertex
{
	glm::vec3 Position{};
	glm::vec3 Normal{};
	glm::vec3 Tangent{};
	glm::vec3 Binormal{};
	glm::vec2 TexCoord{};

	Uint32 IDs[4] = { 0, 0,0, 0 };
	float Weights[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

	void AddBoneData(Uint32 BoneID, float Weight);
};

static const int NumAttributes = 5;


////////////////////////////////////////////////////////////////////////
/// Index
////////////////////////////////////////////////////////////////////////

struct Index
{
	Uint32 V1, V2, V3;
};

static_assert(sizeof(Index) == 3 * sizeof(Uint32));


////////////////////////////////////////////////////////////////////////
/// Bone Info
////////////////////////////////////////////////////////////////////////

struct BoneInfo
{
	glm::mat4 BoneOffset;
	glm::mat4 FinalTransformation;
};


////////////////////////////////////////////////////////////////////////
/// Vertex Bone Data
////////////////////////////////////////////////////////////////////////

struct VertexBoneData
{
	Uint32 IDs[4]{};
	float Weights[4]{};

	VertexBoneData();

	void AddBoneData(Uint32 BoneID, float Weight);
};

struct Triangle
{
	Vertex V0, V1, V2;

	Triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2)
		: V0(v0), V1(v1), V2(v2)
	{
	}
};

class Submesh
{
public:
	Uint32 BaseVertex{};
	Uint32 BaseIndex{};
	Uint32 MaterialIndex{};
	Uint32 IndexCount{};

	glm::mat4 Transform{};
	AABB BoundingBox;

	std::string NodeName, MeshName;
};

class Mesh : public RefCounted
{
public:
	Mesh(const std::string &filename);
	~Mesh();

	void OnUpdate(Time ts);
	void DumpVertexBuffer();

	std::vector<Submesh> &GetSubmeshes() { return m_Submeshes; }
	const std::vector<Submesh> &GetSubmeshes() const { return m_Submeshes; }

	Ref<Shader> GetMeshShader() const { return m_MeshShader; }
	Ref<Material> GetMaterial() const { return m_BaseMaterial; }
	std::vector<Ref<MaterialInstance>> GetMaterials() const { return m_Materials; }
	const std::vector<Ref<Texture2D>> &GetTextures() const { return m_Textures; }
	const std::string &GetFilePath() const { return m_FilePath; }

	std::vector<Triangle> GetTriangleCache(Uint32 index) const { return m_TriangleCache.at(index); }
private:
	void BoneTransform(float time);
	void ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const glm::mat4 &ParentTransform);
	void TraverseNodes(aiNode *node, const glm::mat4 &parentTransform = glm::mat4(1.0f), Uint32 level = 0);

	const aiNodeAnim *FindNodeAnim(const aiAnimation *animation, const std::string &nodeName);
	Uint32 FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim);
	Uint32 FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim);
	Uint32 FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim);
	glm::vec3 InterpolateTranslation(float animationTime, const aiNodeAnim *nodeAnim);
	glm::quat InterpolateRotation(float animationTime, const aiNodeAnim *nodeAnim);
	glm::vec3 InterpolateScale(float animationTime, const aiNodeAnim *nodeAnim);
private:
	std::vector<Submesh> m_Submeshes;

	std::unique_ptr<Assimp::Importer> m_Importer;

	glm::mat4 m_InverseTransform{};

	Uint32 m_BoneCount = 0;
	std::vector<BoneInfo> m_BoneInfo;

	Ref<Pipeline> m_Pipeline;
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;

	std::vector<Vertex> m_StaticVertices;
	std::vector<AnimatedVertex> m_AnimatedVertices;
	std::vector<Index> m_Indices;
	std::unordered_map<std::string, Uint32> m_BoneMapping;
	std::vector<glm::mat4> m_BoneTransforms;
	const aiScene *m_Scene;

	// Materials
	Ref<Shader> m_MeshShader;
	Ref<Material> m_BaseMaterial;
	std::vector<Ref<Texture2D>> m_Textures;
	std::vector<Ref<Texture2D>> m_NormalMaps;
	std::vector<Ref<MaterialInstance>> m_Materials;

	std::unordered_map<Uint32, std::vector<Triangle>> m_TriangleCache;

	// Animation
	bool m_IsAnimated = false;
	float m_AnimationTime = 0.0f;
	float m_WorldTime = 0.0f;
	float m_TimeMultiplier = 1.0f;
	bool m_AnimationPlaying = true;

	std::string m_FilePath;

	friend class Renderer;
	friend class SceneHierarchyPanel;
};
}
