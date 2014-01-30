/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2014 Jon Hatchett
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "OpenGL32RenderInterface.hpp"

const char * colorVert = R"delim(
#version 150

uniform vec2 translation;
uniform mat4 projection;
uniform vec2 viewDimensions;

in vec2 vertexPosition;
in vec4 vertexColor;

out vec4 fragColor;

void main() {
	fragColor = vertexColor;
	gl_Position = projection * vec4(vertexPosition + translation, 0.0, 1.0);
}
)delim";

const char * colorFrag = R"delim(
#version 150

in vec2 texCoord;
in vec4 fragColor;

out vec4 finalColor;

void main() {
    finalColor = fragColor;
}
)delim";

const char * texVert = R"delim(
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
)delim";

const char * texFrag = R"delim(
#version 150

uniform sampler2D texSampler;

in vec2 texCoord;
in vec4 fragColor;

out vec4 finalColor;

void main() {
    vec4 objectColor = texture2D(texSampler, texCoord);
    finalColor = objectColor * fragColor;
}
)delim";

/**
 *
 */
string GetInfoLog(GLuint object, void (_GL_CALL *glGet__iv)(GLuint, GLenum, GLint *), void (_GL_CALL *glGet__InfoLog)(GLuint, GLsizei, GLsizei *, GLchar *)) {
	GLint length;
	string log;
	glGet__iv(object, GL_INFO_LOG_LENGTH, &length);
	log.reserve(length);
	glGet__InfoLog(object, length, NULL, &log[0]);
	return log;
}

GLuint texProgram = 0;
GLuint colorProgram = 0;
GLuint colorVertShader = 0;
GLuint texVertShader = 0;
GLuint colorFragShader = 0;
GLuint texFragShader = 0;
GLuint vertexArray = 0;
GLuint vertexBuffer = 0;
GLuint indexBuffer = 0;
GLuint vertexPosition = -1;
GLuint vertexColor = -1;
GLuint vertexTexCoord = -1;
GLuint texSampler = -1;
GLuint trans = -1;
GLuint projection = -1;

struct RendererGeometry {
	GLuint vao, vbo, vio;
	int numVert;
	int numInd;
	Rocket::Core::TextureHandle tex;

	RendererGeometry() : vao(0), vbo(0), vio(0), numVert(0), numInd(0), tex(0) {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &vio);
	};

	void Bind() {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
	}

	void Unbind() {
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Buffer(Rocket::Core::Vertex * vertices, int num_vertices, int * indices, int num_indices, Rocket::Core::TextureHandle texture) {
		tex = texture;
		numVert = num_vertices;
		numInd = num_indices;

		Bind();

		glBufferData(GL_ARRAY_BUFFER, sizeof(Rocket::Core::Vertex) * num_vertices, vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, indices, GL_STATIC_DRAW);

		Unbind();
	};

	void Render(const Rocket::Core::Vector2f & translation) {
		GLuint program = tex ? texProgram : colorProgram;

		Bind();

		glUseProgram(program);

		vertexPosition = glGetAttribLocation(program, "vertexPosition");
		vertexColor = glGetAttribLocation(program, "vertexColor");
		trans = glGetUniformLocation(program, "translation");

		if (tex) {
			texSampler = glGetUniformLocation(program, "texSampler");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(texSampler, 0);

			vertexTexCoord = glGetAttribLocation(program, "vertexTexCoord");
			glEnableVertexAttribArray(vertexTexCoord);
			glVertexAttribPointer(vertexTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Rocket::Core::Vertex), (GLvoid *)(sizeof(Rocket::Core::Vertex::position) + sizeof(Rocket::Core::Vertex::colour)));
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnableVertexAttribArray(vertexPosition);
		glEnableVertexAttribArray(vertexColor);

		glVertexAttribPointer(vertexPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Rocket::Core::Vertex), 0);
		glVertexAttribPointer(vertexColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Rocket::Core::Vertex), (GLvoid *)sizeof(Rocket::Core::Vertex::position));

		glUniform2f(trans, translation.x, translation.y);

		// Draw the geometry
		glDrawElements(GL_TRIANGLES, numInd, GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(vertexPosition);
		glDisableVertexAttribArray(vertexColor);

		if (tex) {
			glDisableVertexAttribArray(vertexTexCoord);
		}

		glDisable(GL_BLEND);

		Unbind();

		glUseProgram(0);
	};

	~RendererGeometry() {
		glDeleteBuffers(1, &vio);
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	};
};

RendererGeometry * dynamic;

