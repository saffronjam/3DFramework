#pragma once

namespace Se
{
struct BufferFlags
{
	enum Enum : Uint16
	{
		/// No flags specified.		
		None,

		/// Specifies the format of data in a compute buffer as being 8x1.		
		ComputeFormat8x1 = 0x1,

		/// Specifies the format of data in a compute buffer as being 8x2.		
		ComputeFormat8x2 = 0x2,

		/// Specifies the format of data in a compute buffer as being 8x4.		
		ComputeFormat8x4 = 0x3,

		/// Specifies the format of data in a compute buffer as being 16x1.		
		ComputeFormat16x1 = 0x4,

		/// Specifies the format of data in a compute buffer as being 16x2.		
		ComputeFormat16x2 = 0x5,

		/// Specifies the format of data in a compute buffer as being 16x4.		
		ComputeFormat16x4 = 0x6,

		/// Specifies the format of data in a compute buffer as being 32x1.
		ComputeFormat32x1 = 0x7,

		/// Specifies the format of data in a compute buffer as being 32x2.		
		ComputeFormat32x2 = 0x8,

		/// Specifies the format of data in a compute buffer as being 32x4.		
		ComputeFormat32x4 = 0x9,

		/// Specifies the type of data in a compute buffer as being unsigned integers.		
		ComputeTypeInt = 0x10,

		/// Specifies the type of data in a compute buffer as being signed integers.		
		ComputeTypeUInt = 0x20,

		/// Specifies the type of data in a compute buffer as being floating point values.		
		ComputeTypeFloat = 0x30,

		/// Buffer will be read by a compute shader.		
		ComputeRead = 0x100,

		/// Buffer will be written into by a compute shader. It cannot be accessed by the CPU.		
		ComputeWrite = 0x200,

		/// Buffer is the source of indirect draw commands.		
		DrawIndirect = 0x400,

		/// Buffer will resize on update if a different quantity of data is passed. If this flag is not set
		/// the data will be trimmed to fit in the existing buffer size. Effective only for dynamic buffers.		
		AllowResize = 0x800,

		/// Buffer is using 32-bit indices. Useful only for index buffers.		
		Index32 = 0x1000,

		/// Buffer will be read and written by a compute shader.		
		ComputeReadWrite = ComputeRead | ComputeWrite
	};
};

struct TextureFlags
{
	enum Enum : Uint64
	{
		/// No flags set.
		None = 0x00000000,

		/// Mirror the texture in the U coordinate.    
		MirrorU = 0x00000001,

		/// Clamp the texture in the U coordinate.    
		ClampU = 0x00000002,

		/// Use a border color for addresses outside the range in the U coordinate.    
		BorderU = 0x00000003,

		/// Mirror the texture in the V coordinate.    
		MirrorV = 0x00000004,

		/// Clamp the texture in the V coordinate.    
		ClampV = 0x00000008,

		/// Use a border color for addresses outside the range in the V coordinate.    
		BorderV = 0x0000000c,

		/// Mirror the texture in the W coordinate.    
		MirrorW = 0x00000010,

		/// Clamp the texture in the W coordinate.    
		ClampW = 0x00000020,

		/// Use a border color for addresses outside the range in the W coordinate.    
		BorderW = 0x00000030,

		/// Mirror the texture in the U,V, and W coordinates.    
		MirrorUVW = MirrorU | MirrorV | MirrorW,

		/// Clamp the texture in the U,V, and W coordinates.    
		ClampUVW = ClampU | ClampV | ClampW,

		/// Use a border color for addresses outside the range in the U,V, and W coordinates.    
		BorderUVW = BorderU | BorderV | BorderW,

		/// Use point filtering for texture minification.    
		MinFilterPoint = 0x00000040,

		/// Use anisotropic filtering for texture minification.    
		MinFilterAnisotropic = 0x00000080,

		/// Use point filtering for texture magnification.    
		MagFilterPoint = 0x00000100,

		/// Use anisotropic filtering for texture magnification.    
		MagFilterAnisotropic = 0x00000200,

