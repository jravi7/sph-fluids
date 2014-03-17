#include "springSystem.h"


SpringSystem::SpringSystem(int n)
{
	count = n	;
	init();
	initbuffers();
}

void SpringSystem::init()
{
	p.push_back(glm::vec3(0,0,0));
	p.push_back(glm::vec3(0,-20,0));
	v.push_back(glm::vec3(0,0,0));
	v.push_back(glm::vec3(0,0,0));
	a.push_back(glm::vec3(0,0,0));
	a.push_back(glm::vec3(0,0,0));
}

void SpringSystem::initbuffers()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpringSystem::render(int pos_loc)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, p.size()*sizeof(glm::vec3), &p[0], GL_STREAM_DRAW);
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_POINTS, 0, p.size());
}

void SpringSystem::simulate(float dt)
{
	float rest = 20;			//rest length

	glm::vec3 spr = p[0]-p[1];

	float originalLength = spr.length();
	//spr = glm::normalize(spr);

	v[1] += glm::vec3(spr.x*0.5,spr.y*0.5,spr.z*0.5);

	v[1] = glm::vec3(0,1,0);

	p[1] += dt * v[1];
	
}

SpringSystem::~SpringSystem(void)
{

}
