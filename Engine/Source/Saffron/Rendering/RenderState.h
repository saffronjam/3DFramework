#pragma once

#include "Saffron/Base.h"

namespace Se
{
struct RenderState
{
	static constexpr int AlphaRefShift = 40;
	static constexpr int PointSizeShift = 52;
	static constexpr Uint64 AlphaRefMask = 0x0000ff0000000000;
	static constexpr Uint64 PointSizeMask = 0x0ff0000000000000;

	const Uint64 Value;

	/// No state bits set.    
	static const RenderState None;
	/// Enable writing the Red color channel to the framebuffer.    
	static const RenderState WriteR;
	/// Enable writing the Green color channel to the framebuffer.    
	static const RenderState WriteG;
	/// Enable writing the Blue color channel to the framebuffer.    
	static const RenderState WriteB;
	/// Enable writing alpha data to the framebuffer.    
	static const RenderState WriteA;
	/// Enable writing to the depth buffer.    
	static const RenderState WriteZ;
	/// Enable writing all three color channels to the framebuffer.    
	static const RenderState WriteRGB;
	/// Use a "less than" comparison to pass the depth test.    
	static const RenderState DepthTestLess;
	/// Use a "less than or equal to" comparison to pass the depth test.    
	static const RenderState DepthTestLessEqual;
	/// Pass the depth test if both Values are equal.    
	static const RenderState DepthTestEqual;
	/// Use a "greater than or equal to" comparison to pass the depth test.    
	static const RenderState DepthTestGreaterEqual;
	/// Use a "greater than" comparison to pass the depth test.    
	static const RenderState DepthTestGreater;
	/// Pass the depth test if both Values are not equal.
	static const RenderState DepthTestNotEqual;
	/// Never pass the depth test.
	static const RenderState DepthTestNever;
	/// Always pass the depth test.
	static const RenderState DepthTestAlways;
	/// Use a Value of 0 as an input to a blend equation.
	static const RenderState BlendZero;
	/// Use a Value of 1 as an input to a blend equation.
	static const RenderState BlendOne;
	/// Use the source pixel color as an input to a blend equation.
	static const RenderState BlendSourceColor;
	/// Use one minus the source pixel color as an input to a blend equation.
	static const RenderState BlendInverseSourceColor;
	/// Use the source pixel alpha as an input to a blend equation.
	static const RenderState BlendSourceAlpha;
	/// Use one minus the source pixel alpha as an input to a blend equation.
	static const RenderState BlendInverseSourceAlpha;
	/// Use the destination pixel alpha as an input to a blend equation.
	static const RenderState BlendDestinationAlpha;
	/// Use one minus the destination pixel alpha as an input to a blend equation.
	static const RenderState BlendInverseDestinationAlpha;
	/// Use the destination pixel color as an input to a blend equation.
	static const RenderState BlendDestinationColor;
	/// Use one minus the destination pixel color as an input to a blend equation.
	static const RenderState BlendInverseDestinationColor;
	/// Use the source pixel alpha (saturated) as an input to a blend equation.
	static const RenderState BlendSourceAlphaSaturate;
	/// Use an application supplied blending factor as an input to a blend equation.
	static const RenderState BlendFactor;
	/// Use one minus an application supplied blending factor as an input to a blend equation.
	static const RenderState BlendInverseFactor;
	/// Blend equation: A + B
	static const RenderState BlendEquationAdd;
	/// Blend equation: B - A
	static const RenderState BlendEquationSub;
	/// Blend equation: A - B
	static const RenderState BlendEquationReverseSub;
	/// Blend equation: min(a, b)
	static const RenderState BlendEquationMin;
	/// Blend equation: max(a, b)
	static const RenderState BlendEquationMax;
	/// Enable independent blending of simultaenous render targets.
	static const RenderState BlendIndependent;
	/// Enable alpha to coverage blending.
	static const RenderState BlendAlphaToCoverage;
	/// Don't perform culling of back faces.
	static const RenderState NoCulling;
	/// Perform culling of clockwise faces.
	static const RenderState CullClockwise;
	/// Perform culling of counter-clockwise faces.
	static const RenderState CullCounterclockwise;
	/// Primitive topology: triangle list.
	static const RenderState PrimitiveTriangles;
	/// Primitive topology: triangle strip.
	static const RenderState PrimitiveTriangleStrip;
	/// Primitive topology: line list.
	static const RenderState PrimitiveLines;
	/// Primitive topology: line strip.
	static const RenderState PrimitiveLineStrip;
	/// Primitive topology: point list.
	static const RenderState PrimitivePoints;
	/// Enable multisampling.
	static const RenderState Multisampling;
	/// Enable line antialiasing.
	static const RenderState LineAA;
	/// Enable conservative rasterization.
	static const RenderState ConservativeRasterization;
	/// Provides a set of sane defaults.
	static const RenderState Default;
	/// Predefined blend effect: additive blending.
	static const RenderState BlendAdd;
	/// Predefined blend effect: alpha blending.
	static const RenderState BlendAlpha;
	/// Predefined blend effect: "darken" blending.
	static const RenderState BlendDarken;
	/// Predefined blend effect: "lighten" blending.
	static const RenderState BlendLighten;
	/// Predefined blend effect: multiplicative blending.
	static const RenderState BlendMultiply;
	/// Predefined blend effect: normal blending based on alpha.
	static const RenderState BlendNormal;
	/// Predefined blend effect: "screen" blending.
	static const RenderState BlendScreen;
	/// Predefined blend effect: "linear burn" blending.
	static const RenderState BlendLinearBurn;


