#version 400

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

struct Material{
   float kd;
   float ks;
   float ka;
   float alpha;
};

struct Light{
   vec3 pos;
   vec3 col;
};


uniform Material material;
uniform Light light;

in vec3 nEye;
in vec3 pEye;
out vec4 FragColor;

void main()
{


   // v is eyepos - current vertex
   // L is lightposition - eyeposition, normalized
   vec3 L = normalize(light.pos + -1*pEye);
   vec3 r = 2*(dot(L,nEye))*nEye-L;
   vec3 s = normalize(vec3(light.pos + -1*pEye));
   vec3 v = normalize(-1*pEye);


   // ambient
   vec3 ia = material.ka * light.col;
   // diffuse
   vec3 id = material.kd * max((dot(L,nEye)),0) * light.col * vec3(0.4,0.80,0.4);
   // specular
   vec3 is = material.ks * light.col *pow(max((dot(v,r)),0),material.alpha);
   vec3 col = vec3(ia+id+is);

   FragColor = vec4(col, 1.0f);

}
