#pragma once
#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <vector>
#include <glm\glm.hpp>
#include <iostream>
#include "camera.h"
#include "Misc.h"
#include "util.h"
#include <math.h>


#define WORLD_SIZE 1.28
#define SMOOTH_LENGTH 0.04
#define CELL_SIZE SMOOTH_LENGTH
#define GRID_SIZE (WORLD_SIZE/CELL_SIZE)
#define MAX_PARTICLES 512

#define FLUID_MASS 1.98
#define PARTICLE_MASS (FLUID_MASS/MAX_PARTICLES)
#define REST_DENSITY 1000
#define GRAVITY -1.8
#define VISCOSITY 6.5
#define GAS_CONSTANT 1.0
#define DELTA_T 0.003

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
	SPHSystem();
	~SPHSystem(void);
	void init();
	void render(int pos_loc, Camera &cam);
	void update();
	void checkBoundary();
	void computeDensity();
	void computePressureForce();
	void step();

private:
	void initializePositions();
	void initBuffers();
	void initGrid();
	void initKernels();
	void drawBoundary(Camera &cam);
	int  computeHash(glm::vec3 p);
	void updateGrid();
	void addToGrid(int hash, int index);
	void neighbours(glm::vec3 p, int* list, int &count);
	void neighbourSearch();
	void clearAcceleration();

	std::vector<glm::vec3> mP;
	std::vector<glm::vec3> mV;
	std::vector<glm::vec3> mA;
	std::vector<glm::vec3> mVprev;
	std::vector<glm::vec3> mForce;
	std::vector<float>	   mPressure;
	std::vector<float>	   mDensity;


	
	//kernels
	float mPolyKernel;
	float mSpikyKernel;
	float mViscousKernel;

	//Spatial Grid
	HashTable* sg;
	

	//Draw Boundary 
	std::vector<glm::vec3> mBox;
	glm::vec3 mMin;
	glm::vec3 mMax;

	//buffers
	GLuint m_vbo;

	//total number of particles currently within world boundary
	int  TOTAL_PARTICLES;

};

