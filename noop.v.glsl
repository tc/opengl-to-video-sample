attribute vec3 position;
varying vec2 texcoord;

void main(void) {
 gl_Position = vec4(position, 1.0);
 texcoord = vec2(position) + vec2(0.5);
}


