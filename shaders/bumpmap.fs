#version 400

struct Light {
  vec3 pos;  // 0 => directional light; 1 => point light
  vec3 col; 
};

struct Material {
  float kd;
  float ka;
  float ks;
  float alpha;
};

uniform sampler2D diffuseTexture;
uniform sampler2D normalMapTexture;

uniform Light light; 
uniform Material material;

in vec4 position;
in vec4 lightpos;
in vec3 normal;
in vec2 uv;

out vec4 FragColor;

vec3 phongModel(in vec3 ePos, in vec3 eNormal) {
  vec3 L = normalize(lightpos.xyz - lightpos.w * ePos);
  vec3 v = normalize(-ePos);
  vec3 r = normalize(reflect(L, eNormal));

  vec3 ambient = light.col * material.ka;

  float sDotN = max( dot(L,eNormal), 0.0 ); 
  vec3 diffuse = sDotN * light.col * material.kd;

  vec3 mainColor = texture(diffuseTexture, uv).rgb;
  vec3 color = mainColor*(ambient + diffuse);

  float b = max(dot(r, -v), 0.0);
  vec3 spec = light.col * material.ks * pow(b, material.alpha);

  return color + spec;
}

void main() {
//   vec3 texNormal = normalize(2*(texture(normalMapTexture, uv).xyz-0.5f));
  vec3 texNormal = texture(normalMapTexture, uv).xyz;
  // vec3 norm = normalize(normal + texNormal);
  vec3 norm = 1.25*normalize(normal + texNormal);
  vec3 color = phongModel(position.xyz, norm);
  FragColor = vec4(color, 1.0);
}