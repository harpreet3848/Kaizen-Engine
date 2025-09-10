#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0)uniform sampler2D screenTexture;
layout (binding = 1)uniform sampler2D bloomBlur;

const float offset = 1.0 / 300.0;  

uniform int   uEffect;  
uniform float exposure;
uniform bool useBloom;

uniform float uEdgeThreshold;

/* ---- Helpers ---- */
vec3 fetch(vec2 uv) { return texture(screenTexture, uv).rgb; }

vec2 texelSize()
{
    return 1.0 / vec2(textureSize(screenTexture, 0));
}

void gather3x3(vec2 uv, out vec3 s[9]) {
    vec2 o = texelSize();
    vec2 offs[9] = vec2[](
        vec2(-o.x,  o.y), vec2(0,  o.y), vec2( o.x,  o.y),
        vec2(-o.x,  0  ), vec2(0,  0  ), vec2( o.x,  0  ),
        vec2(-o.x, -o.y), vec2(0, -o.y), vec2( o.x, -o.y)
    );
    for (int i=0;i<9;i++) 
        s[i] = fetch(uv + offs[i]);
}
/* ---- Effects ---- */

vec3 effectBoxBlur3(vec2 uv) {
    vec3 s[9]; gather3x3(uv, s);
    vec3 sum = vec3(0);
    for (int i=0;i<9;i++) sum += s[i];
    return sum / 9.0;
}

vec3 effectGaussian3(vec2 uv) {
    vec3 s[9]; gather3x3(uv, s);
    float w[9] = float[](
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    );
    vec3 sum = vec3(0);
    float W = 0.0;
    for (int i=0;i<9;i++){ sum += s[i]*w[i]; W += w[i]; }
    return sum / W;
}

vec3 effectSobel(vec2 uv) {
    vec3 s[9]; gather3x3(uv, s);
    float l[9];
    for (int i=0;i<9;i++) l[i] = dot(s[i], vec3(0.299, 0.587, 0.114));
    float gx =  -l[0] + l[2] - 2.0*l[3] + 2.0*l[5] - l[6] + l[8];
    float gy =  -l[0] - 2.0*l[1] - l[2] + l[6] + 2.0*l[7] + l[8];
    float mag = sqrt(gx*gx + gy*gy);
    float edge = mag > uEdgeThreshold ? 1.0 : 0.0;
    return vec3(edge);
}

void main()
{

  vec3 col;
    switch (uEffect) {
        case 1:  col = effectBoxBlur3(TexCoords);  break;
        case 2:  col = effectGaussian3(TexCoords); break;
        case 3:  col = effectSobel(TexCoords);     break;
        default: col = fetch(TexCoords);           break;
    }

   //FragColor = vec4(col, 1.0);

    const float gamma = 2.2;
    //vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
  
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    if(useBloom)
        col += bloomColor;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-col * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);
} 