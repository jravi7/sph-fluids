#include "SPHSystem.h"


SPHSystem::SPHSystem(int count){

	mN = count;
	initializePositions();
	initBuffers();

}
SPHSystem::~SPHSystem(void){}


void SPHSystem::initializePositions()
{

}

void SPHSystem::setBoundary(glm::vec3 min, glm::vec3 max)
{
	mMin = min;
	mMax = max;
}



void SPHSystem::initBuffers()
{
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void SPHSystem::check()
{
	GLenum err = glGetError();
	std::cout<<gluErrorString(err)<<std::endl;
}

void SPHSystem::render(int pos_loc)
{
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);	
	glBufferData(GL_ARRAY_BUFFER, mP.size()*sizeof(glm::vec3), &mP[0], GL_STREAM_DRAW);
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_POINTS, 0, mP.size());
	
	

}