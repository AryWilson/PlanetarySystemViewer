#version 400
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;


struct Light{
   vec3 pos;
   vec3 col;
};
uniform Light light;

uniform mat4 ModelViewMatrix;
uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjMatrix;
uniform mat4 MVP;

out vec3 ViewDir;
out vec2 uv;
out vec3 lightDir;
void main()
{
 // Transform normal and tangent to eye space
 vec3 norm = normalize(NormalMatrix * VertexNormal);
 vec3 tempTan = vec3(VertexNormal.y,VertexNormal.z, VertexNormal.x);
 if(VertexNormal.x == VertexNormal.y && VertexNormal.x == VertexNormal.z){
   tempTan = vec3(1,0,0);
 }
 vec3 tang = normalize(NormalMatrix * tempTan); //need tangen from external

 // Compute the binormal
 vec3 binormal = normalize( cross( norm, tang ) ) * VertexTangent.w;
 tang = normalize( cross( binormal, norm) ) * VertexTangent.w;

 // Matrix for transformation to tangent space
 mat3 toObjectLocal = mat3(
 tang.x, binormal.x, norm.x,
 tang.y, binormal.y, norm.y,
 tang.z, binormal.z, norm.z ) ;

 // Get the position in eye coordinates
 vec3 pos = vec3( ModelViewMatrix *
 vec4(VertexPosition,1.0) );

 // Transform light dir. and view dir. to tangent space
 lightDir = normalize( toObjectLocal * (light.pos - pos) );

 ViewDir = toObjectLocal * normalize(-pos);

 // Pass along the texture coordinate
 uv = VertexTexCoord;
 gl_Position = MVP * vec4(VertexPosition,1.0);
}