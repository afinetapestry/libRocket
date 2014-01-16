#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "SDL2RenderInterface.hpp"

#include "glexception.hpp"

string LoadFile(const string & filename) {
	ifstream file;
	file.exceptions(std::ifstream::failbit);
	file.open(filename.c_str());
	string text(istreambuf_iterator<char>(file), (istreambuf_iterator<char>()));
	return text;
}

string GetInfoLog(GLuint object, void (_GL_CALL *glGet__iv)(GLuint, GLenum, GLint *), void (_GL_CALL *glGet__InfoLog)(GLuint, GLsizei, GLsizei *, GLchar *)) {
	GLint length;
	string log;
	glGet__iv(object, GL_INFO_LOG_LENGTH, &length);
	_glException();
	log.reserve(length);
	glGet__InfoLog(object, length, NULL, &log[0]);
	_glException();
	return log;
}

GLuint program = 0;
GLuint vertShader = 0;
GLuint fragShader = 0;
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

	RendererGeometry() : vao(0), vbo(0), vio(0), tex(0), numVert(0) {
		glGenVertexArrays(1, &vao);
		_glException();
		glGenBuffers(1, &vbo);
		_glException();
		glGenBuffers(1, &vio);
		_glException();
	};

	void Bind() {
		glBindVertexArray(vao);
		_glException();
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		_glException();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
		_glException();
	}

	void Unbind() {
		glBindVertexArray(0);
		_glException();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		_glException();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		_glException();
	}

	void Buffer(Rocket::Core::Vertex * vertices, int num_vertices, int * indices, int num_indices, Rocket::Core::TextureHandle texture) {
		tex = texture;
		numVert = num_vertices;
		numInd = num_indices;

		Bind();

		glBufferData(GL_ARRAY_BUFFER, sizeof(Rocket::Core::Vertex) * num_vertices, vertices, GL_STATIC_DRAW);
		_glException();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, indices, GL_STATIC_DRAW);
		_glException();

		Unbind();
	};

	void Render(const Rocket::Core::Vector2f & translation) {
		glUseProgram(program);
		_glException();

		Bind();

		glActiveTexture(GL_TEXTURE0);
		_glException();
		glBindTexture(GL_TEXTURE_2D, tex);
		_glException();
		glUniform1i(texSampler, 0);
		_glException();

		glEnable(GL_BLEND);
		_glException();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		_glException();

		glEnableVertexAttribArray(vertexPosition);
		_glException();
		glEnableVertexAttribArray(vertexColor);
		_glException();
		glEnableVertexAttribArray(vertexTexCoord);
		_glException();

		glVertexAttribPointer(vertexPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Rocket::Core::Vertex), 0);
		_glException();
		glVertexAttribPointer(vertexColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Rocket::Core::Vertex), (GLvoid *)sizeof(Rocket::Core::Vertex::position));
		_glException();
		glVertexAttribPointer(vertexTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Rocket::Core::Vertex), (GLvoid *)(sizeof(Rocket::Core::Vertex::position) + sizeof(Rocket::Core::Vertex::colour)));
		_glException();

		glUniform2f(trans, translation.x, translation.y);
		_glException();

		// Draw the geometry
		glDrawElements(GL_TRIANGLES, numInd, GL_UNSIGNED_INT, 0);
		_glException();

		glDisableVertexAttribArray(vertexPosition);
		_glException();
		glDisableVertexAttribArray(vertexColor);
		_glException();
		glDisableVertexAttribArray(vertexTexCoord);
		_glException();

		glDisable(GL_BLEND);
		_glException();

		Unbind();

		glUseProgram(0);
		_glException();
	};

	~RendererGeometry() {
		glDeleteBuffers(1, &vio);
		_glException();
		glDeleteBuffers(1, &vbo);
		_glException();
		glDeleteVertexArrays(1, &vao);
	};
};

RendererGeometry * dynamic;

