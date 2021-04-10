#include "SaffronPCH.h"

#include "Saffron/Rendering/RenderState.h"

namespace Se
{
const RenderState None(0);

const RenderState WriteR(0x0000000000000001);

const RenderState WriteG(0x0000000000000002);

const RenderState WriteB(0x0000000000000004);

const RenderState WriteA(0x0000000000000008);

const RenderState WriteZ(0x0000004000000000);

const RenderState WriteRGB(0x0000000000000001 | 0x0000000000000002 | 0x0000000000000004);

const RenderState DepthTestLess(0x0000000000000010);

const RenderState DepthTestLessEqual(0x0000000000000020);

const RenderState DepthTestEqual(0x0000000000000030);

const RenderState DepthTestGreaterEqual(0x0000000000000040);

const RenderState DepthTestGreater(0x0000000000000050);

const RenderState DepthTestNotEqual(0x0000000000000060);

const RenderState DepthTestNever(0x0000000000000070);

const RenderState DepthTestAlways(0x0000000000000080);

const RenderState BlendZero(0x0000000000001000);

const RenderState BlendOne(0x0000000000002000);

const RenderState BlendSourceColor(0x0000000000003000);

const RenderState BlendInverseSourceColor(0x0000000000004000);

const RenderState BlendSourceAlpha(0x0000000000005000);

const RenderState BlendInverseSourceAlpha(0x0000000000006000);

const RenderState BlendDestinationAlpha(0x0000000000007000);

const RenderState BlendInverseDestinationAlpha(0x0000000000008000);

const RenderState BlendDestinationColor(0x0000000000009000);

const RenderState BlendInverseDestinationColor(0x000000000000a000);

const RenderState BlendSourceAlphaSaturate(0x000000000000b000);

const RenderState BlendFactor(0x000000000000c000);

const RenderState BlendInverseFactor(0x000000000000d000);

const RenderState BlendEquationAdd(0x0000000000000000);

const RenderState BlendEquationSub(0x0000000010000000);

const RenderState BlendEquationReverseSub(0x0000000020000000);

const RenderState BlendEquationMin(0x0000000030000000);

const RenderState BlendEquationMax(0x0000000040000000);

const RenderState BlendIndependent(0x0000000400000000);

const RenderState BlendAlphaToCoverage(0x0000000800000000);

const RenderState NoCulling(0x0000000000000000);

const RenderState CullClockwise(0x0000001000000000);

const RenderState CullCounterclockwise(0x0000002000000000);

const RenderState PrimitiveTriangles(0x0000000000000000);

const RenderState PrimitiveTriangleStrip(0x0001000000000000);

const RenderState PrimitiveLines(0x0002000000000000);

const RenderState PrimitiveLineStrip(0x0003000000000000);

const RenderState PrimitivePoints(0x0004000000000000);

const RenderState Multisampling(0x1000000000000000);

const RenderState LineAA(0x2000000000000000);

const RenderState ConservativeRasterization(0x4000000000000000);

const RenderState Default(WriteRGB | WriteA | WriteZ | DepthTestLess | CullClockwise | Multisampling);

const RenderState BlendAdd(RenderState::BlendFunction(BlendOne, BlendOne));

const RenderState BlendAlpha(RenderState::BlendFunction(BlendSourceAlpha, BlendInverseSourceAlpha));

const RenderState BlendDarken(
	RenderState::BlendFunction(BlendOne, BlendOne) | RenderState::BlendEquation(BlendEquationMin));

const RenderState BlendLighten(
	RenderState::BlendFunction(BlendOne, BlendOne) | RenderState::BlendEquation(BlendEquationMax));

const RenderState BlendMultiply(RenderState::BlendFunction(BlendDestinationColor, BlendZero));

const RenderState BlendNormal(RenderState::BlendFunction(BlendOne, BlendInverseSourceAlpha));

const RenderState BlendScreen(RenderState::BlendFunction(BlendOne, BlendInverseSourceColor));

const RenderState BlendLinearBurn = RenderState::BlendFunction(BlendDestinationColor, BlendInverseDestinationColor) |
RenderState::BlendEquation(BlendEquationSub);
}


}
