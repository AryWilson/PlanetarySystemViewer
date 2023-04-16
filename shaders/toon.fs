#version 400

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

struct Material{
   float kd;
   float ks;
   float ka;
   vec3 col;
   float alpha;
};

struct Light{
   vec3 pos;
   vec3 col;
};

in vec3 nEye;
in vec3 pEye;

uniform Material material;
uniform Light light;

const int levels = 3;
const float sf = 1.0f/levels;

out vec4 FragColor;


void main()
{
    vec3 s = normalize(light.pos - pEye); // vPos
    float cos = max(0.0, dot(s, nEye)); // vNormals
    float t = material.kd;
    vec3 d = material.col* floor(cos * levels) * (sf);
    FragColor = vec4(light.col * (material.ka + d),1.0);

}

