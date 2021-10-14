// MODULE BEGIN: resources/shaders/relative/utils/b.glsl
vec2 hash2(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=.1,seed+=.1)));
    uvec2 rz = uvec2(n, n*48271U);
    return vec2(rz.xy & uvec2(0x7fffffffU))/float(0x7fffffff);
}

// MODULE BEGIN: resources/shaders/relative/utils/nested/a.glsl
// #include "../b.glsl"

uint base_hash(uvec2 p) {
    p = 1103515245U*((p >> 1U)^(p.yx));
    uint h32 = 1103515245U*((p.x)^(p.y>>3U));
    return h32^(h32 >> 16);
}

// MODULE BEGIN: resources/shaders/relative/main.glsl
// #include <utils/nested/a.glsl>

out vec4 out_Color;
void main(void) {
    out_Color = vec4(1, 0, 0, 1);
}
