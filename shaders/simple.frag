#version 330 core

out vec3 color;

void main() {
  float t = gl_FragCoord.y / 720;
  color = vec3(1.0 - t, 0.0, t);
}
