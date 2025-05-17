#version 450 core

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 vOrigin;
layout(location = 2) in vec2 vUV;

out gl_PerVertex {
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

out block {
  layout(location = 1) vec3 vColour;
  layout(location = 2) vec2 vUV;
}
Out;

void main() {
  gl_Position = vec4(vPos, 0.0f, 1.0f);
  Out.vColour = vec3(0.5f, 1.0f, 1.0f);
  Out.vUV = vUV;
}
