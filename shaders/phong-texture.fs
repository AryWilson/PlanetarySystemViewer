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

uniform sampler2D diffuseTexture; 

in vec3 pEye;
in vec3 nEye;
in vec2 uv;

out vec4 FragColor;
// void main(){
// vec3 c = color * texture(diffuseTexture, uv*10.0f).xyz; // float determines wrap, negative inverts
// FragColor= vec4(c, 1.0);
// }

void main() {
   vec4 texColor = texture( diffuseTexture, uv );
   vec3 n = normalize( nEye );
   vec3 s = normalize( vec3(light.pos) - pEye );
   vec3 v = normalize(vec3(-pEye));
   vec3 r = reflect( -s, n );
   // ambient
   vec3 ia = material.ka * light.col;
   // diffuse
   vec3 id = material.kd * max( dot(s, n), 0.0 ) * light.col;
   // specular
   vec3 is = material.ks * light.col * pow( max( dot(r,v), 0.0 ),material.alpha);

    FragColor = vec4((ia+id),1.0) * texColor + vec4(is,1.0);
}
