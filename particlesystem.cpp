#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(void)
{
	//default boundaries
	m_bmin = glm::vec3(-20.0f, -20.0f, -20.0f);
	m_bmin = glm::vec3(20.0f, 20.0f, 20.0f);
	//default particles
	pnum = 50;
	lastTime = 0;

}


ParticleSystem::~ParticleSystem(void)
{

}

void ParticleSystem::setPnum(int n)
{
	pnum = n;
}

void ParticleSystem::setBoundary(glm::vec3 bmin, glm::vec3 bmax)
{
	m_bmin = bmin;
	m_bmax = bmax;
}



void ParticleSystem::simulate(float dt, glm::vec3 center, glm::vec3 aux)
{
	gravitate(center, aux);
	//repel();
	for(int i=0; i< pnum;i++)
	{
		//integrate
		*(mVel+i) += dt * *(mAcc+i);
		*(mPos+i) += dt * *(mVel+i); 
		

		//check for boundary conditions
		if((mPos+i)->x > m_bmax.x)
			(mPos+i)->x = m_bmin.x;
		if((mPos+i)->x < m_bmin.x)
			(mPos+i)->x = m_bmax.x;

		if((mPos+i)->y > m_bmax.y)
			(mPos+i)->y = m_bmin.y;
		if((mPos+i)->y < m_bmin.y)
			(mPos+i)->y = m_bmax.y;
		
		if((mPos+i)->z > m_bmax.z)
			(mPos+i)->z = m_bmin.z;
		if((mPos+i)->z < m_bmin.z)
			(mPos+i)->z = m_bmax.z;
			
	}
}

void ParticleSystem::initParticles()
{
	//Randomize particle positions
	//initialize velocity and randomize acceleration for each particle
	if(isRandPos)
	{
		for(int i = 0; i < pnum; i++)
		{
			*(mPos+i) = randVec(m_bmin, m_bmax); 	
			*(mVel+i) = glm::vec3(0,0,0);
			*(mAcc+i) = randVec(glm::vec3(-0.2, -0.2, -0.2), glm::vec3(0.2, 0.2, 0.2));
			*(mass+i) = randomize(1, 20);
			*(color+i) = randVec(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1., 0.2, 1.0));
			
		}

	}
	//Place on a grid
	else{
		for(int i = 0; i<pnum;i++)
		{
			glm::vec3 p = glm::vec3();
			p.x = 5; 
			p.y = 0; 
			p.z = -0; 
			*(mPos+i) = p;
		}
	}
	

}


void ParticleSystem::initMemory()
{
	mPos = (glm::vec3*) malloc(pnum*sizeof(glm::vec3));
	mVel = (glm::vec3*) malloc(pnum*sizeof(glm::vec3));
	mAcc = (glm::vec3*) malloc(pnum*sizeof(glm::vec3));
	color = (glm::vec3*) malloc(pnum*sizeof(glm::vec3));
	mass = (float*) malloc(pnum*sizeof(float));
	force = (float*) malloc(pnum*sizeof(float));
}

void ParticleSystem::setRandPos(bool val)
{
	isRandPos = val;
}

bool ParticleSystem::RandPos()
{
	return isRandPos; 
}


void ParticleSystem::gravitate(glm::vec3 center, glm::vec3 aux)
{
	float M = 100;   /// ----> Mass of the mouse pointer in kg
	for(int i = 0; i<pnum; i++)
	{
		glm::vec3 temp = center - *(mPos+i);
		float rSquared = glm::dot(temp, temp); 
		float rSquareRoot = sqrt(rSquared);
		float f = *(mass+i) * M / rSquared; 


		glm::vec3 newAcc = glm::vec3();

		float m = *(mass+i) / 100;

		newAcc =  glm::normalize(temp);
		
		*(mAcc+i) = newAcc;
	}
}

void ParticleSystem::repel()
{
	for(int i = 0; i < pnum ; i++)
	{
		for(int j = 0 ; j < pnum ; j++)
		{
			if(i != j)
			{
				glm::vec3 v1 = *(mPos+i);
				glm::vec3 v2 = *(mPos+j);
				glm::vec3 temp = v2 - v1;
				float rSquared = glm::dot(temp, temp);
				float rSquareRoot = sqrt(rSquared);

				float f = *(mass+i) * *(mass+j) / rSquared; 

				f /= *(mass+i);

				*(mAcc+i) = f * glm::normalize(temp);
			}
				
		}
	}
}
