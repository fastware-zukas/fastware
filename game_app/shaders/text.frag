#version 450

in block {
  layout(location = 1) vec3 vColour;
  layout(location = 2) vec2 vUV;
}
In;

layout(location = 11, binding = 0) uniform sampler2D textureSampler;

layout(location = 0, index = 0) out vec4 fColour;

void main(void) { 
  
  float value = texture(textureSampler, In.vUV).r;
  vec4 colour = vec4(1.0, 1.0, 1.0, value);
  fColour = vec4(In.vColour, 1.0) * colour;
}
