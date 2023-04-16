#version 400

uniform sampler2D diffuseTexture; 
in vec2 uv;
in vec3 color;

out vec4 FragColor;

void main(){
vec3 c = color * texture(diffuseTexture, uv*10.0f).xyz; 
FragColor= vec4(c, 1.0);
}