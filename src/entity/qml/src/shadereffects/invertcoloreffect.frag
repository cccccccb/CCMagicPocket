#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float adjustX;
    float adjustY;
};
layout(binding = 1) uniform sampler2D source;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 invertColor(vec3 c)
{
    if (c.b == 0.0) {
        c.g = 0.0;
        c.b = 1.0;
        return c;
    }

    if (c.g == 0.0) {
        c.g = 1.0;
        c.b = 0.0;
        return c;
    }

    c = mod(c + vec3(0.5, 0.5, 0.5), vec3(1.0, 1.0, 1.0));
    return c;
}

float distanceEllipse(vec2 pos, vec2 center, float a, float b)
{
    vec2 offset = pos - center;
    return (offset.x * offset.x) / (a * a) + (offset.y * offset.y) / (b * b);
}

void main() {
    vec4 p = texture(source, qt_TexCoord0);
    vec3 g = rgb2hsv(p.xyz);
    vec3 c = hsv2rgb(invertColor(g));

    float d = 0.0;
    float edge = 0.01;
    if (qt_TexCoord0.x < adjustX && qt_TexCoord0.y < adjustY) {
        d = distanceEllipse(qt_TexCoord0, vec2(adjustX, adjustY), adjustX, adjustY);
        edge = adjustX < 0.5 ? fwidth(qt_TexCoord0.y - vec2(adjustX, adjustY).y) / 1.4 : fwidth(qt_TexCoord0.x - vec2(adjustX, adjustY).x) / 1.4;
    } else if (qt_TexCoord0.x > 1.0 - adjustX && qt_TexCoord0.y < adjustY) {
        d = distanceEllipse(qt_TexCoord0, vec2(1.0 - adjustX, adjustY), adjustX, adjustY);
        edge = adjustX < 0.5 ? fwidth(qt_TexCoord0.y - vec2(1.0 - adjustX, adjustY).y) / 1.4 : fwidth(qt_TexCoord0.x - vec2(1.0 - adjustX, adjustY).x) / 1.4;
    } else if (qt_TexCoord0.x < adjustX && qt_TexCoord0.y > 1.0 - adjustY) {
        d = distanceEllipse(qt_TexCoord0, vec2(adjustX, 1.0 - adjustY), adjustX, adjustY);
        edge = adjustX < 0.5 ? fwidth(qt_TexCoord0.y - vec2(adjustX, 1.0 - adjustY).y) / 1.4 : fwidth(qt_TexCoord0.x - vec2(adjustX, 1.0 - adjustY).x) / 1.4;
    } else if (qt_TexCoord0.x > 1.0 - adjustX && qt_TexCoord0.y > 1.0 - adjustY) {
        d = distanceEllipse(qt_TexCoord0, vec2(1.0 - adjustX, 1.0 - adjustY), adjustX, adjustY);
        edge = adjustX < 0.5 ? fwidth(qt_TexCoord0.y - vec2(1.0 - adjustX, 1.0 - adjustY).y) / 1.4 : fwidth(qt_TexCoord0.x - vec2(1.0 - adjustX, 1.0 - adjustY).x) / 1.4;
    }

    fragColor = vec4(c, p.w) * step(d, 1.0) * smoothstep(d, 1.0 + edge, 1.0) * qt_Opacity;
}
