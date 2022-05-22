#version 450

in block {
  layout(location = 1) vec3 vNorm;
  layout(location = 2) vec2 vUV;
}
In;

layout(location = 11, binding = 0) uniform sampler2D textureSampler;

layout(location = 0, index = 0) out vec4 fColour;

void main(void) {
  vec3 multiple = In.vNorm * 0.4;
  vec3 texture_rgb = texture(textureSampler, In.vUV).rgb;
  vec3 colour = (texture_rgb + multiple);
  fColour = vec4(colour, 1.0);
}
