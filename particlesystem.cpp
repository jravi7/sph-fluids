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
			//glm::vec3 p = glm::vec3((i+1)*spacing,(j+1)*spacing,0);
			glm::vec3 p = glm::vec3(10, 15, 0) * rand;
			glm::vec3 v = speed * dir * rand;
			glm::vec3 a = glm::vec3(0,-1.8,0);
			glm::vec3 f = glm::vec3(0,0,0);
			glm::vec3 c = randVec(glm::vec3(0,0,0), glm::vec3(1,1,1));
			float m = 0.02; //kg
			std::cout<<std::endl;
			std::cout<<"Particle "<<count+1<<std::endl;
			
			mHashTable[computeHash(p)].push_back(count);

			mPos.push_back(p);
			mVel.push_back(v);
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




void ParticleSystem::simulate(float dt, glm::vec3 center, bool isPause)
{
	if(!isPause)
	{
		for(int i=0; i< mPos.size();i++)
		{
			//integrate
			mVel[i] += dt * (mAcc[i]);
			mPos[i] += dt * (mVel[i]);
	//		print(mPos[i]);
	
			mAcc[i] = glm::vec3(0,0,0);

			//check for boundary conditions
			if(mPos[i].x > m_bmax.x)
				mVel[i].x *= -1;
			if(mPos[i].x < m_bmin.x)
				mVel[i].x *= -1;

			if(mPos[i].y > m_bmax.y)
				mVel[i].y *= -1;
			if(mPos[i].y < m_bmin.y)
				mVel[i].y *= -1;
		
			/*if(mPos[i].z > m_bmax.z)
				mVel[i].z *= -0.9;
			if(mPos[i].z < m_bmin.z)
				mVel[i].z *= -0.9;
				*/
		}
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
