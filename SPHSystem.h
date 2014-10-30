#pragma once
#include <GL/glew.h>
#include <vector>
#include <glm\glm.hpp>
#include <iostream>
#include "camera.h"
#include "Misc.h"
#include "util.h"

#define NUM_PARTICLES 2000

struct Particle
{
	unsigned int n;
	Particle* next;
};

struct HashTable{
	int size; 
	Particle** first; 
};

class SPHSystem
{
public:
	SPHSystem(int count);
	~SPHSystem(void);
	void init();
	void render(int pos_loc, Camera &cam);
	void check();
	void setBoundary(glm::vec3 min, glm::vec3 max);
	void update();
	void checkBoundary();
	void step();

private:
	void initializePositions();
	void initBuffers();
	void drawBoundary(Camera &cam);
	int computeHash(glm::vec3 p);
	void updateGrid();
	void addToGrid(int hash, int index);
	void addToStack(Particle* &head, int index);
	void neighbours(glm::vec3 p, int* list, int &count);
	
	std::vector<glm::vec3> mP;
	std::vector<glm::vec3> mV;
	std::vector<glm::vec3> mA;
	std::vector<glm::vec3> mVprev;
	std::vector<float>	   mPressure;
	std::vector<float>	   mDensity;
	int					   mN;


	//SPH Params
	float mSmoothingLength;
	float mParticleMass;
	float mRestDensity;
	

	//Spatial Grid
	HashTable* sg;
	float	mCellSize;
	

	//Draw Boundary 
	std::vector<glm::vec3> mBox;
	glm::vec3 mMin;
	glm::vec3 mMax;

	//buffers
	GLuint m_vbo;

};

