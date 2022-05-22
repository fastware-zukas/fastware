#version 450

in block {
  layout(location = 1) vec3 vNorm;
  layout(location = 2) vec2 vUV;
}
In;

layout(location = 11, binding = 0) uniform sampler2D textureSampler;

layout(location = 0, index = 0) out vec4 fColour;

void main(void) { fColour = vec4(0.8, 0.5, 0.3, 1.0); }
