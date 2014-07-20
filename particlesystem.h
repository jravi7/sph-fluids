#pragma once
#define _USE_MATH_DEFINES
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include "util.h"
#include <math.h>

struct Wall{
	float nx; 
	float ny; 
	float c;
};

class ParticleSystem
{
public:
	ParticleSystem(glm::vec3 bmin, glm::vec3 bmax, int n);
	~ParticleSystem(void);
	void update(float dt, glm::dvec3 center, bool isPause);
	void render(int pos_loc, int color_loc);
	int computeHash(glm::vec3 p);
private:
	void init();
	void initbuffers();
	void print(glm::vec3);
	std::vector<int> neighbours(glm::vec3 p);
	void clearForces();
	void updateGrid();
	void computeDensityPressure();
	void computePressureForce();
	void computeViscousForce();
	void computeGravityForce();
	void computeRepulsion();
	void step(float dt);
	void initWalls();
	void checkEdges(float dt);
	//Invisible boundary of the particle system
	glm::vec3 m_bmin;					//boundary min
	glm::vec3 m_bmax;					//boundary max
	int mN;							//NxN grid of particles
	std::vector<glm::vec3> mPos;
	std::vector<glm::dvec3> mVel;
	std::vector<glm::dvec3> mVelPrev;
	std::vector<glm::dvec3> mAcc;
	std::vector<glm::vec3> mcolor;
	std::vector<glm::dvec3> mforce;
	std::vector<double> m_density; 
	std::vector<double> m_pressure; 
	std::vector<float> mass; 
	std::vector<Wall> walls;
	glm::ivec3 m_domain;
	double m_wPolyKernel;
	double m_wSpikyKernel;
	double m_LaplaceKernel;

	//Spatial Grid
	std::vector<std::vector<int>> mHashTable;
	std::vector<std::vector<int>> mZeroTable;

	//Buffers
	GLuint vbo;
	GLuint cbo;

	//SPH Params; 
	float mSupportRadius;	//	0.00459
	float mSimScale;		// 0.004

};

