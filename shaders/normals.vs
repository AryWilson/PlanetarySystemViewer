#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vUV;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

out vec3 normCol;

void main()
{   
   normCol = vNormal + vec3(1,1,1);
   normCol = normCol * vec3(0.5f,0.5f,0.5f);
   gl_Position = MVP * vec4(vPos, 1.0);
   
   float nx = (vNormal.x + 1)/2.0f;
   float ny = (vNormal.y + 1)/2.0f;
   float nz = (vNormal.z + 1)/2.0f;
   normCol = vec3(nx,ny,nz);
}
