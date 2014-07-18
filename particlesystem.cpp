#include "ParticleSystem.h"

using namespace std;

ParticleSystem::ParticleSystem(glm::vec3 bmin, glm::vec3 bmax, int n)
{
	m_bmin = bmin;
	m_bmax = bmax;
	
	m_domain = glm::ivec3(m_bmax - m_bmin);
	mN = n;
	
	mSupportRadius = 0.0457;		//h
	mSimScale = 0.04;

	init();
	initbuffers();
}

void ParticleSystem::init()
{
	int count = 0;
	float spacing = mSupportRadius/mSimScale;
	//particle initial position domain
	int w = 5; 
	int h = 5; 
	for(int i = 0; i < w; i++)
	{
		for(int j = 0; j < h ; j++)
		{
		
			if(count < mN){
			glm::vec3 p = glm::vec3(2+(i*spacing), 2+(j*spacing), 0);
			glm::dvec3 v = glm::dvec3(rand1(rand1(-6.6, 0), rand1(0, 4.5)), rand1(0, -4.5), 0);
			glm::dvec3 a = glm::dvec3(0, -9.8, 0);
			glm::dvec3 vprev = v - (0.5f * 0.003 * a); 
			glm::dvec3 f = glm::dvec3(0, 0, 0);
			glm::vec3 c = glm::vec3(rand1(0.2,0.6), rand1(0.1, 0.9), rand1(0.7, 0.9));
			float m = 0.04; 
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
	}

	/* terminate program if the vectors of particles are not
	of equal size */ 
	assert(mN==mPos.size()&&
		   mN==mVel.size()&&
		   mN==mVelPrev.size()&&
		   mN==mAcc.size()&&
		   mN==mforce.size()&&
		   mN==m_density.size()&&
		   mN==m_pressure.size()&&
		   mN==mass.size()&&
		   mN==mcolor.size());
}

int ParticleSystem::computeHash(glm::vec3 p)
{
	float wCellSize = mSupportRadius / mSimScale;
	int x = floor(p.x / wCellSize);
	int y = floor (p.y / wCellSize);
	int hash = x + 100 * y; 

	return hash; 
}

std::vector<int> ParticleSystem::neighbours(glm::vec3 p)
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
			int hash = computeHash(pp);
			if(hash > -1){
				std::vector<int> cell = mHashTable[hash];
				for(int i = 0; i < cell.size(); i++)
				{
					std::cout<<(cell[i])<<std::endl;
					list.push_back(cell[i]);
				}
			}
		}
	}
	return list; 
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


void ParticleSystem::clearForces()
{
	for(int i = 0 ; i < mPos.size() ; i++)
	{
		mforce[i].x = 0.0;
		mforce[i].y = 0.0;
		mforce[i].z = 0.0;
	}
}

void ParticleSystem::update(float dt, glm::dvec3 center, bool isPause)
{

	//apply forces; 
	//update velocity and position
	//checkedges
	if(!isPause)
	{
		
		//clearForces();
		updateGrid();
		computeDensityPressure();
		//computePressureForce();
		//computeViscousForce();
		//computeGravityForce();
		//checkEdges();
		//step(dt);
		
		
	}
}

void ParticleSystem::step(float dt)
{
	for(int i=0; i< mPos.size();i++)
	{
		glm::dvec3 vnext = mVelPrev[i] + double(dt) * mAcc[i];
		mVel[i] = 0.5 * ( mVelPrev[i] + vnext );
		mVelPrev[i] = vnext;
		mPos[i] += double(dt) * (vnext);
	}
}

void ParticleSystem::updateGrid()
{
	mHashTable.clear();
	mHashTable.resize(m_domain.x * m_domain.y);
	for(int i = 0 ; i < mN ; i++)
	{
		glm::vec3 p = mPos[i];
		mHashTable[computeHash(p)].push_back(i);
	}
}


void ParticleSystem::checkEdges()
{
	//Define collision restituition
	double restituition = 0.9;

	for(int i=0; i< mPos.size();i++)
		{
			//check for boundary conditions
			if(mPos[i].x > m_bmax.x)
			{
				mVelPrev[i].x *= -restituition;
				mPos[i].x = m_bmax.x;
			}
			if(mPos[i].x < m_bmin.x)
			{
				mVelPrev[i].x *= -restituition;
				mPos[i].x = m_bmin.x;
			}

			if(mPos[i].y > m_bmax.y)
			{
				mVelPrev[i].y *= -restituition;
				mPos[i].y =m_bmax.y;
			}
			if(mPos[i].y < m_bmin.y)
			{
				mVelPrev[i].y *= -restituition;
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
	double h = mSupportRadius;
	double h2 = h * h;
	double restDensity = 998.29;
	double gasStiffness = 3.0;

	m_wPolyKernel = 315.f / (64.f * M_PI * pow(h, 9));

	for(int i = 0 ; i < mN ; i++)
	{
		double sum = 0; 
		vector<int> neighbors = neighbours(mPos[i]);
		for(int j = 0 ; j < neighbors.size() ; j++)
		{
			if(i!=neighbors[j])
			{
				glm::dvec3 ri = glm::dvec3(mPos[i]) * double(mSimScale);
				glm::dvec3 rj = glm::dvec3(mPos[neighbors[j]]) * double(mSimScale);
				double r = glm::length(ri-rj);
				double r2 = r * r;
				if(h > r){
					sum += (mass[neighbors[j]] * pow((h2 - r2), 3));
				}
			}
		}
		m_density[i] = sum * m_wPolyKernel;
		m_pressure[i] = (m_density[i] - restDensity) * gasStiffness;
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
				glm::dvec3 ri = glm::dvec3(mPos[i]) * double(mSimScale);
				glm::dvec3 rj = glm::dvec3(mPos[j]) * double(mSimScale);
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
				glm::dvec3 ri = glm::dvec3(mPos[i]) * double(mSimScale);
				glm::dvec3 rj = glm::dvec3(mPos[j]) * double(mSimScale);
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

void ParticleSystem::computeGravityForce()
{
	glm::dvec3 g = glm::dvec3(0.9, -9.8, 0.0);

	for(int i = 0 ; i < mPos.size() ; i++)
	{
		mforce[i] += m_density[i] * g;
	}
}



void ParticleSystem::print(glm::vec3 p)
{
	std::cout<<"x:"<<p.x<<" y:"<<p.y<<" z:"<<p.z<<std::endl;
}

ParticleSystem::~ParticleSystem(void)
{

}
