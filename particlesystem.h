#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include "util.h"


class ParticleSystem
{
public:
	ParticleSystem(glm::vec3 bmin, glm::vec3 bmax, int n);
	~ParticleSystem(void);
	void simulate(float dt, glm::vec3 center, bool isPause);
	void render(int pos_loc, int color_loc);
	int computeHash(glm::vec3 p);



	

private:
	void init();
	void initbuffers();
	void print(glm::vec3);
	void gravitate(glm::vec3 center, const glm::vec3 &p, glm::vec3 &a, glm::vec3 &f, const float &m);
	void neighbours(glm::vec3 p);
	void computeDensity();
	//Invisible boundary of the particle system
	glm::vec3 m_bmin;					//boundary min
	glm::vec3 m_bmax;					//boundary max
	int mN;							//NxN grid of particles
	std::vector<glm::vec3> mPos;
	std::vector<glm::vec3> mVel;
	std::vector<glm::vec3> mAcc;
	std::vector<glm::vec3> mcolor;
	std::vector<glm::vec3> mforce;
	std::vector<float> mass; 

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

