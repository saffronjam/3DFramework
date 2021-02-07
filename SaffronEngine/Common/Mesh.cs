using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Numerics;
using System.Runtime.InteropServices;
using Renderer;

namespace SaffronEngine.Common
{
    public interface IUniformGroup
    {
        void SubmitPerDrawUniforms();
    }

    public class RenderStateGroup
    {
        public RenderState State;
        public readonly uint BlendFactorRgba;
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
        private VertexLayout _vertexLayout;
        public readonly List<MeshGroup> _groups;

        public Mesh(MemoryBlock vertices, VertexLayout layout, ushort[] indices)
        {
            var group = new MeshGroup();
            group.VertexBuffer = new VertexBuffer(vertices, layout);
            group.IndexBuffer = new IndexBuffer(MemoryBlock.FromArray(indices));

            _vertexLayout = layout;
            _groups = new List<MeshGroup> {group};
        }

        internal Mesh(VertexLayout layout, List<MeshGroup> groups)
        {
            _vertexLayout = layout;
            this._groups = groups;
        }

        public void Submit(
            byte viewId,
            Program program,
            Matrix4x4 transform,
            RenderStateGroup renderStateGroup,
            IUniformGroup uniforms = null,
            Texture texture = null,
            Uniform textureSampler = default)
        {
            foreach (var group in _groups)
            {
                uniforms?.SubmitPerDrawUniforms();

                if (texture != null)
                {
                    Bgfx.SetTexture(0, textureSampler, texture);
                }

                unsafe
                {
                    Bgfx.SetTransform((float*) &transform);
                }

                Bgfx.SetIndexBuffer(group.IndexBuffer);
                Bgfx.SetVertexBuffer(0, group.VertexBuffer);
                Bgfx.SetRenderState(renderStateGroup.State, (int) renderStateGroup.BlendFactorRgba);
                Bgfx.SetStencil(renderStateGroup.FrontFace, renderStateGroup.BackFace);
                Bgfx.Submit(viewId, program);
            }
        }

        public void Dispose()
        {
            foreach (var group in _groups)
            {
                group.VertexBuffer.Dispose();
                group.IndexBuffer.Dispose();
            }

            _groups.Clear();
        }

        public static Mesh Create(string filepath)
        {
            return ResourceLoader.LoadMesh(filepath);
        }
    }

    public class MeshGroup
    {
        public VertexBuffer VertexBuffer { get; set; }

        public IndexBuffer IndexBuffer { get; set; }

        public Collection<Primitive> Primitives { get; private set; }

        public MeshGroup()
        {
            Primitives = new Collection<Primitive>();
        }
    }

#pragma warning disable 649 // Field 'Primitive.StartIndex' is never assigned to, and will always have its default value 0
    [StructLayout(LayoutKind.Sequential)]
    public readonly struct Primitive
    {
        public readonly int StartIndex;
        public readonly int IndexCount;
        public readonly int StartVertex;
        public readonly int VertexCount;
    }
#pragma warning restore 649
}