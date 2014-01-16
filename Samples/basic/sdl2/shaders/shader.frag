#version 150

uniform sampler2D texSampler;

in vec2 texCoord;
in vec4 fragColor;

out vec4 finalColor;

void main() {
    vec4 objectColor = texture2D(texSampler, texCoord);
    finalColor = objectColor * fragColor;
}