		/// Use point filtering for texture mipmaps.    
		MipFilterPoint = 0x00000400,

		/// Use point filtering for minification, magnification, and texture mipmaps.    
		FilterPoint = MinFilterPoint | MagFilterPoint | MipFilterPoint,

		/// Use a "less than" operator when comparing textures.    
		CompareLess = 0x00010000,

		/// Use a "less than or equal" operator when comparing textures.    
		CompareLessEqual = 0x00020000,

		/// Use an equality operator when comparing textures.    
		CompareEqual = 0x00030000,

		/// Use a "greater than or equal" operator when comparing textures.    
		CompareGreaterEqual = 0x00040000,

		/// Use a "greater than" operator when comparing textures.    
		CompareGreater = 0x00050000,

		/// Use an inequality operator when comparing textures.    
		CompareNotEqual = 0x00060000,

		/// Never compare two textures as equal.    
		CompareNever = 0x00070000,

		/// Always compare two textures as equal.    
		CompareAlways = 0x00080000,

		/// Sample stencil instead of depth.    
		SampleStencil = 0x100000,

		/// Perform MSAA sampling on the texture.    
		MSAASample = 0x800000000,

		/// The texture will be used as a render target.    
		RenderTarget = 0x1000000000,

		/// The render target texture support 2x multisampling.    
		RenderTargetMultisample2x = 0x2000000000,

		/// The render target texture support 4x multisampling.

		RenderTargetMultisample4x = 0x3000000000,

		/// The render target texture support 8x multisampling.    
		RenderTargetMultisample8x = 0x4000000000,

		/// The render target texture support 16x multisampling.    
		RenderTargetMultisample16x = 0x5000000000,

		/// The texture is only writeable (render target).    
		RenderTargetWriteOnly = 0x8000000000,

		/// Texture is the target of compute shader writes.    
		ComputeWrite = 0x100000000000,

		/// Texture data is in non-linear sRGB format.    
		Srgb = 0x200000000000,

		/// Texture can be used as the destination of a blit operation.    
		BlitDestination = 0x400000000000,

		/// Texture data can be read back.    
		ReadBack = 0x800000000000
	};
};

struct TextureFormat
{
	enum class Enum
	{
		/// Block compression with three color channels, 1 bit alpha.
		BC1,

		/// Block compression with three color channels, 4 bits alpha.
		BC2,

		/// Block compression with three color channels, 8 bits alpha.
		BC3,

		/// Block compression for 1-channel color.
		BC4,

		/// Block compression for 2-channel color.
		BC5,

		/// Block compression for three-channel HDR color.
		BC6H,

		/// Highest quality block compression.
		BC7,

		/// Original ETC block compression.
		ETC1,

		/// Improved ETC block compression (no alpha).
		ETC2,

		/// Improved ETC block compression with full alpha.
		ETC2A,

		/// Improved ETC block compression with 1-bit punchthrough alpha.
		ETC2A1,

		/// PVRTC1 compression (2 bits per pixel)

		PTC12,

		/// PVRTC1 compression (4 bits per pixel)

		PTC14,

		/// PVRTC1 compression with alpha (2 bits per pixel)

		PTC12A,

		/// PVRTC1 compression with alpha (4 bits per pixel)

		PTC14A,

		/// PVRTC2 compression with alpha (2 bits per pixel)

		PTC22,

		/// PVRTC2 compression with alpha (4 bits per pixel)

		PTC24,

		/// ATC RGB (4 bits per pixel)

		ATC,

		/// ATCE RGBA with explicit alpha (8 bits per pixel)

		ATCE,

		/// ATCE RGBA with interpolated alpha (8 bits per pixel)

		ATCI,

		/// ASTC 4x4 8.0 bpp

		ASTC4x4,

		/// ASTC 5x5 5.12 bpp

		ASTC5x5,

		/// ASTC 6x6 3.56 bpp

		ASTC6x6,

		/// ASTC 8x5 3.20 bpp

