#pragma once

struct Vec1Fp {
    float x;
};

struct Vec2Fp {
    float x, y;
};

struct Vec3Fp {
    union {
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        };
    };
};

struct Vec4Fp {
    float x, y, z, w;
};

struct VerticesFp {
    Vec3Fp position;
    Vec3Fp color;
    Vec2Fp textCoords;
    float  texIndex;
};