OpenGL32RenderInterface::OpenGL32RenderInterface() {
	colorVertShader = glCreateShader(GL_VERTEX_SHADER);
	int colorVertLen = strlen(colorVert);
	glShaderSource(colorVertShader, 1, &colorVert, &colorVertLen);
	glCompileShader(colorVertShader);
	GLint texVertStatus;
	glGetShaderiv(colorVertShader, GL_COMPILE_STATUS, &texVertStatus);
	if (!texVertStatus) {
		string log = "Shader Compilation Failed:\n" + GetInfoLog(colorVertShader, *glGetShaderiv, *glGetShaderInfoLog);
		throw runtime_error(log);
	}

	colorFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	int colorFragLen = strlen(colorFrag);
	glShaderSource(colorFragShader, 1, &colorFrag, &colorFragLen);
	glCompileShader(colorFragShader);
	GLint colorFragStatus;
	glGetShaderiv(colorFragShader, GL_COMPILE_STATUS, &colorFragStatus);
	if (!colorFragStatus) {
		string log = "Shader Compilation Failed:\n" + GetInfoLog(colorFragShader, *glGetShaderiv, *glGetShaderInfoLog);
		throw runtime_error(log);
	}

	colorProgram = glCreateProgram();
	glAttachShader(colorProgram, colorVertShader);
	glAttachShader(colorProgram, colorFragShader);
	glLinkProgram(colorProgram);
	GLint colorLinkStatus;
	glGetProgramiv(colorProgram, GL_LINK_STATUS, &colorLinkStatus);
	if (!colorLinkStatus) {
		string log = "Shader Linking Failed:\n" + GetInfoLog(colorProgram, *glGetProgramiv, *glGetProgramInfoLog);
		throw runtime_error(log);
	}

	texVertShader = glCreateShader(GL_VERTEX_SHADER);
	int texVertLen = strlen(texVert);
	glShaderSource(texVertShader, 1, &texVert, &texVertLen);
	glCompileShader(texVertShader);
	GLint colorVertStatus;
	glGetShaderiv(texVertShader, GL_COMPILE_STATUS, &colorVertStatus);
	if (!colorVertStatus) {
		string log = "Shader Compilation Failed:\n" + GetInfoLog(texVertShader, *glGetShaderiv, *glGetShaderInfoLog);
		throw runtime_error(log);
	}

	texFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	int texFragLen = strlen(texFrag);
	glShaderSource(texFragShader, 1, &texFrag, &texFragLen);
	glCompileShader(texFragShader);
	GLint texFragStatus;
	glGetShaderiv(texFragShader, GL_COMPILE_STATUS, &texFragStatus);
	if (!colorFragStatus) {
		string log = "Shader Compilation Failed:\n" + GetInfoLog(texFragShader, *glGetShaderiv, *glGetShaderInfoLog);
		throw runtime_error(log);
	}

	texProgram = glCreateProgram();
	glAttachShader(texProgram, texVertShader);
	glAttachShader(texProgram, texFragShader);
	glLinkProgram(texProgram);
	GLint texLinkStatus;
	glGetProgramiv(texProgram, GL_LINK_STATUS, &texLinkStatus);
	if (!texLinkStatus) {
		string log = "Shader Linking Failed:\n" + GetInfoLog(texProgram, *glGetProgramiv, *glGetProgramInfoLog);
		throw runtime_error(log);
	}

	dynamic = new RendererGeometry();
}

OpenGL32RenderInterface::~OpenGL32RenderInterface() {
	glUseProgram(0);
	glDeleteProgram(texProgram);
	glDeleteProgram(colorProgram);
	glDeleteShader(texFragShader);
	glDeleteShader(colorFragShader);
	glDeleteShader(texVertShader);

	delete dynamic;
}

void OpenGL32RenderInterface::SetViewport(int width, int height) {
	m_width = width;
	m_height = height;

	float left = 0.0f;
	float right = width;
	float top = 0.0f;
	float bottom = height;
	float far = 1.0f;
	float near = -1.0f;


	// Ortho matrix in column major
	GLfloat proj[4][4] = {
		{2.0f / (right - left),				0.0f,								0.0f,							0.0f},
		{0.0f,								2.0f / (top - bottom),				0.0f,							0.0f},
		{0.0f,								0.0f,								-2.0f / (far - near),			0.0f},
		{-(right + left) / (right - left),	-(top + bottom) / (top - bottom),	-(far + near) / (far - near),	1.0f}
	};

	glUseProgram(texProgram);
	projection = glGetUniformLocation(texProgram, "projection");
	glUniformMatrix4fv(projection, 1, GL_FALSE, (const GLfloat *)&proj);
	glUseProgram(colorProgram);
	projection = glGetUniformLocation(colorProgram, "projection");
	glUniformMatrix4fv(projection, 1, GL_FALSE, (const GLfloat *)&proj);
	glUseProgram(0);
}

