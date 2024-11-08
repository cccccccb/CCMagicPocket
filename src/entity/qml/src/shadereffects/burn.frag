#version 440
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D source;
layout(binding = 2) uniform sampler2D noiseSrc;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float time;
    float maskTile;
    vec4 edgeColor;
} ubuf;

void main()
{
    vec2 uv = qt_TexCoord0;
    vec4 clrA = texture(source, uv);
    vec4 clrBG = vec4(0., 0., 0., 0.);

    float t = ubuf.time;
    float edge_width_start = 0.15;
    float edge_width_end = 0.05;
    float edge_width = mix(edge_width_start, edge_width_end, smoothstep(0., 1., t));
    
    float myAlpha = mix(0. - edge_width, 1., t);
    vec2 uv_mask = uv;
    
    vec4 alphaTex = texture(noiseSrc, uv_mask * ubuf.maskTile);
    float a = step(alphaTex.r, myAlpha);
    float edge = smoothstep(alphaTex.r - edge_width, alphaTex.r, myAlpha);
    vec4 edgeColor = ubuf.edgeColor * edge * 10.;
    
    clrA += edgeColor;
    fragColor = mix(clrA, clrBG, a);
}
