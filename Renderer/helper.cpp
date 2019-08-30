#include "helper.h"
#include "glfw/glfw3.h"
#include <fstream>
#include <iostream>
#include <time.h>
#include <stdio.h>
using std::ifstream;
using std::getline;
using std::ios;

string loadShaderFile(string shaderName)
{
	ifstream file;

	file.open(shaderName);

	string buffer;
	string output;

	if (file.is_open())
	{
		while (getline(file, buffer))
		{
			for (size_t i = 0; i < buffer.length(); ++i)
			{
				output += buffer[i];
			}
			output += "\n";
		}
	}
	else
	{
		std::cout << "\nError: shader file not found" << "\n";
	}

	file.close();

	return output;
}

float Time::programTime()
{
	return glfwGetTime();
}

float Time::systemTime()
{
	time_t curTime;
	curTime = time(NULL);
	return curTime;
}

float Time::deltaTime()
{
	lastDeltaTime = programTime() - lastTime;
	lastTime = programTime();
	return lastDeltaTime;
}

void Time::resetTime()
{
	lastTime = 0;
}

void Time::setTime(float newTime)
{
	lastTime = newTime;
}
