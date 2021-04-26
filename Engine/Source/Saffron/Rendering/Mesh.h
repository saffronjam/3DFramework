#pragma once

#include "Saffron/Base.h"
#include "Saffron/Math/AABB.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Rendering/Material.h"
#include "Saffron/Rendering/Resources/IndexBuffer.h"
#include "Saffron/Rendering/Resources/Pipeline.h"
#include "Saffron/Rendering/Resources/Shader.h"
#include "Saffron/Rendering/Resources/VertexBuffer.h"

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
	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 Binormal;
	Vector2 TexCoord;
};


////////////////////////////////////////////////////////////////////////
/// Animated Vertex
////////////////////////////////////////////////////////////////////////

struct AnimatedVertex
{
	Vector3 Position{};
	Vector3 Normal{};
	Vector3 Tangent{};
	Vector3 Binormal{};
	Vector2 TexCoord{};

	uint IDs[4] = {0, 0, 0, 0};
	float Weights[4]{0.0f, 0.0f, 0.0f, 0.0f};

	void AddBoneData(uint BoneID, float Weight);
};

static const int NumAttributes = 5;


////////////////////////////////////////////////////////////////////////
/// Index
////////////////////////////////////////////////////////////////////////

struct Index
{
	uint V1, V2, V3;
};

static_assert(sizeof(Index) == 3 * sizeof(uint));


////////////////////////////////////////////////////////////////////////
/// Bone Info
////////////////////////////////////////////////////////////////////////

struct BoneInfo
{
	Matrix4 BoneOffset;
	Matrix4 FinalTransformation;
};


////////////////////////////////////////////////////////////////////////
/// Vertex Bone Data
////////////////////////////////////////////////////////////////////////

struct VertexBoneData
{
	uint IDs[4]{};
	float Weights[4]{};

	VertexBoneData();

	void AddBoneData(uint BoneID, float Weight);
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
	uint BaseVertex{};
	uint BaseIndex{};
	uint MaterialIndex{};
	uint IndexCount{};

	Matrix4 Transform{};
	AABB BoundingBox;

	String NodeName, MeshName;
};

class Mesh : public Managed
{
public:
	explicit Mesh(Filepath filepath);

	void OnUpdate();
	void DumpVertexBuffer();

	ArrayList<Submesh>& GetSubmeshes();
	const ArrayList<Submesh>& GetSubmeshes() const;

	const Matrix4& GetLocalTransform() const;
	void SetLocalTransform(Matrix4 localTransform);

	ArrayList<AABB> GetBoundingBoxes(const Matrix4& transform = Matrix4(1));
	Shared<Shader> GetMeshShader() const;

	Shared<Material> GetMaterial() const;
	ArrayList<Shared<MaterialInstance>> GetMaterials() const;
	const ArrayList<Shared<Texture2D>>& GetTextures() const;

	const Filepath& GetFilepath() const;
	ArrayList<Triangle> GetTriangleCache(uint index) const;

private:
	void BoneTransform(float time);
	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4& ParentTransform);
	void TraverseNodes(aiNode* node, const Matrix4& parentTransform = Matrix4(1.0f), uint level = 0);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const String& nodeName);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	Vector3 InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim);
	glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
	Vector3 InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim);

private:
	ArrayList<Submesh> _submeshes;

	Unique<Assimp::Importer> _importer;

	Matrix4 _inverseTransform{};

	uint _boneCount = 0;
	ArrayList<BoneInfo> _boneInfo;

	Shared<Pipeline> _pipeline;
	Shared<VertexBuffer> _vertexBuffer;
	Shared<IndexBuffer> _indexBuffer;

	ArrayList<Vertex> _staticVertices;
	ArrayList<AnimatedVertex> _animatedVertices;
	ArrayList<Index> _indices;
	HashMap<String, uint> _boneMapping;
	ArrayList<Matrix4> _boneTransforms;
	const aiScene* _scene;

	// Materials
	Shared<Shader> _meshShader;
	Shared<Material> _baseMaterial;
	ArrayList<Shared<Texture2D>> _textures;
	ArrayList<Shared<Texture2D>> _normalMaps;
	ArrayList<Shared<MaterialInstance>> _materials;
	Matrix4 _localTransform;

	HashMap<uint, ArrayList<Triangle>> _triangleCache;

	// Animation
	bool _isAnimated = false;
	float _animationTime = 0.0f;
	float _worldTime = 0.0f;
	float _timeMultiplier = 1.0f;
	bool _animationPlaying = true;

	Filepath _filepath;

	static constexpr const char* MeshesFolder = "Assets/Meshes/";
	
	friend class Renderer;
	friend class EntityComponentsPanel;
	friend class SceneHierarchyPanel;
};
}