		ASTC8x5,

		/// ASTC 8x6 2.67 bpp

		ASTC8x6,

		/// ASTC 10x5 2.56 bpp

		ASTC10x5,

		/// Unknown texture format.
		Unknown,

		/// 1-bit single channel.
		R1,

		/// 8-bit single channel (alpha).
		A8,

		/// 8-bit single channel.
		R8,

		/// 8-bit single channel (integer).
		R8I,

		/// 8-bit single channel (unsigned).
		R8U,

		/// 8-bit single channel (signed).
		R8S,

		/// 16-bit single channel.
		R16,

		/// 16-bit single channel (integer).
		R16I,

		/// 16-bit single channel (unsigned).
		R16U,

		/// 16-bit single channel (float).
		R16F,

		/// 16-bit single channel (signed).
		R16S,

		/// 32-bit single channel (integer).
		R32I,

		/// 32-bit single channel (unsigned).
		R32U,

		/// 32-bit single channel (float).
		R32F,

		/// 8-bit two channel.
		RG8,

		/// 8-bit two channel (integer).
		RG8I,

		/// 8-bit two channel (unsigned).
		RG8U,

		/// 8-bit two channel (signed).
		RG8S,

		/// 16-bit two channel.
		RG16,

		/// 16-bit two channel (integer).
		RG16I,

		/// 16-bit two channel (unsigned).
		RG16U,

		/// 16-bit two channel (float).
		RG16F,

		/// 16-bit two channel (signed).
		RG16S,

		/// 32-bit two channel (integer).
		RG32I,

		/// 32-bit two channel (unsigned).
		RG32U,

		/// 32-bit two channel (float).
		RG32F,

		/// 8-bit three channel.
		RGB8,

		/// 8-bit three channel (integer).
		RGB8I,

		/// 8-bit three channel (unsigned).
		RGB8U,

		/// 8-bit three channel (signed).
		RGB8S,

		/// 9-bit three channel floating point with shared 5-bit exponent.
		RGB9E5F,

		/// 8-bit BGRA color.
		BGRA8,

		/// 8-bit RGBA color.
		RGBA8,

		/// 8-bit RGBA color (integer).
		RGBA8I,

		/// 8-bit RGBA color (unsigned).
		RGBA8U,

		/// 8-bit RGBA color (signed).
		RGBA8S,

		/// 16-bit RGBA color.
		RGBA16,

		/// 16-bit RGBA color (integer).
		RGBA16I,

		/// 16-bit RGBA color (unsigned).
		RGBA16U,

		/// 16-bit RGBA color (float).
		RGBA16F,

		/// 16-bit RGBA color (signed).
		RGBA16S,

		/// 32-bit RGBA color (integer).
		RGBA32I,

		/// 32-bit RGBA color (unsigned).
		RGBA32U,

		/// 32-bit RGBA color (float).
		RGBA32F,

		/// 5-6-6 color.
		R5G6B5,

		/// 4-bit RGBA color.
		RGBA4,

		/// 5-bit RGB color with 1-bit alpha.
		RGB5A1,

		/// 10-bit RGB color with 2-bit alpha.
		RGB10A2,

		/// 11-11-10 color (float).
		RG11B10F,

		/// Unknown depth format.
		UnknownDepth,

		/// 16-bit depth.
		D16,

		/// 24-bit depth.
		D24,

		/// 24-bit depth, 8-bit stencil.
		D24S8,

		/// 32-bit depth.
		D32,

		/// 16-bit depth (float).
		D16F,

		/// 24-bit depth (float).
		D24F,

		/// 32-bit depth (float).
		D32F,

		/// 8-bit stencil.
		D0S8,

		/// The count of all texture formats. Not a valid value.
		Count
	};
};

struct UniformType
{
	enum Enum
	{
		/// Texture sampler.
		Sampler,

		/// 4D vector.
		Vector4 = 2,

		/// 3x3 Matrix.
		Matrix3x3,

		///4x4 Matrix.
		Matrix4x4
	};
};
}
