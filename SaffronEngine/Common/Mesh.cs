using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using Assimp;
using Renderer;
using Matrix4x4 = System.Numerics.Matrix4x4;

namespace SaffronEngine.Common
{
    public interface IUniformGroup
    {
        void SubmitPerDrawUniforms();
    }

    public class RenderStateGroup
    {
        public RenderState State;
        public uint BlendFactorRgba;
        public StencilFlags FrontFace;
        public StencilFlags BackFace;

        public RenderStateGroup(RenderState state, uint blendFactor, StencilFlags frontFace, StencilFlags backFace)
        {
            State = state;
            BlendFactorRgba = blendFactor;
            FrontFace = frontFace;
            BackFace = backFace;
        }
    }

    public class Mesh : IDisposable
    {
        public class Submesh
        {
            public uint BaseVertex;
            public uint BaseIndex;
            public uint MaterialIndex;
            public uint IndexCount;

            public Matrix4x4 Transform;
            public AABB BoundingBox;

            public string NodeName, MeshName;
        };

        public struct Vertex
        {
            public Vector3D Position;
            public Vector3D Normal;
            public Vector3D Tangent;
            public Vector3D BiTangent;
            public Vector3D TexCoord;
        }

        public struct AnimatedVertex
        {
            public Vector3D Position { get; set; }
            public Vector3D Normal { get; set; }
            public Vector3D Tangent { get; set; }
            public Vector3D BiTangent { get; set; }
            public Vector3D TexCoord { get; set; }

            public uint[] IDs { get; set; }
            public float[] Weights { get; set; }

            // TODO: Check if this bothers animations
            // public AnimatedVertex()
            // {
            //     IDs = new uint[4] {0, 0, 0, 0};
            //     Weights = new float[4] {0.0f, 0.0f, 0.0f, 0.0f};
            // }

            public void AddBoneData(uint boneID, float weight)
            {
                for (var i = 0; i < 4; i++)
                {
                    if (Weights[i] != 0.0) continue;

                    IDs[i] = boneID;
                    Weights[i] = weight;
                    return;
                }
            }
        };

        public class BoneInfo
        {
            public Matrix4x4 BoneOffset;
            public Matrix4x4 FinalTransformation;
        };

        // Base
        private Scene _scene;
        private Matrix4x4 _transform;
        private List<Submesh> _submeshes;

        // Vericies
        private List<Vertex> _staticVerticies;
        private List<AnimatedVertex> _animatedVerticies;

        // Bones
        private Dictionary<string, uint> _boneMapping;
        private uint _boneCount = 0;
        private List<BoneInfo> _boneInfos;

        // Layout and buffers
        private VertexLayout vertexLayout;
        private VertexBuffer _vertexBuffer;
        private IndexBuffer _indexBuffer;
        private List<int> _concatenatedIndices;

        public Mesh()
        {
        }

        public unsafe void Submit(
            byte viewId,
            Program program,
            Matrix4x4* transform,
            RenderStateGroup renderStateGroup,
            IUniformGroup uniforms,
            Texture texture = null,
            Uniform textureSampler = default(Uniform))
        {
            if (uniforms != null)
                uniforms.SubmitPerDrawUniforms();

            if (texture != null)
                Bgfx.SetTexture(0, textureSampler, texture);

            Bgfx.SetTransform((float*) transform);
            Bgfx.SetIndexBuffer(_indexBuffer);
            Bgfx.SetVertexBuffer(0, _vertexBuffer);
            Bgfx.SetRenderState(renderStateGroup.State, (int) renderStateGroup.BlendFactorRgba);
            Bgfx.SetStencil(renderStateGroup.FrontFace, renderStateGroup.BackFace);
            Bgfx.Submit(viewId, program);
        }

        public void Dispose()
        {
            _vertexBuffer.Dispose();
            _indexBuffer.Dispose();
        }

