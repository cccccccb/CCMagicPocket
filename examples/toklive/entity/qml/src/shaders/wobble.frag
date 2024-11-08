// #version 440

// layout(location = 0) in vec2 qt_TexCoord0;
// layout(location = 0) out vec4 fragColor;

// layout(binding = 1) uniform sampler2D source;

// layout(std140, binding = 0) uniform buf {
//     mat4 qt_Matrix;
//     float qt_Opacity;
//     float amplitude;
//     float frequency;
//     float time;
// } ubuf;

// void main()
// {
//     vec2 p = sin(ubuf.time + ubuf.frequency * qt_TexCoord0);
//     fragColor = texture(source, qt_TexCoord0 + ubuf.amplitude * vec2(p.y, -p.x)) * ubuf.qt_Opacity;
// }

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
    float width;
    float height;
    vec4 edgeColor;
} ubuf;

const float N = 2.0; // grid ratio
float gridTexture( in vec2 p )
{
    // coordinates
    vec2 i = step( fract(p), vec2(1.0/N) );
    //pattern
    //return (1.0-i.x)*(1.0-i.y);   // grid (N=10)
    
    // other possible patterns are these
    //return 1.0-i.x*i.y;           // squares (N=4)
    return 1.0-i.x-i.y+2.0*i.x*i.y; // checker (N=2)
}

void main()
{
    vec2 uv = qt_TexCoord0;
    vec2 coord = uv * vec2(ubuf.width, ubuf.height);

    // color textures
    vec4 clrA = texture(source, uv);
    
    // background grid
    vec4 clrBG = 0.2 * vec4(1., 1., 1., 1.) * gridTexture(coord.xy/vec2(ubuf.width, ubuf.width) * vec2(5., 5.)) + 0.6;
    
    // set this to fade the alpha (0-1)
    float t = ubuf.time;
	// set these to increase/decrease the edge width
    float edge_width_start = 0.15; // width at the start of the dissolve (alpha = 1)
    float edge_width_end = 0.05; // width at the end of the dissolve (alpha = 0)
    
    float edge_width = mix(edge_width_start, edge_width_end, smoothstep(0., 1., t)); // 
    
    // increase the alpha range by the edge width so we are not left with only glowy edges 
    float myAlpha = mix(0. - edge_width, 1., t); 
    
    // fade mask uv
    vec2 uv_mask = uv;
    
    // fade mask texture
    // use a linear texture that has values between 0-1
    vec4 alphaTex = texture(noiseSrc, uv_mask * ubuf.maskTile);

    // alpha mask (1-bit)
    float a = step(alphaTex.r, myAlpha);

    // edge mask which is a slightly progressed version of the alpha
    // this mask doesn't need to be 1 bit as it will just be added to the color
    float edge = smoothstep(alphaTex.r - edge_width, alphaTex.r, myAlpha);
    vec4 edgeColor = ubuf.edgeColor * edge * 10.;
    
    // add edge color to the color
    clrA += edgeColor;

    fragColor = mix(clrA, clrBG, a);
}

