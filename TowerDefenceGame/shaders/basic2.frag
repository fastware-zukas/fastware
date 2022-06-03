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

  vec3 norm = normalize(In.vNorm);
  vec3 lightDir = (lightPosition - In.vFragPos);
  vec3 lightDirNorm = normalize(lightDir);

  float dist = length(lightDir);
  float diff = clamp(dot(norm, lightDirNorm), 0, 1);

  float ambientStrength = 0.05;
  vec3 ambient = ambientStrength * normalize(lightColour);

  vec3 diffuse = texture(textureSampler, In.vUV).rgb;

  vec3 result =
      ambient + diffuse * lightColour * diff * lightIntensity / (dist * dist);

  if (mode <= 0) {
    fColour = vec4(result, 1.0);
  } else if (mode == 1) {
    fColour = vec4(diffuse, 1.0);
  } else if (mode == 2) {
    fColour = vec4(lightDirNorm, 1.0);
  } else {
    fColour = vec4(lightColour * lightIntensity / (dist * dist), 1.0);
  }
}