        public static Mesh Create(Assimp.Scene scene)
        {
            var saffronMesh = new Mesh
            {
                vertexLayout = new VertexLayout(),
                _transform = Matrix4x4FromAssimpMatrix4x4(scene.RootNode.Transform),
                _scene = scene
            };

            saffronMesh.vertexLayout
                .Begin()
                .Add(VertexAttributeUsage.Position, 3, VertexAttributeType.Float)
                .Add(VertexAttributeUsage.Normal, 3, VertexAttributeType.Float)
                .Add(VertexAttributeUsage.Tangent, 3, VertexAttributeType.Float)
                .Add(VertexAttributeUsage.Bitangent, 3, VertexAttributeType.Float)
                .Add(VertexAttributeUsage.TexCoord0, 2, VertexAttributeType.Float);

            if (scene.HasAnimations)
            {
                saffronMesh.vertexLayout
                    .Add(VertexAttributeUsage.Indices, 4, VertexAttributeType.Int16)
                    .Add(VertexAttributeUsage.Weight, 4, VertexAttributeType.Float);
            }

            saffronMesh.vertexLayout.End();


            var vertexCounter = 0u;
            var indexCounter = 0u;
            foreach (var mesh in scene.Meshes)
            {
                saffronMesh._submeshes = new List<Submesh>(scene.MeshCount);
                var submesh = new Submesh
                {
                    BaseVertex = vertexCounter,
                    BaseIndex = indexCounter,
                    MaterialIndex = (uint) mesh.MaterialIndex,
                    MeshName = mesh.Name
                };
                saffronMesh._submeshes.Add(submesh);

                vertexCounter += (uint) mesh.VertexCount;
                indexCounter += submesh.IndexCount;

                Debug.Assert(mesh.HasVertices, "Meshes require positions.");
                Debug.Assert(mesh.HasNormals, "Meshes require normals.");


                if (scene.HasAnimations)
                {
                    saffronMesh._animatedVerticies = new List<AnimatedVertex>(mesh.VertexCount);
                    for (var i = 0; i < mesh.VertexCount; i++)
                    {
                        var vertex = new AnimatedVertex
                        {
                            Position = mesh.Vertices[i],
                            Normal = mesh.Vertices[i],
                            Tangent = mesh.Vertices[i],
                            BiTangent = mesh.Vertices[i],
                            TexCoord = mesh.TextureCoordinateChannels[0][i]
                        };

                        saffronMesh._animatedVerticies.Add(vertex);
                    }
                }
                else
                {
                    saffronMesh._staticVerticies = new List<Vertex>(mesh.VertexCount);
                    for (var i = 0; i < mesh.VertexCount; i++)
                    {
                        var vertex = new Vertex
                        {
                            Position = mesh.Vertices[i],
                            Normal = mesh.Vertices[i],
                        };

                        if (mesh.HasTangentBasis)
                        {
                            vertex.Tangent = mesh.Vertices[i];
                            vertex.BiTangent = mesh.Vertices[i];
                        }

                        if (mesh.HasTextureCoords(0))
                        {
                            vertex.TexCoord = mesh.TextureCoordinateChannels[0][i];
                        }


                        saffronMesh._staticVerticies.Add(vertex);
                    }
                }
            }

            saffronMesh._concatenatedIndices = new List<int>(scene.Meshes.Count * 3);
            foreach (var face in scene.Meshes.SelectMany(mesh => mesh.Faces))
            {
                Debug.Assert(face.HasIndices && face.IndexCount == 3, "Face must have 3 indices");

                saffronMesh._concatenatedIndices.AddRange(face.Indices);

                if (!scene.HasAnimations)
                {
                    // TODO   
                    /*
                         m_TriangleCache[m].emplace_back(m_StaticVertices[index.V1 + submesh.BaseVertex],
                           m_StaticVertices[index.V2 + submesh.BaseVertex],
                           m_StaticVertices[index.V3 + submesh.BaseVertex]);                 
                      
                     */
                }
            }


            saffronMesh.TraverseNodes(scene.RootNode, Matrix4x4.Identity);

            // Bones
            if (scene.HasAnimations)
            {
                saffronMesh._boneMapping = new Dictionary<string, uint>();
                saffronMesh._boneInfos = new List<BoneInfo>();

                for (var i = 0; i < scene.MeshCount; i++)
                {
                    var mesh = scene.Meshes[i];
                    var submesh = saffronMesh._submeshes[i];

                    foreach (var bone in mesh.Bones)
                    {
                        var boneIndex = 0u;
                        if (!saffronMesh._boneMapping.ContainsKey(bone.Name))
                        {
                            boneIndex = saffronMesh._boneCount;
                            saffronMesh._boneCount++;

                            var boneInfo = new BoneInfo();
                            saffronMesh._boneInfos.Add(boneInfo);
                            saffronMesh._boneInfos[(int) boneIndex].BoneOffset =
                                Matrix4x4FromAssimpMatrix4x4(bone.OffsetMatrix);
                        }
                        else
                        {
                            boneIndex = saffronMesh._boneMapping[bone.Name];
                        }

                        foreach (var vertexWeight in bone.VertexWeights)
                        {
                            var vertexId = submesh.BaseVertex + vertexWeight.VertexID;
                            var weight = vertexWeight.Weight;
                            saffronMesh._animatedVerticies[(int) vertexId].AddBoneData(boneIndex, weight);
                        }
                    }
                }
            }

            // TODO: (BIG) Materials
//             if (scene->HasMaterials())
//             {
//                 HZ_MESH_LOG("---- Materials - {0} ----", filename);
//
//                 m_Textures.resize(scene->mNumMaterials);
//                 m_Materials.resize(scene->mNumMaterials);
//                 for (uint32_t i = 0; i < scene->mNumMaterials; i++)
//                 {
//                     auto aiMaterial = scene->mMaterials[i];
//                     auto aiMaterialName = aiMaterial->GetName();
//
//                     auto mi = Ref < MaterialInstance >::Create(m_BaseMaterial, aiMaterialName.data);
//                     m_Materials[i] = mi;
//
//                     HZ_MESH_LOG("  {0} (Index = {1})", aiMaterialName.data, i);
//                     aiString aiTexPath;
//                     uint32_t textureCount = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
//                     HZ_MESH_LOG("    TextureCount = {0}", textureCount);
//
//                     aiColor3D aiColor;
//                     aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
//
//                     float shininess, metalness;
//                     if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS)
//                         shininess = 80.0f; // Default value
//
//                     if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != aiReturn_SUCCESS)
//                         metalness = 0.0f;
//
//                     float roughness = 1.0f - glm::sqrt(shininess / 100.0f);
//                     HZ_MESH_LOG("    COLOR = {0}, {1}, {2}", aiColor.r, aiColor.g, aiColor.b);
//                     HZ_MESH_LOG("    ROUGHNESS = {0}", roughness);
//                     bool hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
//                     if (hasAlbedoMap)
//                     {
//                         // TODO: Temp - this should be handled by Hazel's filesystem
//                         std::filesystem::path path = filename;
//                         auto parentPath = path.parent_path();
//                         parentPath /= std::string(aiTexPath.data);
//                         std::string texturePath = parentPath.string();
//                         HZ_MESH_LOG("    Albedo map path = {0}", texturePath);
//                         auto texture = Texture2D::Create(texturePath, true);
//                         if (texture->Loaded())
//                         {
//                             m_Textures[i] = texture;
//                             mi->Set("u_AlbedoTexture", m_Textures[i]);
//                             mi->Set("u_AlbedoTexToggle", 1.0f);
//                         }
//                         else
//                         {
//                             HZ_CORE_ERROR("Could not load texture: {0}", texturePath);
//                             // Fallback to albedo color
//                             mi->Set("u_AlbedoColor", glm::vec3{
//                                 aiColor.r, aiColor.g, aiColor.b
//                             });
//                         }
//                     }
//                     else
//                     {
//                         mi->Set("u_AlbedoColor", glm::vec3 {
//                             aiColor.r, aiColor.g, aiColor.b
//                         });
//                         HZ_MESH_LOG("    No albedo map");
//                     }
//
//                     // Normal maps
//                     mi->Set("u_NormalTexToggle", 0.0f);
//                     if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS)
//                     {
//                         // TODO: Temp - this should be handled by Hazel's filesystem
//                         std::filesystem::path path = filename;
//                         auto parentPath = path.parent_path();
//                         parentPath /= std::string(aiTexPath.data);
//                         std::string texturePath = parentPath.string();
//                         HZ_MESH_LOG("    Normal map path = {0}", texturePath);
//                         auto texture = Texture2D::Create(texturePath);
//                         if (texture->Loaded())
//                         {
//                             mi->Set("u_NormalTexture", texture);
//                             mi->Set("u_NormalTexToggle", 1.0f);
//                         }
//                         else
//                         {
//                             HZ_CORE_ERROR("    Could not load texture: {0}", texturePath);
//                         }
//                     }
//                     else
//                     {
//                         HZ_MESH_LOG("    No normal map");
//                     }
//
//                     // Roughness map
//                     // mi->Set("u_Roughness", 1.0f);
//                     // mi->Set("u_RoughnessTexToggle", 0.0f);
//                     if (aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS)
//                     {
//                         // TODO: Temp - this should be handled by Hazel's filesystem
//                         std::filesystem::path path = filename;
//                         auto parentPath = path.parent_path();
//                         parentPath /= std::string(aiTexPath.data);
//                         std::string texturePath = parentPath.string();
//                         HZ_MESH_LOG("    Roughness map path = {0}", texturePath);
//                         auto texture = Texture2D::Create(texturePath);
//                         if (texture->Loaded())
//                         {
//                             mi->Set("u_RoughnessTexture", texture);
//                             mi->Set("u_RoughnessTexToggle", 1.0f);
//                         }
//                         else
//                         {
//                             HZ_CORE_ERROR("    Could not load texture: {0}", texturePath);
//                         }
//                     }
//                     else
//                     {
//                         HZ_MESH_LOG("    No roughness map");
//                         mi->Set("u_Roughness", roughness);
//                     }
//
// #if 0
//                     // Metalness map (or is it??)
//                     if (aiMaterial->Get("$raw.ReflectionFactor|file", aiPTI_String, 0, aiTexPath) == AI_SUCCESS)
//                     {
//                         // TODO: Temp - this should be handled by Hazel's filesystem
//                         std::filesystem::path path = filename;
//                         auto parentPath = path.parent_path();
//                         parentPath /= std::string(aiTexPath.data);
//                         std::string texturePath = parentPath.string();
//
//                         auto texture = Texture2D::Create(texturePath);
//                         if (texture->Loaded())
//                         {
//                             HZ_MESH_LOG("    Metalness map path = {0}", texturePath);
//                             mi->Set("u_MetalnessTexture", texture);
//                             mi->Set("u_MetalnessTexToggle", 1.0f);
//                         }
//                         else
//                         {
//                             HZ_CORE_ERROR("Could not load texture: {0}", texturePath);
//                         }
//                     }
//                     else
//                     {
//                         HZ_MESH_LOG("    No metalness texture");
//                         mi->Set("u_Metalness", metalness);
//                     }
// #endif
//
//                     bool metalnessTextureFound = false;
//                     for (uint32_t i = 0; i < aiMaterial->mNumProperties; i++)
//                     {
//                         auto prop = aiMaterial->mProperties[i];
//
// #if DEBUG_PRINT_ALL_PROPS
//         					HZ_MESH_LOG("Material Property:");
//         					HZ_MESH_LOG("  Name = {0}", prop->mKey.data);
//         					// HZ_MESH_LOG("  Type = {0}", prop->mType);
//         					// HZ_MESH_LOG("  Size = {0}", prop->mDataLength);
//         					float data = *(float*)prop->mData;
//         					HZ_MESH_LOG("  Value = {0}", data);
//         
//         					switch (prop->mSemantic)
//         					{
//         					case aiTextureType_NONE:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_NONE");
//         						break;
//         					case aiTextureType_DIFFUSE:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_DIFFUSE");
//         						break;
//         					case aiTextureType_SPECULAR:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_SPECULAR");
//         						break;
//         					case aiTextureType_AMBIENT:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_AMBIENT");
//         						break;
//         					case aiTextureType_EMISSIVE:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_EMISSIVE");
//         						break;
//         					case aiTextureType_HEIGHT:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_HEIGHT");
//         						break;
//         					case aiTextureType_NORMALS:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_NORMALS");
//         						break;
//         					case aiTextureType_SHININESS:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_SHININESS");
//         						break;
//         					case aiTextureType_OPACITY:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_OPACITY");
//         						break;
//         					case aiTextureType_DISPLACEMENT:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_DISPLACEMENT");
//         						break;
//         					case aiTextureType_LIGHTMAP:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_LIGHTMAP");
//         						break;
//         					case aiTextureType_REFLECTION:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_REFLECTION");
//         						break;
//         					case aiTextureType_UNKNOWN:
//         						HZ_MESH_LOG("  Semantic = aiTextureType_UNKNOWN");
//         						break;
//         					}
// #endif
//
//                         if (prop->mType == aiPTI_String)
//                         {
//                             uint32_t strLength = *(uint32_t*) prop->mData;
//                             std::string str(prop->
//                             mData + 4, strLength);
//
//                             std::string key = prop->mKey.data;
//                             if (key == "$raw.ReflectionFactor|file")
//                             {
//                                 metalnessTextureFound = true;
//
//                                 // TODO: Temp - this should be handled by Hazel's filesystem
//                                 std::filesystem::path path = filename;
//                                 auto parentPath = path.parent_path();
//                                 parentPath /= str;
//                                 std::string texturePath = parentPath.string();
//                                 HZ_MESH_LOG("    Metalness map path = {0}", texturePath);
//                                 auto texture = Texture2D::Create(texturePath);
//                                 if (texture->Loaded())
//                                 {
//                                     mi->Set("u_MetalnessTexture", texture);
//                                     mi->Set("u_MetalnessTexToggle", 1.0f);
//                                 }
//                                 else
//                                 {
//                                     HZ_CORE_ERROR("    Could not load texture: {0}", texturePath);
//                                     mi->Set("u_Metalness", metalness);
//                                     mi->Set("u_MetalnessTexToggle", 0.0f);
//                                 }
//
//                                 break;
//                             }
//                         }
//                     }
//
//                     if (!metalnessTextureFound)
//                     {
//                         HZ_MESH_LOG("    No metalness map");
//
//                         mi->Set("u_Metalness", metalness);
//                         mi->Set("u_MetalnessTexToggle", 0.0f);
//                     }
//                 }
//
//                 HZ_MESH_LOG("------------------------");
//             }


            if (scene.HasAnimations)
            {
                saffronMesh._vertexBuffer =
                    new VertexBuffer(MemoryBlock.FromArray(saffronMesh._animatedVerticies.ToArray()),
                        saffronMesh.vertexLayout);
            }
            else
            {
                saffronMesh._vertexBuffer =
                    new VertexBuffer(MemoryBlock.FromArray(saffronMesh._staticVerticies.ToArray()),
                        saffronMesh.vertexLayout);
            }


            saffronMesh._indexBuffer =
                new IndexBuffer(MemoryBlock.FromArray(saffronMesh._concatenatedIndices.ToArray()));


            return saffronMesh;
        }