void OpenGL32RenderInterface::RenderGeometry(Rocket::Core::Vertex * vertices, int num_vertices, int * indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f & translation) {
	dynamic->Bind();
	dynamic->Buffer(vertices, num_vertices, indices, num_indices, texture);
	dynamic->Render(translation);
	dynamic->Unbind();
}

Rocket::Core::CompiledGeometryHandle OpenGL32RenderInterface::CompileGeometry(Rocket::Core::Vertex * vertices, int num_vertices, int * indices, int num_indices, Rocket::Core::TextureHandle texture) {
	RendererGeometry * geo = new RendererGeometry();
	geo->Buffer(vertices, num_vertices, indices, num_indices, texture);
	return (Rocket::Core::CompiledGeometryHandle)geo;
}

void OpenGL32RenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f & translation) {
	RendererGeometry * geo = (RendererGeometry *)geometry;
	geo->Render(translation);
}

void OpenGL32RenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry) {
	RendererGeometry * geo = (RendererGeometry *)geometry;
	delete geo;
}

/**
 * @author Lloyd Weehuizen
 */
void OpenGL32RenderInterface::EnableScissorRegion(bool enable) {
	if (enable) {
		glEnable(GL_SCISSOR_TEST);
	} else {
		glDisable(GL_SCISSOR_TEST);
	}
}
void OpenGL32RenderInterface::SetScissorRegion(int x, int y, int width, int height) {
	glScissor(x, m_height - (y + height), width, height);
}

#pragma pack(1)
struct TGAHeader {
	char  idLength;
	char  colourMapType;
	char  dataType;
	short int colourMapOrigin;
	short int colourMapLength;
	char  colourMapDepth;
	short int xOrigin;
	short int yOrigin;
	short int width;
	short int height;
	char  bitsPerPixel;
	char  imageDescriptor;
};
// Restore packing
#pragma pack()

bool OpenGL32RenderInterface::LoadTexture(Rocket::Core::TextureHandle & texture_handle, Rocket::Core::Vector2i & texture_dimensions, const Rocket::Core::String & source) {
	Rocket::Core::FileInterface * file_interface = Rocket::Core::GetFileInterface();
	Rocket::Core::FileHandle file_handle = file_interface->Open(source);
	if (!file_handle) {
		return false;
	}

	file_interface->Seek(file_handle, 0, SEEK_END);
	size_t buffer_size = file_interface->Tell(file_handle);
	file_interface->Seek(file_handle, 0, SEEK_SET);

	char* buffer = new char[buffer_size];
	file_interface->Read(buffer, buffer_size, file_handle);
	file_interface->Close(file_handle);

	TGAHeader header;
	memcpy(&header, buffer, sizeof(TGAHeader));

	int color_mode = header.bitsPerPixel / 8;
	int image_size = header.width * header.height * 4; // We always make 32bit textures

	if (header.dataType != 2) {
		Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Only 24/32bit uncompressed TGAs are supported.");
		return false;
	}

	// Ensure we have at least 3 colors
	if (color_mode < 3) {
		Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Only 24 and 32bit textures are supported");
		return false;
	}

	const char* image_src = buffer + sizeof(TGAHeader);
	unsigned char* image_dest = new unsigned char[image_size];

	// Targa is BGR, swap to RGB and flip Y axis
	for (long y = 0; y < header.height; y++) {
		long read_index = y * header.width * color_mode;
		long write_index = ((header.imageDescriptor & 32) != 0) ? read_index : (header.height - y - 1) * header.width * color_mode;
		for (long x = 0; x < header.width; x++)
		{
			image_dest[write_index] = image_src[read_index+2];
			image_dest[write_index+1] = image_src[read_index+1];
			image_dest[write_index+2] = image_src[read_index];
			if (color_mode == 4) {
				image_dest[write_index+3] = image_src[read_index+3];
			} else {
				image_dest[write_index+3] = 255;
			}

			write_index += 4;
			read_index += color_mode;
		}
	}

	texture_dimensions.x = header.width;
	texture_dimensions.y = header.height;

	bool success = GenerateTexture(texture_handle, image_dest, texture_dimensions);

	delete[] image_dest;
	delete[] buffer;

	return success;
}
bool OpenGL32RenderInterface::GenerateTexture(Rocket::Core::TextureHandle & texture_handle, const Rocket::Core::byte * source, const Rocket::Core::Vector2i & source_dimensions){
	GLuint texture_id = 0;
	glGenTextures(1, &texture_id);
	if (texture_id == 0) {
		printf("Failed to generate textures\n");
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	texture_handle = (Rocket::Core::TextureHandle) texture_id;

	return true;
}
void OpenGL32RenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture_handle) {
	glDeleteTextures(1, (GLuint*) &texture_handle);
}
