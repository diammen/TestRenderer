#pragma once

#include "glew/GL/glew.h"
#include "glm/glm.hpp"

struct vertex
{
	glm::vec4 pos;
	glm::vec4 color;
};

struct geometry
{
	GLuint vao, vbo, ibo;	// buffers
	GLuint size;			// index count
};

struct shader
{
	GLuint program;
};

geometry makeGeometry(vertex * verts, size_t vertCount,
					  unsigned * indices, size_t indxCount);

void freeGeometry(geometry &geo);

shader makeShader(const char * vertSource,
			      const char * fragSource);

void freeShader(shader &shad);

void draw(const shader &shad, const geometry &geo);

void setUniform(const shader &shad, GLuint location, const glm::mat4 &value);

void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message,
	const void *userParam);