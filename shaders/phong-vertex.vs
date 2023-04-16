#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormals;
layout (location = 2) in vec2 vTextureCoords;

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

uniform Material material;
uniform Light light;

out vec3 col;

void main()
{
   // get pos and normal in eye space
   vec3 nEye = normalize(NormalMatrix * vNormals);
   vec3 pEye = (ModelViewMatrix * vec4(vPos,1.0f)).xyz;

   // v is eyepos - current vertex
   // L is lightposition - eyeposition, normalized
   vec3 L = normalize(light.pos + -1*pEye);
   vec3 r = 2*(dot(L,nEye))*nEye-L;
   vec3 s = normalize(vec3(light.pos + -1*pEye));
   vec3 v = normalize(-1*pEye);


   // ambient
   vec3 ia = material.ka * light.col;
   // diffuse
   vec3 id = material.kd * max((dot(L,nEye)),0) * light.col * material.col;
   // specular
   vec3 is = material.ks * light.col *pow(max((dot(v,r)),0),material.alpha);
   col = vec3(ia+id+is);

   gl_Position = MVP * vec4(vPos, 1.0f);
}