SDL2RenderInterface::SDL2RenderInterface() {
	program = glCreateProgram();
	_glException();

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	_glException();
	string vertSource = LoadFile("shaders/shader.vert");
	const char * vertStr = vertSource.c_str();
	int vertLen = vertSource.length();
	glShaderSource(vertShader, 1, &vertStr, &vertLen);
	_glException();
	glCompileShader(vertShader);
	_glException();
	GLint vertStatus;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &vertStatus);
	_glException();
	if (!vertStatus) {
		string log = "Shader Compilation Failed:\n" + GetInfoLog(vertShader, *glGetShaderiv, *glGetShaderInfoLog);
		throw runtime_error(log);
	}
	glAttachShader(program, vertShader);
	_glException();

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	_glException();
	string fragSource = LoadFile("shaders/shader.frag");
	const char * fragStr = fragSource.c_str();
	int fragLen = fragSource.length();
	glShaderSource(fragShader, 1, &fragStr, &fragLen);
	_glException();
	glCompileShader(fragShader);
	_glException();
	GLint fragStatus;
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &fragStatus);
	_glException();
	if (!fragStatus) {
		string log = "Shader Compilation Failed:\n" + GetInfoLog(fragShader, *glGetShaderiv, *glGetShaderInfoLog);
		throw runtime_error(log);
	}
	glAttachShader(program, fragShader);
	_glException();

	glLinkProgram(program);
	_glException();
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	_glException();
	if (!linkStatus) {
		string log = "Shader Linking Failed:\n" + GetInfoLog(program, *glGetProgramiv, *glGetProgramInfoLog);
		throw runtime_error(log);
	}

	texSampler = glGetUniformLocation(program, "texSampler");
	vertexTexCoord = glGetAttribLocation(program, "vertexTexCoord");
	trans = glGetUniformLocation(program, "translation");
	projection = glGetUniformLocation(program, "projection");
	vertexPosition = glGetAttribLocation(program, "vertexPosition");
	vertexColor = glGetAttribLocation(program, "vertexColor");

	dynamic = new RendererGeometry();
}

SDL2RenderInterface::~SDL2RenderInterface() {
	glUseProgram(0);
	_glException();
	glDeleteShader(fragShader);
	_glException();
	glDeleteShader(vertShader);
	_glException();
	glDeleteProgram(program);
	_glException();

	delete dynamic;
}

void SDL2RenderInterface::SetViewport(int width, int height) {
	m_width = width;
	m_height = height;

	glViewport(0, 0, width, height);
	_glException();

	float left = 0.0f;
	float right = width;
	float top = 0.0f;
	float bottom = height;
	float far = 1.0f;
	float near = -1.0f;

	glUseProgram(program);
	_glException();

	// Ortho matrix in column major
	GLfloat proj[4][4] = {
		{2.0f / (right - left),				0.0f,								0.0f,							0.0f},
		{0.0f,								2.0f / (top - bottom),				0.0f,							0.0f},
		{0.0f,								0.0f,								-2.0f / (far - near),			0.0f},
		{-(right + left) / (right - left),	-(top + bottom) / (top - bottom),	-(far + near) / (far - near),	1.0f}
	};
	glUniformMatrix4fv(projection, 1, GL_FALSE, (const GLfloat *)&proj);
	_glException();

	glUseProgram(0);
	_glException();
}

void SDL2RenderInterface::RenderGeometry(Rocket::Core::Vertex * vertices, int num_vertices, int * indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f & translation) {
	dynamic->Bind();

	dynamic->Buffer(vertices, num_vertices, indices, num_indices, texture);

	dynamic->Render(translation);

	dynamic->Unbind();
}

Rocket::Core::CompiledGeometryHandle SDL2RenderInterface::CompileGeometry(Rocket::Core::Vertex * vertices, int num_vertices, int * indices, int num_indices, Rocket::Core::TextureHandle texture) {
	RendererGeometry * geo = new RendererGeometry();
	geo->Buffer(vertices, num_vertices, indices, num_indices, texture);
	return (Rocket::Core::CompiledGeometryHandle)geo;
	//return (Rocket::Core::CompiledGeometryHandle)NULL;
}

void SDL2RenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f & translation) {
	RendererGeometry * geo = (RendererGeometry *)geometry;
	geo->Render(translation);
}

void SDL2RenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry) {
	RendererGeometry * geo = (RendererGeometry *)geometry;
	delete geo;
}

/**
 * @author Lloyd Weehuizen
 */
void SDL2RenderInterface::EnableScissorRegion(bool enable) {
	if (enable) {
		glEnable(GL_SCISSOR_TEST);
		_glException();
	} else {
		glDisable(GL_SCISSOR_TEST);
		_glException();
	}
}
void SDL2RenderInterface::SetScissorRegion(int x, int y, int width, int height) {
	glScissor(x, m_height - (y + height), width, height);
	_glException();
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

bool SDL2RenderInterface::LoadTexture(Rocket::Core::TextureHandle & texture_handle, Rocket::Core::Vector2i & texture_dimensions, const Rocket::Core::String & source) {
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
bool SDL2RenderInterface::GenerateTexture(Rocket::Core::TextureHandle & texture_handle, const Rocket::Core::byte * source, const Rocket::Core::Vector2i & source_dimensions){
	GLuint texture_id = 0;
	glGenTextures(1, &texture_id);
	_glException();
	if (texture_id == 0) {
		printf("Failed to generate textures\n");
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, texture_id);
	_glException();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
	_glException();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_glException();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	_glException();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	_glException();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	_glException();

	texture_handle = (Rocket::Core::TextureHandle) texture_id;

	return true;
}
void SDL2RenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture_handle) {
	glDeleteTextures(1, (GLuint*) &texture_handle);
	_glException();
}
