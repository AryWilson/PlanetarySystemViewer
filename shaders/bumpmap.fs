#version 400

in vec3 ViewDir;
in vec3 pEye;
in vec3 nEye;
in vec2 uv;
in vec3 lightDir;

uniform sampler2D diffuseTexture; 
uniform sampler2D normalMapTexture;

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

layout( location = 0 ) out vec4 FragColor;

vec3 phongModel( vec3 norm, vec3 diffR ) {
    vec3 r = reflect( -lightDir, norm );
    float sDotN = max( dot(lightDir, norm), 0.0 );

    vec3 ambient = material.ka * light.col;
    vec3 diffuse = light.col * diffR * sDotN;
    vec3 spec = vec3(0.0);

    if( sDotN > 0.0 ) {
        spec = light.col * material.ks * pow( max( dot(r,ViewDir), 0.0 ),
    material.alpha);
    }

    return ambient + diffuse + spec;
}
void main() {
 // Lookup the normal from the normal map
 vec4 normal = texture( normalMapTexture, uv );
 // The color texture is used as the diffuse reflectivity
 vec4 texColor = texture( diffuseTexture, uv );
 FragColor = vec4( phongModel(normal.xyz, texColor.rgb),
 1.0 );
}