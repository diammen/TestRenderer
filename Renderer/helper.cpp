#include "helper.h"
#include <fstream>
#include <iostream>
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
