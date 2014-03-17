#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(glm::vec3 bmin, glm::vec3 bmax, int n)
{
	m_bmin = bmin;
	m_bmax = bmax;
	count = n;
	init();
	initbuffers();
}

void ParticleSystem::init()
{
	//initialize velocity and randomize acceleration for each particle
	for(int i = 0; i < count; i++)
	{
		glm::vec3 p = randVec(m_bmin, m_bmax); 	
		glm::vec3 v = glm::vec3(0,0,0);
		glm::vec3 a = glm::vec3(0,0,0);
		glm::vec3 f = glm::vec3(0,0,0);
		glm::vec3 c = randVec(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0));
		float m = randomize(1, 100);

		mPos.push_back(p);
		mVel.push_back(v);
		mAcc.push_back(a);
		mforce.push_back(f);
		mass.push_back(m);
		mcolor.push_back(c);
	}
}

void ParticleSystem::initbuffers()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::render(int pos_loc, int color_loc)
{	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, count*sizeof(glm::vec3), &mPos[0], GL_STREAM_DRAW);
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, count*sizeof(glm::vec3), &mcolor[0], GL_STREAM_DRAW);
	glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_POINTS, 0, mPos.size());
}




void ParticleSystem::simulate(float dt, glm::vec3 center)
{
	for(int i=0; i< count;i++)
	{
		gravitate(center, mPos[i], mAcc[i], mforce[i], mass[i]);
		
		//integrate
		mVel[i] += dt * (mAcc[i]);
		mPos[i] += dt * (mVel[i]);
		
		mAcc[i] = glm::vec3(0,0,0);

		//check for boundary conditions
		if(mPos[i].x > m_bmax.x)
			mVel[i].x *= -0.9;
		if(mPos[i].x < m_bmin.x)
			mPos[i].x *= -0.9;

		if(mPos[i].y > m_bmax.y)
			mPos[i].y *= -0.9;
		if(mPos[i].y < m_bmin.y)
			mPos[i].y *= -0.9;
		
		if(mPos[i].z > m_bmax.z)
			mPos[i].z *= -0.9;
		if(mPos[i].z < m_bmin.z)
			mPos[i].z *= -0.9;
			
	}
}

void ParticleSystem::print(glm::vec3 p)
{
	std::cout<<"x:"<<p.x<<"y:"<<p.y<<"z:"<<p.z<<std::endl;
}

void ParticleSystem::gravitate(glm::vec3 center, const glm::vec3 &p, glm::vec3 &a, glm::vec3 &f, const float &m)
{
	float M = 1000;   /// ----> Mass of the mouse pointer in kg

	glm::vec3 temp = center - p;
	float rSquared = glm::dot(temp, temp); 
	float rSquareRoot = sqrt(rSquared);
	float mag = M / rSquared; 
	glm::normalize(temp);
	f = temp * mag;
	a = f/m;
}

ParticleSystem::~ParticleSystem(void)
{

}
