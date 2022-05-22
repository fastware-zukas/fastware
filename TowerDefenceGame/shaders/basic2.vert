#version 450 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;
layout(location = 2) in vec2 vUV;
layout(location = 3) in mat4 vModel;

layout(location = 7) uniform mat4 uPV;

out gl_PerVertex {
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

out block {
  layout(location = 1) vec3 vNorm;
  layout(location = 2) vec2 vUV;
}
Out;

void main() {
  gl_Position = uPV * vModel * vec4(vPos, 1.0);
  Out.vNorm = vNorm;
  Out.vUV = vUV;
}
