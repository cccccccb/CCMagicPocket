#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D source;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float time;
} ubuf;

vec2 hash2(vec2 p) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)))) * 43758.5453);
}

void voronoi(vec2 uv, float time, out float d1, out float d2, out vec2 centerPos) {
    vec2 iuv = floor(uv);
    vec2 fuv = fract(uv);

    d1 = d2 = 1e3;
    vec2 closestCenter;

    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            vec2 cell = iuv + vec2(x, y);
            vec2 rand = hash2(cell);

            float gravity = 0.8;
            float initialSpeedY = 0.5 + rand.x * 2.0;
            float horizontalSpeed = (rand.y - 0.5) * 0.4;

            vec2 offset = vec2(
                horizontalSpeed * time,
                -(initialSpeedY * time + 0.5 * gravity * time * time)
            );

            vec2 featurePoint = cell + 0.5 + (rand - 0.5) * 0.4 + offset;
            vec2 diff = featurePoint - uv;
            float dist = length(diff);

            if(dist < d1) {
                d2 = d1;
                d1 = dist;
                closestCenter = featurePoint;
            } else if(dist < d2) {
                d2 = dist;
            }
        }
    }
    centerPos = closestCenter;
}

void main() {
    vec2 uv = qt_TexCoord0;
    float aspectRatio = 1.0; // 可按实际宽高传入

    uv.y *= aspectRatio;

    float gridDensity = 8.0;
    vec2 scaledUV = uv * gridDensity;

    float d1, d2;
    vec2 centerPos;
    voronoi(scaledUV, ubuf.time, d1, d2, centerPos);

    float edgeWidth = 0.05;
    float edge = smoothstep(0.0, edgeWidth, d2 - d1);

    float visibility = step(centerPos.y / gridDensity, aspectRatio + 0.5);

    vec3 normal = normalize(vec3(dFdx(d1), dFdy(d1), 1.0));

    vec2 refractedUV = qt_TexCoord0 + normal.xy * 0.02;
    vec3 bgColor = texture(source, refractedUV).rgb;

    vec3 viewDir = vec3(0.0, 0.0, 1.0);
    float fresnel = pow(1.0 - dot(normal, viewDir), 5.0);
    vec3 reflColor = vec3(0.8) * (1.0 - fresnel);

    vec3 glassColor = mix(bgColor, reflColor, fresnel);
    vec3 finalColor = mix(vec3(0.0), glassColor, edge);

    vec3 lightDir = normalize(vec3(0.5, 0.5, 1.0));
    float spec = pow(max(dot(reflect(-lightDir, normal), viewDir), 0.0), 32.0);
    finalColor += vec3(0.4) * spec;

    finalColor *= visibility;

    fragColor = vec4(finalColor, 1.0) * ubuf.qt_Opacity;
}
