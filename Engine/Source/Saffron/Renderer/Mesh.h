#pragma once

#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Core/Time.h"
#include "Saffron/Math/AABB.h"
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
	Vector3f Position;
	Vector3f Normal;
	Vector3f Tangent;
	Vector3f Binormal;
	Vector2f TexCoord;
};


////////////////////////////////////////////////////////////////////////
/// Animated Vertex
////////////////////////////////////////////////////////////////////////

struct AnimatedVertex
{
	Vector3f Position{};
	Vector3f Normal{};
	Vector3f Tangent{};
	Vector3f Binormal{};
	Vector2f TexCoord{};

	Uint32 IDs[4] = {0, 0, 0, 0};
	float Weights[4]{0.0f, 0.0f, 0.0f, 0.0f};

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
	Matrix4f BoneOffset;
	Matrix4f FinalTransformation;
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

	Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) :
		V0(v0),
		V1(v1),
		V2(v2)
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

	Matrix4f Transform{};
	AABB BoundingBox;

	String NodeName, MeshName;
};

class Mesh : public MemManaged<Mesh>
{
public:
	explicit Mesh(Filepath filepath);

	void OnUpdate();
	void DumpVertexBuffer();

	ArrayList<Submesh>& GetSubmeshes();
	const ArrayList<Submesh>& GetSubmeshes() const;

	const Matrix4f& GetLocalTransform() const;
	void SetLocalTransform(Matrix4f localTransform);

	ArrayList<AABB> GetBoundingBoxes(const Matrix4f& transform = Matrix4f(1));
	Shared<Shader> GetMeshShader() const;

	Shared<Material> GetMaterial() const;
	ArrayList<Shared<MaterialInstance>> GetMaterials() const;
	const ArrayList<Shared<Texture2D>>& GetTextures() const;

	const Filepath& GetFilepath() const;
	ArrayList<Triangle> GetTriangleCache(Uint32 index) const;

private:
	void BoneTransform(float time);
	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
	void TraverseNodes(aiNode* node, const Matrix4f& parentTransform = Matrix4f(1.0f), Uint32 level = 0);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const String& nodeName);
	Uint32 FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	Uint32 FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	Uint32 FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	Vector3f InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim);
	glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
	Vector3f InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim);

private:
	ArrayList<Submesh> m_Submeshes;

	Unique<Assimp::Importer> m_Importer;

	Matrix4f m_InverseTransform{};

	Uint32 m_BoneCount = 0;
	ArrayList<BoneInfo> m_BoneInfo;

	Shared<Pipeline> m_Pipeline;
	Shared<VertexBuffer> m_VertexBuffer;
	Shared<IndexBuffer> m_IndexBuffer;

	ArrayList<Vertex> m_StaticVertices;
	ArrayList<AnimatedVertex> m_AnimatedVertices;
	ArrayList<Index> m_Indices;
	UnorderedMap<String, Uint32> m_BoneMapping;
	ArrayList<Matrix4f> m_BoneTransforms;
	const aiScene* m_Scene;

	// Materials
	Shared<Shader> m_MeshShader;
	Shared<Material> m_BaseMaterial;
	ArrayList<Shared<Texture2D>> m_Textures;
	ArrayList<Shared<Texture2D>> m_NormalMaps;
	ArrayList<Shared<MaterialInstance>> m_Materials;
	Matrix4f m_LocalTransform;

	UnorderedMap<Uint32, ArrayList<Triangle>> m_TriangleCache;

	// Animation
	bool m_IsAnimated = false;
	float m_AnimationTime = 0.0f;
	float m_WorldTime = 0.0f;
	float m_TimeMultiplier = 1.0f;
	bool m_AnimationPlaying = true;

	Filepath m_Filepath;

	static constexpr const char *MeshesFolder = "Assets/Meshes/";
	
	friend class Renderer;
	friend class EntityPanel;
	friend class ScenePanel;
};
}
