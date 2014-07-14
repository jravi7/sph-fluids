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

	mSupportRadius = 0.0457;		//h
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
			//glm::vec3 p = glm::vec3(50, 80, 0);
			glm::dvec3 p = glm::dvec3(2+(i*5), 2+(j*5), 0);
			glm::dvec3 v = glm::dvec3(rand1(rand1(-6.6, 0), rand1(0, 4.5)), rand1(0, -4.5), 0);
			glm::dvec3 a = glm::dvec3(0, -9.8, 0);
			glm::dvec3 vprev = v - (0.5f * 0.003 * a); 
			glm::dvec3 f = glm::dvec3(0, 0, 0);
			glm::vec3 c = glm::vec3(rand1(0.2,0.6), rand1(0.1, 0.9), rand1(0.7, 0.9));
			float m = 0.04; //kg	using eq. 5.13(Kelager)

			std::cout<<std::endl;
			std::cout<<"Particle "<<count+1<<std::endl;
			
			mHashTable[computeGridHash(p)].push_back(count);

			mPos.push_back(p);
			mVel.push_back(v);
			mVelPrev.push_back(vprev);
			mAcc.push_back(a);
			m_density.push_back(0);
			m_pressure.push_back(0);
			mforce.push_back(f);
			mass.push_back(m);
			mcolor.push_back(c);
			count++;
		}
	}

	//neighbor search
	std::cout<<std::endl<<std::endl;
	std::cout<<"neighbor of particle 8"<<std::endl;
	neighbours(mPos[7]);


}

int ParticleSystem::computeHash(glm::dvec3 p)
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

int ParticleSystem::computeGridHash(glm::dvec3 p)
{
	float wCellSize = mSupportRadius / mSimScale;
	int x = floor(p.x / wCellSize);
	int y = floor (p.y / wCellSize);

	int hash = x + 100 * y; 

	return hash; 
}

void ParticleSystem::neighbours(glm::dvec3 p)
{
	std::vector<int> list; 
	float rad = mSupportRadius/mSimScale;

	glm::vec3 min = glm::vec3(p.x-rad,p.y-rad, 0);
	glm::vec3 max = glm::vec3(p.x+rad,p.y+rad, 0);

	for(float x = min.x; x <= max.x; x+=rad)
	{
		for(float y = min.y; y <= max.y; y+=rad)
		{
			glm::dvec3 pp = glm::dvec3(x,y,0);

			print(pp);
			int hash = computeGridHash(pp);
			std::cout<<"Neighbouring cell: "<<hash<<std::endl;
			if(hash > -1){
			std::vector<int> cell = mHashTable[hash];
			for(int i = 0; i < cell.size(); i++)
			{

				std::cout<<(cell[i]+1)<<std::endl;
				list.push_back(cell[i]);
			}
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




void ParticleSystem::update(float dt, glm::dvec3 center, bool isPause)
{

	//apply forces; 
	//update velocity and position
	//checkedges
	if(!isPause)
	{
		
		computeDensityPressure();
		computePressureForce();
		computeViscousForce();
		for(int i=0; i< mPos.size();i++)
		{
			glm::dvec3 vnext = mVelPrev[i] + double(dt) * mAcc[i];
			mVel[i] = 0.5 * ( mVelPrev[i] + vnext );
			mVelPrev[i] = vnext;

			//integrate
			//mVel[i] += dt * (mAcc[i]);
			mPos[i] += double(dt) * (vnext);
			//std::cout<<"Particle "<<i<<std::endl;
		//	print(mPos[i]);
		
	
			//mAcc[i] *= 0.f;

			
		}
		checkEdges();
		
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



void ParticleSystem::computeDensityPressure()
{
	m_wPolyKernel = 315.f/(64.f * M_PI * pow(mSupportRadius, 9.f));

	double h2 = mSupportRadius * mSupportRadius;

	for(int i = 0 ; i < mPos.size() ; i++)
	{
		double sum = 0; 
		for(int j = 0 ; j < mPos.size() ; j++)
		{
			if(i!=j)
			{
				glm::dvec3 ri = mPos[i] * double(mSimScale);
				glm::dvec3 rj = mPos[j] * double(mSimScale);
				double r = glm::length(ri-rj);
				double r2 = r * r;
				if(mSupportRadius > r){
					sum += (mass[j] * pow((h2 - r2), 3));
				}
			}
		}
		double restDensity = 998.29;
		m_density[i] = sum * m_wPolyKernel;
		m_pressure[i] = (m_density[i] - restDensity) * 3.f;

	}

}


void ParticleSystem::computePressureForce()
{
	m_wSpikyKernel = -45.f / (M_PI * pow(mSupportRadius, 6));
	for(int i = 0 ; i < mPos.size() ; i++)
	{
		glm::dvec3 sumPressureForce = glm::dvec3(0.0,0.0,0.0);
		for(int j = 0; j < mPos.size() ; j++)
		{
			if(i != j)
			{
				glm::dvec3 ri = mPos[i] * double(mSimScale);
				glm::dvec3 rj = mPos[j] * double(mSimScale);
				glm::dvec3 r = ri-rj;
				double rmag = glm::length(r);
				if(mSupportRadius > rmag){
					sumPressureForce += -((m_pressure[i] + m_pressure[j]) / 2) * (1.0 / m_density[j]) * (r/rmag) * pow((mSupportRadius-rmag), 2);
				}
			}
		}

		mforce[i] += sumPressureForce * m_wSpikyKernel * double(mass[i]);
	}
}

void ParticleSystem::computeViscousForce()
{
	m_LaplaceKernel = 45.f / (M_PI * pow(mSupportRadius, 6));
	double viscosity = 0.2;
	for(int i = 0 ; i < mPos.size() ; i++)
	{
		glm::dvec3 sumViscousForce = glm::dvec3(0.0,0.0,0.0);
		for(int j = 0; j < mPos.size() ; j++)
		{
			if(i != j)
			{
				glm::dvec3 ri = mPos[i] * double(mSimScale);
				glm::dvec3 rj = mPos[j] * double(mSimScale);
				glm::dvec3 r = ri-rj;
				double rmag = glm::length(r);
				if(mSupportRadius > rmag){
					sumViscousForce += (mVel[j] - mVel[i]) * (mSupportRadius - rmag);
				}
			}
		}

		mforce[i] += (m_LaplaceKernel * viscosity * mass[i] )/m_density[i] * sumViscousForce;
	}
}



void ParticleSystem::print(glm::dvec3 p)
{
	std::cout<<"x:"<<p.x<<" y:"<<p.y<<" z:"<<p.z<<std::endl;
}

ParticleSystem::~ParticleSystem(void)
{

}
