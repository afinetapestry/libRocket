#version 150

uniform vec2 translation;
uniform mat4 projection;
uniform vec2 viewDimensions;

in vec2 vertexPosition;
in vec4 vertexColor;
in vec2 vertexTexCoord;

out vec2 texCoord;
out vec4 fragColor;

void main() {
	texCoord = vertexTexCoord;
	fragColor = vertexColor;
	gl_Position = projection * vec4(vertexPosition + translation, 0.0, 1.0);
}
