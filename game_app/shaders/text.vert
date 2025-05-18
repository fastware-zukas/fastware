#version 450 core

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 vOrigin;
layout(location = 2) in vec2 vUV;

layout(location = 10) uniform mat4 uPV;

out gl_PerVertex {
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

out block {
  layout(location = 1) vec3 vColour;
  layout(location = 2) vec2 vOrigin;
  layout(location = 3) vec2 vUV;
}
Out;

void main() {
  gl_Position = uPV * vec4(vPos, 0.0f, 1.0f);
  Out.vColour = vec3(0.45,0.4, 0.65);
  Out.vOrigin = vOrigin;
  Out.vUV = vUV;
}
