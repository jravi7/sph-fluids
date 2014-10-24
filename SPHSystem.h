#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm\glm.hpp>
#include <iostream>



class SPHSystem
{
public:
	SPHSystem(int count);
	~SPHSystem(void);
	void render(int pos_loc);
	void check();
	void setBoundary(glm::vec3 min, glm::vec3 max);

private:
	void initializePositions();
	void initBuffers();
	void drawBoundary();
	
	std::vector<glm::vec3> mP;
	std::vector<glm::vec3> mV;
	std::vector<glm::vec3> mA;
	std::vector<glm::vec3> mVprev;
	std::vector<float>	   mPressure;
	std::vector<float>	   mDensity;
	int					   mN;


	//Grid
	std::vector<glm::vec3> mBox;
	glm::vec3 mMin;
	glm::vec3 mMax;

	//buffers
	GLuint m_vbo;

};

