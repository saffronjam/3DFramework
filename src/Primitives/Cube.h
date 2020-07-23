#pragma once

#include <optional>
#include <array>

#include "IndexedTriangleList.h"

class Cube
{
public:
    static IndexedTriangleList Make(std::optional<VertexElementLayout> layout = {})
    {
        using Type = ElementType;

        if (!layout)
        {
            layout = std::make_optional<VertexElementLayout>();
            layout->Append(Type::Position3D);
        }

        constexpr float side = 1.0f / 2.0f;

        RawVertexBuffer rvb(std::move(*layout), 8);
        rvb[0].Attr<Type::Position3D>() = {-side, -side, -side};
        rvb[1].Attr<Type::Position3D>() = {side, -side, -side};
        rvb[2].Attr<Type::Position3D>() = {-side, side, -side};
        rvb[3].Attr<Type::Position3D>() = {side, side, -side};
        rvb[4].Attr<Type::Position3D>() = {-side, -side, side};
        rvb[5].Attr<Type::Position3D>() = {side, -side, side};
        rvb[6].Attr<Type::Position3D>() = {-side, side, side};
        rvb[7].Attr<Type::Position3D>() = {side, side, side};

        return {
                std::move(rvb), {
                        0, 2, 1, 2, 3, 1,
                        1, 3, 5, 3, 7, 5,
                        2, 6, 3, 3, 6, 7,
                        4, 5, 7, 4, 7, 6,
                        0, 4, 2, 2, 4, 6,
                        0, 1, 4, 1, 5, 4
                }
        };
    }
    static IndexedTriangleList MakeIndependent(const VertexElementLayout &layout)
    {
        using Type = ElementType;

        constexpr float side = 1.0f / 2.0f;

        RawVertexBuffer rvb(layout, 24);
        rvb[0].Attr<Type::Position3D>() = {-side, -side, -side};// 0 near side
        rvb[1].Attr<Type::Position3D>() = {side, -side, -side};// 1
        rvb[2].Attr<Type::Position3D>() = {-side, side, -side};// 2
        rvb[3].Attr<Type::Position3D>() = {side, side, -side};// 3
        rvb[4].Attr<Type::Position3D>() = {-side, -side, side};// 4 far side
        rvb[5].Attr<Type::Position3D>() = {side, -side, side};// 5
        rvb[6].Attr<Type::Position3D>() = {-side, side, side};// 6
        rvb[7].Attr<Type::Position3D>() = {side, side, side};// 7
        rvb[8].Attr<Type::Position3D>() = {-side, -side, -side};// 8 left side
        rvb[9].Attr<Type::Position3D>() = {-side, side, -side};// 9
        rvb[10].Attr<Type::Position3D>() = {-side, -side, side};// 10
        rvb[11].Attr<Type::Position3D>() = {-side, side, side};// 11
        rvb[12].Attr<Type::Position3D>() = {side, -side, -side};// 12 right side
        rvb[13].Attr<Type::Position3D>() = {side, side, -side};// 13
        rvb[14].Attr<Type::Position3D>() = {side, -side, side};// 14
        rvb[15].Attr<Type::Position3D>() = {side, side, side};// 15
        rvb[16].Attr<Type::Position3D>() = {-side, -side, -side};// 16 bottom side
        rvb[17].Attr<Type::Position3D>() = {side, -side, -side};// 17
        rvb[18].Attr<Type::Position3D>() = {-side, -side, side};// 18
        rvb[19].Attr<Type::Position3D>() = {side, -side, side};// 19
        rvb[20].Attr<Type::Position3D>() = {-side, side, -side};// 20 top side
        rvb[21].Attr<Type::Position3D>() = {side, side, -side};// 21
        rvb[22].Attr<Type::Position3D>() = {-side, side, side};// 22
        rvb[23].Attr<Type::Position3D>() = {side, side, side};// 23

        return {
                std::move(rvb), {
                        0, 2, 1, 2, 3, 1,
                        4, 5, 7, 4, 7, 6,
                        8, 10, 9, 10, 11, 9,
                        12, 13, 15, 12, 15, 14,
                        16, 17, 18, 18, 17, 19,
                        20, 23, 21, 20, 22, 23
                }
        };
    }
    static IndexedTriangleList MakeIndependentTextured()
    {
        using Type = ElementType;

        VertexElementLayout layout;
        layout.Append(Type::Position3D).Append(Type::Normal).Append(Type::Texture2D).Append(Type::Normal);

        auto itl = MakeIndependent(layout);
        auto &rvb = itl.rawVertexBuffer;

        rvb[0].Attr<Type::Texture2D>() = {0.0f, 0.0f};
        rvb[1].Attr<Type::Texture2D>() = {1.0f, 0.0f};
        rvb[2].Attr<Type::Texture2D>() = {0.0f, 1.0f};
        rvb[3].Attr<Type::Texture2D>() = {1.0f, 1.0f};
        rvb[4].Attr<Type::Texture2D>() = {0.0f, 0.0f};
        rvb[5].Attr<Type::Texture2D>() = {1.0f, 0.0f};
        rvb[6].Attr<Type::Texture2D>() = {0.0f, 1.0f};
        rvb[7].Attr<Type::Texture2D>() = {1.0f, 1.0f};
        rvb[8].Attr<Type::Texture2D>() = {0.0f, 0.0f};
        rvb[9].Attr<Type::Texture2D>() = {1.0f, 0.0f};
        rvb[10].Attr<Type::Texture2D>() = {0.0f, 1.0f};
        rvb[11].Attr<Type::Texture2D>() = {1.0f, 1.0f};
        rvb[12].Attr<Type::Texture2D>() = {0.0f, 0.0f};
        rvb[13].Attr<Type::Texture2D>() = {1.0f, 0.0f};
        rvb[14].Attr<Type::Texture2D>() = {0.0f, 1.0f};
        rvb[15].Attr<Type::Texture2D>() = {1.0f, 1.0f};
        rvb[16].Attr<Type::Texture2D>() = {0.0f, 0.0f};
        rvb[17].Attr<Type::Texture2D>() = {1.0f, 0.0f};
        rvb[18].Attr<Type::Texture2D>() = {0.0f, 1.0f};
        rvb[19].Attr<Type::Texture2D>() = {1.0f, 1.0f};
        rvb[20].Attr<Type::Texture2D>() = {0.0f, 0.0f};
        rvb[21].Attr<Type::Texture2D>() = {1.0f, 0.0f};
        rvb[22].Attr<Type::Texture2D>() = {0.0f, 1.0f};
        rvb[23].Attr<Type::Texture2D>() = {1.0f, 1.0f};

        rvb[0].Attr<Type::Normal>() = {0.0f, 0.0f, -1.0f};// 0 near side
        rvb[1].Attr<Type::Normal>() = {0.0f, 0.0f, -1.0f};// 1
        rvb[2].Attr<Type::Normal>() = {0.0f, 0.0f, -1.0f};// 2
        rvb[3].Attr<Type::Normal>() = {0.0f, 0.0f, -1.0f};// 3
        rvb[4].Attr<Type::Normal>() = {0.0f, 0.0f, 1.0f}; // 4 far side
        rvb[5].Attr<Type::Normal>() = {0.0f, 0.0f, 1.0f}; // 5
        rvb[6].Attr<Type::Normal>() = {0.0f, 0.0f, 1.0f}; // 6
        rvb[7].Attr<Type::Normal>() = {0.0f, 0.0f, 1.0f}; // 7
        rvb[8].Attr<Type::Normal>() = {0.0f, 0.0f, 1.0f}; // 8 left side
        rvb[9].Attr<Type::Normal>() = {-1.0f, 0.0f, 0.0f};// 9
        rvb[10].Attr<Type::Normal>() = {-1.0f, 0.0f, 0.0f};// 10
        rvb[11].Attr<Type::Normal>() = {-1.0f, 0.0f, 0.0f};// 11
        rvb[12].Attr<Type::Normal>() = {1.0f, 0.0f, 0.0f};// 12 right side
        rvb[13].Attr<Type::Normal>() = {1.0f, 0.0f, 0.0f};// 13
        rvb[14].Attr<Type::Normal>() = {1.0f, 0.0f, 0.0f};// 14
        rvb[15].Attr<Type::Normal>() = {1.0f, 0.0f, 0.0f};// 15
        rvb[16].Attr<Type::Normal>() = {0.0f, -1.0f, 0.0f};// 16 bottom side
        rvb[17].Attr<Type::Normal>() = {0.0f, -1.0f, 0.0f};// 17
        rvb[18].Attr<Type::Normal>() = {0.0f, -1.0f, 0.0f};// 18
        rvb[19].Attr<Type::Normal>() = {0.0f, -1.0f, 0.0f};// 19
        rvb[20].Attr<Type::Normal>() = {0.0f, 1.0f, 0.0f};// 20 top side
        rvb[21].Attr<Type::Normal>() = {0.0f, 1.0f, 0.0f};// 21
        rvb[22].Attr<Type::Normal>() = {0.0f, 1.0f, 0.0f};// 22
        rvb[23].Attr<Type::Normal>() = {0.0f, 1.0f, 0.0f};// 23

        return itl;
    }
};