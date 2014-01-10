#pragma once
#include <glm/glm.hpp>
#include "Particle.h"
#include <vector>
#include <GL/glew.h>
#include "util.h"


class ParticleSystem
{
public:
	ParticleSystem(void);
	~ParticleSystem(void);

	void setBoundary(glm::vec3 bmin, glm::vec3 bmax);

	void simulate(float dt, glm::vec3 center, glm::vec3 aux);
	void setPnum(int);
	void initMemory();
	void initParticles();
	void setRandPos(bool);
	bool RandPos();




	//Invisible boundary of the particle system
	bool isRandPos;			//Should init particle positions be randomized or placed on a grid? 
	glm::vec3 m_bmin;					//boundary min
	glm::vec3 m_bmax;					//boundary max
	int pnum;							//Number of particles
	glm::vec3 *mPos;
	glm::vec3 *mVel;
	glm::vec3 *mAcc;
	glm::vec3 *color;
	float *mass; 
	float *force; 
	float dt; 
	float lastTime; 

private:
	void gravitate(glm::vec3 center, glm::vec3 aux);
	void repel();
	
};

