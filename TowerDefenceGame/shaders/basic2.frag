#version 450

in block {
  layout(location = 1) smooth vec3 vFragPos;
  layout(location = 2) vec3 vNorm;
  layout(location = 3) vec2 vUV;
}
In;

layout(location = 11, binding = 0) uniform sampler2D textureSampler;
layout(location = 12) uniform vec3 lightPosition;
layout(location = 13) uniform vec3 lightColour;
layout(location = 14) uniform float lightIntensity;
layout(location = 15) uniform int mode;

layout(location = 0, index = 0) out vec4 fColour;

void main(void) {

  float ambientStrength = 0.05;
  vec3 ambient = ambientStrength * normalize(lightColour);

  vec3 norm = normalize(In.vNorm);
  vec3 lightDir = normalize(lightPosition - In.vFragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * texture(textureSampler, In.vUV).rgb;

  float dist = length(lightPosition - In.vFragPos);

  vec3 result =
      ambient + diffuse * lightColour * diff * lightIntensity / (dist * dist);

  if (mode <= 0) {
    fColour = vec4(result, 1.0);
  } else if (mode == 1) {
    fColour = vec4(diffuse, 1.0);
  } else if (mode == 2) {
    fColour = vec4(lightDir, 1.0);
  } else {
    fColour = vec4(lightColour * lightIntensity / (dist * dist), 1.0);
  }
}