        private void TraverseNodes(Node node, Matrix4x4 parentTransform, uint level = 0)
        {
            var transform = parentTransform * Matrix4x4FromAssimpMatrix4x4(node.Transform);
            for (var i = 0; i < node.MeshCount; i++)
            {
                var submesh = _submeshes[node.MeshIndices[i]];
                submesh.NodeName = node.Name;
                submesh.Transform = transform;
            }

            foreach (var child in node.Children)
            {
                TraverseNodes(child, transform, level + 1);
            }
        }

        private static Matrix4x4 Matrix4x4FromAssimpMatrix4x4(Assimp.Matrix4x4 assimpMatrix)
        {
            var converted = new Matrix4x4
            {
                M11 = assimpMatrix.A1,
                M12 = assimpMatrix.A2,
                M13 = assimpMatrix.A3,
                M14 = assimpMatrix.A4,

                M21 = assimpMatrix.B1,
                M22 = assimpMatrix.B2,
                M23 = assimpMatrix.B3,
                M24 = assimpMatrix.B4,

                M31 = assimpMatrix.C1,
                M32 = assimpMatrix.C2,
                M33 = assimpMatrix.C3,
                M34 = assimpMatrix.C4,

                M41 = assimpMatrix.D1,
                M42 = assimpMatrix.D2,
                M43 = assimpMatrix.D3,
                M44 = assimpMatrix.D4
            };
            return converted;
        }
    }
}