#pragma once
#define _USE_MATH_DEFINES
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include "util.h"
#include <math.h>

class ParticleSystem
{
public:
	ParticleSystem(glm::vec3 bmin, glm::vec3 bmax, int n);
	~ParticleSystem(void);
	void update(float dt, glm::dvec3 center, bool isPause);
	void render(int pos_loc, int color_loc);
	int computeHash(glm::dvec3 p);
	int computeGridHash(glm::dvec3 p);


	

private:
	void init();
	void initbuffers();
	void print(glm::dvec3);
	void gravitate(glm::dvec3 center, const glm::dvec3 &p, glm::dvec3 &a, glm::dvec3 &f, const float &m);
	void neighbours(glm::dvec3 p);
	void clearForces();
	void computeDensityPressure();
	void computePressureForce();
	void computeViscousForce();
	void computeGravityForce();
	void step(float dt);
	void checkEdges();
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
	double m_wPolyKernel;
	double m_wSpikyKernel;
	double m_LaplaceKernel;

	//Spatial Grid
	std::vector<std::vector<int>> mHashTable;

	//Buffers
	GLuint vbo;
	GLuint cbo;

	//very large prime numbers. 
	int PRIME1  ; 
	int PRIME2  ; 
	int PRIME3  ; 
	int PRIME2N  ;


	//SPH Params; 
	float mSupportRadius;	//	0.00459
	float mSimScale;		// 0.004



	
};

