#include "render.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
using std::string;
using std::vector;

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

#include "glm/gtc/type_ptr.hpp"

geometry makeGeometry(vertex * verts, size_t vertCount, unsigned * indices, size_t indxCount)
{
	// create an instance of geometry
	geometry newGeo = {};
	newGeo.size = indxCount;

	// generate buffers
	glGenVertexArrays(1, &newGeo.vao);	// Vertex Array Object
	glGenBuffers(1, &newGeo.vbo);		// Vertex Buffer Object
	glGenBuffers(1, &newGeo.ibo);		// Index Buffer Object

	// bind buffers
	glBindVertexArray(newGeo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);

	// populate buffers
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxCount * sizeof(unsigned), indices, GL_STATIC_DRAW);

	// describe vertex data
	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)16);

	glEnableVertexAttribArray(2); // normals
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)32);

	glEnableVertexAttribArray(3); // uvs
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)48);

	// unbind buffers (in a SPECIFIC order)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// return the geometry
	return newGeo;
}

void freeGeometry(geometry & geo)
{
	glDeleteBuffers(1, &geo.vbo);
	glDeleteBuffers(1, &geo.ibo);
	glDeleteVertexArrays(1, &geo.vao);

	geo = {};
}

geometry loadGeometry(const char * source)
{
		tinyobj::attrib_t attrib;
		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> materials;
	
		string err;
	
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, source);
	
		if (!err.empty())
		{
			std::cerr << err << std::endl;
		}
	
		if (!ret)
		{
			exit(1);
		}
	
		vector<vertex> vertices;
		vector<unsigned int> indices;
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); ++s)
		{
			// Loop over faces (polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
			{
				int fv = shapes[s].mesh.num_face_vertices[f];

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
					tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
					tinyobj::real_t xz = attrib.vertices[3 * idx.vertex_index + 2];

					// normals
					tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
					tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
					tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

					// texture coords
					tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];

					vertices.push_back(vertex{ {vx, vy, xz, 1}, { 0,0,0,0 }, {nx, ny, nz, 1}, {tx, ty} });
					indices.push_back(3 * f + v);
				}
				index_offset += fv;

				// per-face material
				shapes[s].mesh.material_ids[f];
			}
		}
		return makeGeometry(&vertices[0], vertices.size(), &indices[0], indices.size());
}

shader makeShader(const char * vertSource, const char * fragSource)
{
	// make the shader object
	shader newShad = {};
	newShad.program = glCreateProgram();

	// create the shaders
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	// compile the shaders
	glShaderSource(vert, 1, &vertSource, 0);
	glShaderSource(frag, 1, &fragSource, 0);
	glCompileShader(vert);
	glCompileShader(frag);

	// check vertex shader
	GLint vert_compiled;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &vert_compiled);
	if (vert_compiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(vert, 1024, &log_length, message);
	}

	// check fragment shader
	GLint frag_compiled;
	glGetShaderiv(frag, GL_COMPILE_STATUS, &vert_compiled);
	if (vert_compiled != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(vert, 1024, &log_length, message);
	}

	// attach the shaders
	glAttachShader(newShad.program, vert);
	glAttachShader(newShad.program, frag);

	// link the shaders
	glLinkProgram(newShad.program);

	// check the shader object
	GLint program_linked;
	glGetProgramiv(newShad.program, GL_LINK_STATUS, &program_linked);
	if (program_linked != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetProgramInfoLog(newShad.program, 1024, &log_length, message);
		// Write the error to a log
	}

	// delete the shaders
	glDeleteShader(vert);
	glDeleteShader(frag);

	// return the shader object
	return newShad;

}

void freeShader(shader & shad)
{
	glDeleteProgram(shad.program);
	shad = {};
}

void draw(const shader & shad, const geometry & geo)
{
	// bind the shader program
	glUseProgram(shad.program);
	// bind the VAO (geo and idices)
	glBindVertexArray(geo.vao);
	// draw
	glDrawElements(GL_TRIANGLES, geo.size, GL_UNSIGNED_INT, 0);

}

void setUniform(const shader &shad, GLuint location, const glm::mat4 &value)
{
	glProgramUniformMatrix4fv(shad.program, location, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const shader &shad, GLuint location, const texture &value, int textureSlot)
{
	// specify the texture slot we're working with
	glActiveTexture(GL_TEXTURE0 + textureSlot);

	// bind the texture to that slot
	glBindTexture(GL_TEXTURE_2D, value.handle);

	// assign the uniform to the shader
	glProgramUniform1i(shad.program, location, textureSlot);
}

void setUniform(const shader &shad, GLuint location, const glm::vec3 &value)
{
	glProgramUniform3fv(shad.program, location, 1, glm::value_ptr(value));
}

void setUniform(const shader &shad, GLuint location, const glm::vec4 &value)
{
	glProgramUniform4fv(shad.program, location, 1, glm::value_ptr(value));
}

void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam)
{
	std::cerr << message << std::endl;
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char * pixels)
{
	GLenum oglFormat = GL_RGBA;
	switch (channels)
	{
	case 1:
		oglFormat = GL_RED;
		break;
	case 2:
		oglFormat = GL_RG;
		break;
	case 3:
		oglFormat = GL_RGB;
		break;
	case 4:
		oglFormat = GL_RGBA;
		break;
	default:
		// TODO: fatal error, halt the program
		break;
	}

	texture tex = { 0, width, height, channels };

	// generating and binding the texture
	glGenTextures(1, &tex.handle);
	glBindTexture(GL_TEXTURE_2D, tex.handle);

	// buffer/send the actual data
	glTexImage2D(GL_TEXTURE_2D, 0, oglFormat, width, height, 0, oglFormat, GL_UNSIGNED_BYTE, pixels);

	// describe how the texrue will be used
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

void freeTexture(texture & tex)
{
	glDeleteTextures(1, &tex.handle);
	tex = {};
}

texture loadTexture(const char * imagePath)
{
	int imageWidth, imageHeight, imageFormat;
	unsigned char *rawPixelData = nullptr;

	// tell stb image to load the image
	stbi_set_flip_vertically_on_load(true);
	rawPixelData = stbi_load(imagePath,
							 &imageWidth,
							 &imageHeight,
						  	 &imageFormat,
							 STBI_default);

	// TODO: ensure that rawPixelData is NOT NULL. if it is, the image failed to load
	assert(rawPixelData != nullptr);

	// pass the data to make texture to make the texture
	texture tex = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);

	// free the image
	stbi_image_free(rawPixelData);

	return tex;
}
