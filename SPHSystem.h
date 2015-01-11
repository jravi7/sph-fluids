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
#define NUM_PARTICLES 1000

#define FLUID_MASS 1.98
#define PARTICLE_MASS (FLUID_MASS/NUM_PARTICLES)
#define REST_DENSITY 1000
#define GRAVITY -1.8
#define VISCOSITY 6.5
#define GAS_CONSTANT 1.0
#define DELTA_T 0.003
#define HASHTABLE_SIZE ( * 2)

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
	void check();
	void setBoundary(glm::vec3 min, glm::vec3 max);
	void update();
	void checkBoundary();
	void computeDensity();
	void computePressureForce();
	void step();

private:
	void initializePositions();
	void initBuffers();
	void initSPHParams();
	void drawBoundary(Camera &cam);
	int  computeHash(glm::vec3 p);
	void updateGrid();
	void addToGrid(int hash, int index);
	void neighbours(glm::vec3 p, int* list, int &count);
	void clearAcceleration();

	std::vector<glm::vec3> mP;
	std::vector<glm::vec3> mV;
	std::vector<glm::vec3> mA;
	std::vector<glm::vec3> mVprev;
	std::vector<glm::vec3> mForce;
	std::vector<double>	   mPressure;
	std::vector<double>	   mDensity;


	//SPH Params
	float mSmoothingLength;
	float mParticleMass;
	float mRestDensity;
	float mDeltaT;		//time step
	

	//Spatial Grid
	HashTable* sg;
	

	//Draw Boundary 
	std::vector<glm::vec3> mBox;
	glm::vec3 mMin;
	glm::vec3 mMax;

	//buffers
	GLuint m_vbo;

};

