#include "render.h"
#include <iostream>

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
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)16);

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

	// attach the shaders
	glAttachShader(newShad.program, vert);
	glAttachShader(newShad.program, frag);

	// link the shaders
	glLinkProgram(newShad.program);

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

void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam)
{
	std::cerr << message << std::endl;
}
