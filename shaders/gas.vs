#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 pEye;
out vec3 nEye;
out vec2 uv;
out vec3 lightDir;
uniform mat4 ModelViewMatrix;
uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjMatrix;
uniform mat4 MVP;
uniform float time;

struct Light{
   vec3 pos;
   vec3 col;
};


uniform Light light;

float rand (vec2 uv){
   return fract(sin(dot(uv, vec2(1, 1))) * 43758.5453);
}

void main()
{
 uv = VertexTexCoord;
 lightDir = (ViewMatrix * vec4(light.pos, 1.0)).xyz;
 nEye = normalize( NormalMatrix * VertexNormal);
 pEye = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );

 vec3 v = normalize(VertexNormal);
 vec2 co = uv*sin(.001*time);
 vec3 pos = VertexPosition + v * rand(co) * .05;
 gl_Position = MVP * vec4(pos,1.0);
}