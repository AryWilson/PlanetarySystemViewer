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
uniform time;

struct Light{
   vec3 pos;
   vec3 col;
};


uniform Light light;


void main()
{
 uv = VertexTexCoord;
 lightDir = (ViewMatrix * vec4(light.pos, 1.0)).xyz;
 nEye = normalize( NormalMatrix * VertexNormal);
 pEye = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );
 vec3 v = cross(VertexNormal, vec3(0, 1, 0));
 vec3 pos = VertexPosition + v * sin(time) * .25;
 gl_Position = MVP * vec4(pos,1.0);
}