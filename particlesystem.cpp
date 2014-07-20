#include "ParticleSystem.h"

#define EPSILON 0.000001

using namespace std;

ParticleSystem::ParticleSystem(glm::vec3 bmin, glm::vec3 bmax, int n)
{
	m_bmin = bmin;
	m_bmax = bmax;
	
	m_domain = glm::ivec3(m_bmax - m_bmin);
	mN = n;
	
	mSupportRadius = 0.0457;		//h
	mSimScale = 0.04;

	mZeroTable.resize(m_domain.x * m_domain.y);
	mHashTable.resize(m_domain.x * m_domain.y);

	init();
	initbuffers();
}

void ParticleSystem::init()
{
	int count = 0;
	float spacing = mSupportRadius/mSimScale;
	//particle initial position domain
	int w = 50; 
	int h = 50; 

	//mass calculations
	float m = 1000 * 0.1 / mN;

	for(int i = 0; i < w; i++)
	{
		for(int j = 0; j < h ; j++)
		{
		
			if(count < mN){
			glm::vec3 p = glm::vec3(10+(i*spacing), 10+(j*spacing), 0);
			glm::dvec3 v = double(mSimScale) * glm::dvec3(rand1(rand1(-6.6, 0), rand1(0, 4.5)), rand1(0, -4.5), 0);
			glm::dvec3 a = glm::dvec3(0, 0, 0);
			glm::dvec3 vprev = double(mSimScale) * glm::dvec3(rand1(rand1(-6.6, 0), rand1(0, 4.5)), rand1(0, -4.5), 0);
			glm::dvec3 f = glm::dvec3(0, 0, 0);
			glm::vec3 c = glm::vec3(rand1(0.2,0.6), rand1(0.1, 0.9), rand1(0.7, 0.9));
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

	initWalls();
}

void ParticleSystem::initWalls()
{
	Wall w1, w2, w3, w4; 

	//left
	w1.nx = 1.f; 
	w1.ny = 0.f;
	w1.c = 0.f; 

	//bottom; 
	w2.nx = 0.f; 
	w2.ny = 1.f; 
	w2.c = 0.f; 

	//right
	w3.nx = -1.f; 
	w3.ny = 0.f;
	w3.c = -m_domain.x;

	//top
	w4.nx = 0.f; 
	w4.ny = -1.f; 
	w4.c = -m_domain.y; 

	walls.push_back(w1);
	walls.push_back(w2);
	walls.push_back(w3);
	walls.push_back(w4);
}

int ParticleSystem::computeHash(glm::vec3 p)
{
	if(p.x < 0 || p.y < 0 || p.x > 100  || p.y > 100)
		cout<<"Out of bounds"<<std::endl;

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
					//std::cout<<(cell[i])<<std::endl;
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

	if(!isPause)
	{
		
		clearForces();
		//updateGrid();
		/*computeDensityPressure();
		computePressureForce();
		computeViscousForce();*/
		computeRepulsion();
		//computeGravityForce();
		//checkEdges(dt);
		step(dt);
	}
}

void ParticleSystem::step(float dt)
{
	for(int i = 0 ; i < mN ; i++)
	{
		mAcc[i] = mforce[i] / (double)mass[i];
		
		//acceleration magnitude
		double amag = glm::length(mAcc[i])/double(mSimScale);
		if(abs(amag) > 3.0)
			mAcc[i] *= 0.5;

		mVel[i] += mAcc[i] * double(dt);
		//glm::dvec3 vnext = mVelPrev[i] + double(dt) * mAcc[i];
		//mVel[i] = 0.5 * ( mVelPrev[i] + vnext );
		//mVelPrev[i] = vnext;
		mPos[i] += glm::vec3(double(dt) * (mVel[i]) );
	}
}

void ParticleSystem::computeRepulsion()
{
	for(int i = 0 ;  i < mN ; i++)
	{
			glm::vec3 dist = mPos[i] - glm::vec3(50.0, 50.0, 0);
			float distsq = glm::length(dist) * glm::length(dist);
			if(distsq < 0.1)
				distsq = 0.1;
			
				float f = mass[i] * 100 / distsq;
				glm::vec3 dir = -glm::normalize(dist);
				mforce[i] += dir * f; 

		
	}
}

void ParticleSystem::updateGrid()
{
	mHashTable.swap(mZeroTable);

	for(int i = 0 ; i < mN ; i++)
	{
		glm::vec3 p = mPos[i];
		mHashTable[computeHash(p)].push_back(i);
	}
}


void ParticleSystem::checkEdges(float dt)
{
	//Define collision restituition
	double restituition = 0.8;
	double h = mSupportRadius / mSimScale; 

	for(int i=0; i< mPos.size();i++)
		{

			for(int j = 0 ; j < walls.size() ; j++)
			{
				float dist = walls[j].nx * mPos[i].x + walls[j].ny * mPos[i].y - walls[j].c; 

				if(dist < h)
				{
					if(dist < EPSILON)
						dist = EPSILON; 
					mVel[i].x += (h - dist) * walls[j].nx / dt;
					mVel[i].y += (h - dist) * walls[j].ny / dt;
				}

			}
		}
}



void ParticleSystem::computeDensityPressure()
{
	double h = mSupportRadius;
	double h2 = h * h;
	double restDensity = 10.0;
	double gasStiffness = 3.0;

	m_wPolyKernel = 315.f / (64.f * M_PI * pow(h, 9));

	for(int i = 0 ; i < mN ; i++)
	{
		double sum = 0; 
		vector<int> neighbors = neighbours(mPos[i]);
		for(int j = 0 ; j < neighbors.size() ; j++)
		{
			if(i != neighbors[j])
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
	double h = mSupportRadius;
	m_wSpikyKernel = -45.f / (M_PI * pow(h, 6));
	for(int i = 0 ; i < mN; i++)
	{
		glm::dvec3 sumPressureForce = glm::dvec3(0.0,0.0,0.0);
		vector<int> neighbors = neighbours(mPos[i]);
		for(int j = 0; j < neighbors.size(); j++)
		{
			if(i != neighbors[j] )
			{
				glm::dvec3 ri = glm::dvec3(mPos[i]) * double(mSimScale);
				glm::dvec3 rj = glm::dvec3(mPos[neighbors[j]]) * double(mSimScale);
				glm::dvec3 r = ri-rj;
				double rmag = glm::length(r);
				if(h > rmag){
					sumPressureForce += -((m_pressure[i] + m_pressure[neighbors[j]]) / 2) * (1.0 / m_density[neighbors[j]]) * (r/rmag) * pow((h-rmag), 2);
				}
			}
		}

		mforce[i] += sumPressureForce * m_wSpikyKernel * double(mass[i]);
	}
}

void ParticleSystem::computeViscousForce()
{
	double h = mSupportRadius;
	m_LaplaceKernel = 45.f / (M_PI * pow(h, 6));
	double viscosity = 0.2;
	for(int i = 0 ; i < mPos.size() ; i++)
	{
		glm::dvec3 sumViscousForce = glm::dvec3(0.0,0.0,0.0);
		vector<int> neighbors = neighbours(mPos[i]);
		for(int j = 0; j < neighbors.size() ; j++)
		{
			if(i != j)
			{
				glm::dvec3 ri = glm::dvec3(mPos[i]) * double(mSimScale);
				glm::dvec3 rj = glm::dvec3(mPos[neighbors[j]]) * double(mSimScale);
				glm::dvec3 r = ri-rj;
				double rmag = glm::length(r);
				if(h > rmag){
					sumViscousForce += (mVel[neighbors[j]] - mVel[i]) * (h - rmag);
				}
			}
		}

		mforce[i] += (m_LaplaceKernel * viscosity * mass[i] )/m_density[i] * sumViscousForce;
	}
}

void ParticleSystem::computeGravityForce()
{
	glm::dvec3 g = glm::dvec3(0.0, -9.8, 0.0);

	for(int i = 0 ; i < mPos.size() ; i++)
	{
		mforce[i] += (double)mass[i] * g;
	}
}



void ParticleSystem::print(glm::vec3 p)
{
	std::cout<<"x:"<<p.x<<" y:"<<p.y<<" z:"<<p.z<<std::endl;
}

ParticleSystem::~ParticleSystem(void)
{

}
