#include "context.h"
#include "render.h"
#include "helper.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <regex>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

using std::vector;

geometry generatePlane(float width, float height);

int main()
{
	context game;
	game.init(640, 480, "Source3");

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(errorCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true);
#endif

	// Triangle, CCW
	vertex triVerts[] = 
	{
		{ { -.5f, -.5f, 0, 1 }, { 1.0, 0.0, 0.0, 1.0}, {0, 0, 1, 1}, {0.0, 0.0} },
		{ {  .5f, -.5f, 0, 1 }, { 0.0, 1.0, 0.0, 1.0}, {0, 0, 1, 1}, {1.f, 0.0} },
		{ {    0,  .5f, 0, 1 }, { 0.0, 0.0, 1.0, 1.0}, {0, 0, 1, 1}, {.5f, 1.f} }
	};

	unsigned int triIndices[] = { 0, 1, 2 };

	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);

	geometry yellyfish = loadGeometry("models\\test.obj");

	geometry plane = generatePlane(5, 5);

	texture tex = loadTexture("textures\\tex.tga");
	texture dbgtex = loadTexture("textures\\debug.png");

	string basicVert = loadShaderFile("shaders\\vert.txt");
	string basicFrag = loadShaderFile("shaders\\frag.txt");

	shader basicShad = makeShader(basicVert.c_str(), basicFrag.c_str());

	glm::mat4 triModel = glm::identity<glm::mat4>();

	glm::mat4 camProj = glm::perspective(glm::radians(45.f), 640.f / 480.f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	light sun;
	sun.direction = glm::vec4{ -1, -1, 1, 1 };
	sun.color = glm::vec4{ 1.0, 1.0, 1.0, 1.0 };

	setUniform(basicShad, 0, camProj);
	setUniform(basicShad, 1, camView);
	setUniform(basicShad, 3, tex, 0);
	setUniform(basicShad, 4, sun.direction);
	setUniform(basicShad, 5, sun.color);

	while (!game.shouldClose())
	{
		game.tick();
		game.clear();

		triModel = glm::rotate(triModel, glm::radians(3.f), glm::vec3(0, 1, 0));
		//triModel = glm::translate(triModel, glm::vec3(0.05f, 0, 0));

		setUniform(basicShad, 2, triModel);
		draw(basicShad, yellyfish);

		assert(glGetError() == GL_NO_ERROR);
	}

	game.term();

	return 0;
}

geometry generatePlane(float width, float height)
{
	vector<vertex> meshVerts;
	for (int i = 0; i < height + 1; ++i)
	{
		for (int k = 0; k < width + 1; ++k)
		{
			vertex vert;
			if ((k + i) % 2 == 0)
				vert = { {k, i, 0.0, 1.0}, { 1.0, 0.0, 0.0, 1.0} };
			else
				vert = { {k, i, 0.0, 1.0}, { 0.0, 1.0, 0.0, 1.0} };
			meshVerts.push_back(vert);
		}
	}

	vector<unsigned int> meshTris;
	for (int i = 0; i < height; ++i)
	{
		for (int k = 0; k < width; ++k)
		{
			meshTris.push_back(k + (i * (width + 1)));
			meshTris.push_back(k + (i * (width + 1)) + width + 2);
			meshTris.push_back(k + (i * (width + 1)) + 1);

			meshTris.push_back(k + (i * (width + 1)));
			meshTris.push_back(k + (i * (width + 1)) + width + 1);
			meshTris.push_back(k + (i * (width + 1)) + width + 2);
		}
	}

	return makeGeometry(&meshVerts[0], meshVerts.size(), &meshTris[0], meshTris.size());
}