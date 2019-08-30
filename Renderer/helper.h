#pragma once
#include <string>
using std::string;

class Time
{
private:
	float lastTime;
	float lastDeltaTime;      // time at the end of the last frame

public:
	float programTime();       // time since start of the program
	float systemTime(); // get current real-world time
	float deltaTime();  // time between frames

	void tick();			  // increment time
	void resetTime();         // reset time to zero again
	void setTime(float newTime);      // set time to a new value
};

string loadShaderFile(string shaderName);