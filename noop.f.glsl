varying vec2 texcoord;
uniform sampler2D textures[2];

void main(void) {
    gl_FragColor = texture2D(textures[0], vec2(texcoord.s, texcoord.t));
}
