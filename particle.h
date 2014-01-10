#pragma once
#include <glm/glm.hpp>


class Particle
{
public:
	Particle(void);
	~Particle(void);
	glm::vec3 pos; 
	glm::vec3 vel; 
	glm::vec3 acc; 
	float mass;			// in kg
	float force;			// in N
};

