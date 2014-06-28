#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(glm::vec3 bmin, glm::vec3 bmax,int n)
{
	m_bmin = bmin;
	m_bmax = bmax;
	mN = n;

	PRIME1 = 73856093; 
	PRIME2 = 19349663; 
	PRIME3 = 83492791; 
	PRIME2N =  3203;

	mHashTable.resize(PRIME2N);

	mSupportRadius = 0.0457;
	mSimScale = 0.004;

	init();
	initbuffers();
}

void ParticleSystem::init()
{
	int count = 0;
	for(int i = 0; i < mN; i++)
	{
		for(int j = 0; j < mN ; j++)
		{
			float spacing = mSupportRadius/mSimScale*0.1;
			float speed = 5.f;
			glm::vec3 dir = glm::vec3(34, 45, 0);
			dir = glm::normalize(dir);
			float rand = rand1(0.9, 1.4f);
			glm::vec3 p = glm::vec3(50, 80, 0);
			glm::vec3 v = glm::vec3(rand1(rand1(-6.6, 0), rand1(0, 4.5)), rand1(0, -4.5), 0);
			glm::vec3 a = glm::vec3(0, -9.8, 0);
			glm::vec3 vprev = v - (0.5f * 0.003f * a); 
			glm::vec3 f = glm::vec3(0, 0, 0);
			glm::vec3 c = glm::vec3(rand1(0.2,0.6), rand1(0.1, 0.9), rand1(0.7, 0.9));
			float m = rand1(0.9, 2);
			std::cout<<std::endl;
			std::cout<<"Particle "<<count+1<<std::endl;
			
//			mHashTable[computeHash(p)].push_back(count);

			mPos.push_back(p);
			mVel.push_back(v);
			mVelPrev.push_back(vprev);

			mAcc.push_back(a);
			mforce.push_back(f);
			mass.push_back(m);
			mcolor.push_back(c);
			count++;
		}
	}

	//neighbor search
	std::cout<<std::endl<<std::endl;
	std::cout<<"neighbor of particle 8"<<std::endl;
	//neighbours(mPos[7]);


}

int ParticleSystem::computeHash(glm::vec3 p)
{
	float cellsize = mSupportRadius; //l = h
	float wCellsize = mSupportRadius/mSimScale; 
	
	//compute discretized 3d coordinates
	int x = ceil(p.x/wCellsize);
	int y = ceil(p.y/wCellsize);
	int z = ceil(p.z/wCellsize);

	long xPrime1 = x*PRIME1; 
	long yPrime2 = y*PRIME2;
	long zPrime3 = z*PRIME3;

	long xyz = xPrime1 ^ yPrime2 ^ zPrime3;
	
	int hashVal = xyz % PRIME2N;

	print(p);
	std::cout<<"Hash Value: "<<(hashVal)<<std::endl;
	
	return (hashVal);
}

void ParticleSystem::neighbours(glm::vec3 p)
{
	std::vector<int> list; 
	float rad = mSupportRadius/mSimScale;

	glm::vec3 min = glm::vec3(p.x-rad,p.y-rad, 0);
	glm::vec3 max = glm::vec3(p.x+rad,p.y+rad, 0);

	for(float x = min.x; x <= max.x; x+=rad)
	{
		for(float y = min.y; y <= max.y; y+=rad)
		{
			glm::vec3 pp = glm::vec3(x,y,0);

			print(pp);
			int hash = computeHash(pp);
			std::cout<<"Neighbouring cell: "<<hash<<std::endl;
			std::vector<int> cell = mHashTable[hash];
			for(int i = 0; i < cell.size(); i++)
			{

				std::cout<<(cell[i]+1)<<std::endl;
				list.push_back(cell[i]);
			}
		}
	}
	std::cout<<"Number of neighbours ";
	std::cout<<list.size();
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
	glBufferData(GL_ARRAY_BUFFER, mPos.size()*sizeof(glm::vec3), &mPos[0], GL_STREAM_DRAW);
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, mPos.size()*sizeof(glm::vec3), &mcolor[0], GL_STREAM_DRAW);
	glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_POINTS, 0, mPos.size());
}




void ParticleSystem::update(float dt, glm::vec3 center, bool isPause)
{

	//apply forces; 
	//update velocity and position
	//checkedges
	if(!isPause)
	{
		
		applyRepel();
		applyForces();
		for(int i=0; i< mPos.size();i++)
		{
			glm::vec3 vnext = mVelPrev[i] + dt * mAcc[i];
			mVel[i] = 0.5f * ( mVelPrev[i] + vnext );
			mVelPrev[i] = vnext;

			//integrate
			//mVel[i] += dt * (mAcc[i]);
			mPos[i] += dt * (vnext);
			//std::cout<<"Particle "<<i<<std::endl;
		//	print(mPos[i]);
		
	
			//mAcc[i] *= 0.f;

			
		}
		checkEdges();
		
	}
}

void ParticleSystem::applyRepel()
{
	glm::vec3 center = glm::vec3(50, 20, 0);

	for(int i=0; i< mPos.size();i++)
	{
		glm::vec3 d = center - mPos[i];
		float mag = glm::length(d);
	
		d = glm::normalize(d);
		float force = - 1 * 100 / (mag * mag);
		d = d * force; 

		mforce[i] = d/mass[i];
	}
}

void ParticleSystem::applyForces()
{
	glm::vec3 gravity = glm::vec3(0,-9.8, 0);
	glm::vec3 wind  = glm::vec3(0, 5, 0);
	for(int i=0; i< mPos.size();i++)
	{
		mforce[i] += (wind + gravity) / mass[i]; 
		mAcc[i] = mforce[i];
	}
}

void ParticleSystem::checkEdges()
{
	for(int i=0; i< mPos.size();i++)
		{
			//check for boundary conditions
			if(mPos[i].x > m_bmax.x)
			{
				mVelPrev[i].x *= -1;
				mPos[i].x = m_bmax.x;
			}
			if(mPos[i].x < m_bmin.x)
			{
				mVelPrev[i].x *= -1;
				mPos[i].x = m_bmin.x;
			}

			if(mPos[i].y > m_bmax.y)
			{
				mVelPrev[i].y *= -1;
				mPos[i].y =m_bmax.y;
			}
			if(mPos[i].y < m_bmin.y)
			{
				mVelPrev[i].y *= -1;
				mPos[i].y =m_bmin.y;
			}
		
			/*if(mPos[i].z > m_bmax.z)
			{
				mVel[i].z *= -0.9;
				mPos[i].z = m_bmax.z;
			}
			if(mPos[i].z < m_bmin.z)
			{
				mVel[i].z *= -0.9;
				mPos[i].z = m_bmin.z;
			}*/
				
		}
}



void ParticleSystem::computeDensity()
{

}

void ParticleSystem::print(glm::vec3 p)
{
	std::cout<<"x:"<<p.x<<" y:"<<p.y<<" z:"<<p.z<<std::endl;
}

ParticleSystem::~ParticleSystem(void)
{

}
