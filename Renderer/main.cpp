#include "context.h"
#include "render.h"
#include "helper.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <regex>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

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
	/*
#pragma region Load .obj file
	string inputFile = "yellyfish.obj";
	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputFile.c_str());

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
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
	}
	
#pragma endregion
*/
	// Triangle, CCW
	vertex triVerts[] = 
	{
		{ { -.5f, -.5f, 0, 1 }, { 1.0, 0.0, 0.0, 1.0}, {0.0, 0.0} },
		{ {  .5f, -.5f, 0, 1 }, { 0.0, 1.0, 0.0, 1.0}, {1.f, 0.0} },
		{ {    0,  .5f, 0, 1 }, { 0.0, 0.0, 1.0, 1.0}, {.5f, 1.f} }
	};

	unsigned int triIndices[] = { 0, 1, 2 };

	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);

	geometry plane = generatePlane(5, 5);

	texture tex = loadTexture("textures\\tex.tga");
	texture dbgtex = loadTexture("textures\\debug.png");

	string basicVert = loadShaderFile("shaders\\vert.txt");
	string basicFrag = loadShaderFile("shaders\\frag.txt");

	shader basicShad = makeShader(basicVert.c_str(), basicFrag.c_str());

	glm::mat4 triModel = glm::identity<glm::mat4>();

	glm::mat4 camProj = glm::perspective(glm::radians(45.f), 640.f / 480.f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	setUniform(basicShad, 0, camProj);
	setUniform(basicShad, 1, camView);
	setUniform(basicShad, 3, tex, 0);

	while (!game.shouldClose())
	{
		game.tick();
		game.clear();

		triModel = glm::rotate(triModel, glm::radians(5.f), glm::vec3(1, 0, 0));
		//triModel = glm::translate(triModel, glm::vec3(0.05f, 0, 0));

		setUniform(basicShad, 2, triModel);

		draw(basicShad, triangle);
		//draw(basicShad, plane);

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