	explicit RenderState(Uint64 value) :
		Value(value)
	{
	}

	/// Encodes an alpha reference Value in a render state.

	/// <param name="alpha">The alpha reference Value.</param>
	/// <returns>The encoded render state.</returns>
	static RenderState AlphaRef(Uint8 alpha)
	{
		return RenderState(static_cast<Uint64>(alpha) << AlphaRefShift & AlphaRefMask);
	}

	/// Encodes a point size Value in a render state.

	/// <param name="size">The point size.</param>
	/// <returns>The encoded render state.</returns>
	static RenderState PointSize(Uint8 size)
	{
		return RenderState(static_cast<Uint64>(size) << PointSizeShift & PointSizeMask);
	}

	/// Builds a render state for a blend function.

	/// <param name="source">The source blend operation.</param>
	/// <param name="destination">The destination blend operation.</param>
	/// <returns>The render state for the blend function.</returns>
	static RenderState BlendFunction(RenderState source, RenderState destination)
	{
		return BlendFunction(source, destination, source, destination);
	}

	/// Builds a render state for a blend function.

	/// <param name="sourceColor">The source color blend operation.</param>
	/// <param name="destinationColor">The destination color blend operation.</param>
	/// <param name="sourceAlpha">The source alpha blend operation.</param>
	/// <param name="destinationAlpha">The destination alpha blend operation.</param>
	/// <returns>
	/// The render state for the blend function.
	/// </returns>
	static RenderState BlendFunction(RenderState sourceColor, RenderState destinationColor, RenderState sourceAlpha,
	                                 RenderState destinationAlpha)
	{
		return (sourceColor | (destinationColor << 4)) | ((sourceAlpha | (destinationAlpha << 4)) << 8);
	}

	/// Builds a render state for a blend equation.

	/// <param name="equation">The equation.</param>
	/// <returns>
	/// The render state for the blend equation.
	/// </returns>
	static RenderState BlendEquation(RenderState equation)
	{
		return BlendEquation(equation, equation);
	}


	/// Builds a render state for a blend equation.

	/// <param name="sourceEquation">The source equation.</param>
	/// <param name="alphaEquation">The alpha equation.</param>
	/// <returns>
	/// The render state for the blend equation.
	/// </returns>
	static RenderState BlendEquation(RenderState sourceEquation, RenderState alphaEquation)
	{
		return sourceEquation | (alphaEquation << 3);
	}

	bool operator ==(RenderState right)
	{
		return Value == right.Value;
	}

	bool operator !=(RenderState right)
	{
		return Value != right.Value;
	}

	operator Uint64()
	{
		return Value;
	}

	RenderState operator |(RenderState right) const
	{
		return RenderState(Value | right.Value);
	}

	RenderState operator &(RenderState right) const
	{
		return RenderState(Value & right.Value);
	}

	RenderState operator ~() const
	{
		return RenderState(~Value);
	}

	RenderState operator <<(int amount) const
	{
		return RenderState(Value << amount);
	}

	RenderState operator >>(int amount) const
	{
		return RenderState(Value >> amount);
	}
};

