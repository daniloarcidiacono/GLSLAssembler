// MODULE BEGIN: resources/shaders/diamond/c.glsl
vec2 c_func(inout float seed) {
    return vec2(2. * seed, seed);
}

// MODULE BEGIN: resources/shaders/diamond/a.glsl
// #include <c.glsl>

uint base_hash(uvec2 p) {
    p = 1103515245U*((p >> 1U)^(p.yx));
    uint h32 = 1103515245U*((p.x)^(p.y>>3U));
    return h32^(h32 >> 16);
}

// MODULE BEGIN: resources/shaders/diamond/b.glsl
// #include <c.glsl>

vec2 hash2(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=.1,seed+=.1)));
    uvec2 rz = uvec2(n, n*48271U);
    return vec2(rz.xy & uvec2(0x7fffffffU))/float(0x7fffffff);
}

// MODULE BEGIN: resources/shaders/diamond/main.glsl
// #include <a.glsl>
// #include <b.glsl>

out vec4 out_Color;
void main(void) {
    out_Color = vec4(1, 0, 0, 1);
}
