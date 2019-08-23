#include "context.h"
#include "render.h"
#include "helper.h"
#include <vector>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

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
		{ { -.5f, -.5f, 0, 1 }, { 1.0, 0.0, 0.0, 1.0} },
		{ {  .5f, -0.5f, 0, 1 }, { 0.0, 1.0, 0.0, 1.0} },
		{ {    0,  .5f, 0, 1 }, { 0.0, 0.0, 1.0, 1.0} }
	};

	unsigned int triIndices[] = { 0, 1, 2 };

	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);

	geometry plane = generatePlane(5, 5);

	//string basicVert =
	//	"#version 430\n"
	//	"layout (location = 0) in vec4 position;\n"
	//	"layout (location = 0) uniform mat4 proj;\n"
	//	"layout (location = 1) uniform mat4 view;\n"
	//	"layout (location = 2) uniform mat4 model;\n"
	//	"void main() { gl_Position = position; }";

	//string basicFrag =
	//	"#version 330\n"
	//	"out vec4 vertColor;\n"
	//	"void main() { vertColor = vec4(1.0, 0.0, 0.0, 1.0); }";

	string basicVert = loadShaderFile("shaders\\vert.txt");
	string basicFrag = loadShaderFile("shaders\\frag.txt");

	shader basicShad = makeShader(basicVert.c_str(), basicFrag.c_str());

	glm::mat4 triModel = glm::identity<glm::mat4>();

	glm::mat4 camProj = glm::perspective(glm::radians(45.f), 640.f / 480.f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	setUniform(basicShad, 0, camProj);
	setUniform(basicShad, 1, camView);

	while (!game.shouldClose())
	{
		game.tick();
		game.clear();

		triModel = glm::rotate(triModel, glm::radians(5.f), glm::vec3(0, 1, 0));

		setUniform(basicShad, 2, triModel);

		//draw(basicShad, triangle);
		draw(basicShad, plane);
	}

	game.term();

	return 0;
}

geometry generatePlane(float width, float height)
{
	using std::vector;

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