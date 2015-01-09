#include "SPHSystem.h"

#define EPSILON 0.000001

SPHSystem::SPHSystem(int count){
	mMin = glm::vec3(0);
	mMax = glm::vec3(0);
}
SPHSystem::~SPHSystem(void){}

void SPHSystem::check()
{
	GLenum err = glGetError();
	std::cout<<gluErrorString(err)<<std::endl;
}

void SPHSystem::init()
{
	initBuffers();
	initializePositions();
	initSPHParams();
}

void SPHSystem::initSPHParams()
{
	mDeltaT = 0.02;
	mParticleMass = 1.98 / NUM_PARTICLES;		//Fluid mass = 1.98kg 

}

void SPHSystem::setBoundary(glm::vec3 min, glm::vec3 max)
{
	mMin = min;
	mMax = max;
}

void SPHSystem::initializePositions()
{
	float spacing = (SMOOTH_LENGTH/SIM_SCALE);


	int count = 0;
	glm::vec3 center = (mMin+mMax)/2.f;
	for(float i = center.y ; i < mMax.y ; i++)
	{
		for(float j = center.x; j < mMax.x ; j++)
		{
			if(count < NUM_PARTICLES){
				glm::vec3 p		= glm::vec3(i/spacing, j/spacing, 0);
				glm::vec3 v		= glm::vec3(rand1(-5,5), rand1(-1,1), 0);
				glm::vec3 a		= glm::vec3(0,0,0);
				glm::vec3 vprev = v - 0.5f * float(mDeltaT) * a; 
				glm::vec3 force = glm::vec3(0);
				mP.push_back(p);
				mV.push_back(v);
				mA.push_back(a);
				mVprev.push_back(vprev);
				mDensity.push_back(0);
				mPressure.push_back(0);
				mForce.push_back(force);
			}
			count++;
		}
	}

	sg = (HashTable*)malloc(sizeof(HashTable));
	sg->size = 2 * NUM_PARTICLES;
	sg->first = (Particle**)malloc(NUM_PARTICLES*sizeof(Particle));
	for(int i = 0; i < sg->size; i++)
	{
		sg->first[i] = NULL;
	}
}

void SPHSystem::initBuffers()
{
	glGenBuffers(1, &m_vbo);
}

void SPHSystem::drawBoundary(Camera &cam)
{
	float dx = mMax.x - mMin.x;
	float dy = mMax.y - mMin.y;

	enableFixedFunction(cam);
	glBegin(GL_LINE_STRIP);
		glVertex3f(mMin.x, mMin.y, mMin.z);
		glVertex3f(mMin.x+dx, mMin.y, mMin.z);
		glVertex3f(mMax.x, mMax.y, mMax.z);
		glVertex3f(mMax.x-dx, mMax.y, mMin.z);
		glVertex3f(mMin.x, mMin.y, mMin.z);
	glEnd();
	disableFixedFunction();
}

void SPHSystem::render(int pos_loc, Camera &cam)
{
	
	if(true)
	{
		drawBoundary(cam);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);	
	glBufferData(GL_ARRAY_BUFFER, mP.size()*sizeof(glm::vec3), &mP[0], GL_STREAM_DRAW);
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDrawArrays(GL_POINTS, 0, mP.size());
}

int SPHSystem::computeHash(glm::vec3 p)
{
	float wCellSize = SMOOTH_LENGTH / SIM_SCALE;
	int x = p.x / wCellSize;
	int y = p.y / wCellSize;
	int z = p.z / wCellSize;

	//something wrong with this hashing function
	//doesn't work!! 
	//return (73856093 * x + 19349663 * y + 83492791 * z) % HASHTABLE_SIZE; 

	//this works fine
	return ((y*wCellSize)+x);
}

void SPHSystem::addToGrid(int hash, int index)
{
	if(sg->first[hash] != NULL)
	{
		Particle* list = sg->first[hash];
		while(list->next != NULL)
		{
			list = list->next;
		}
		Particle* newParticle = (Particle*)malloc(sizeof(Particle));
		newParticle->n = index;
		newParticle->next = NULL;
		list->next = newParticle;
	}
	else{
		sg->first[hash] = (Particle*)malloc(sizeof(Particle));
		sg->first[hash]->n = index; 
		sg->first[hash]->next = NULL;
	}
}

void SPHSystem::updateGrid(){

	//clear existing grid
	for(int i = 0 ; i <sg->size;i++)
	{
		free(sg->first[i]);
	}

	//allocate new memory
	sg->first = (Particle**)malloc(NUM_PARTICLES*sizeof(Particle));
	for(int i = 0; i < sg->size; i++)
	{
		sg->first[i] = NULL;
	}
	
	//loop through each particle 
	for(int i = 0 ; i < NUM_PARTICLES ; i++)
	{
		glm::vec3 p = mP[i];
		int hash = computeHash(p);
		addToGrid(hash, i);

	}
}

void SPHSystem::neighbours(glm::vec3 p, int neighbourList[], int &count)
{
	float rad = 5.f;
	glm::vec3 min = glm::vec3(p.x-rad,p.y-rad, 0);
	glm::vec3 max = glm::vec3(p.x+rad,p.y+rad, 0);
	count = 0;

	for(float x = min.x; x <= max.x; x+=rad)
	{
		for(float y = min.y; y <= max.y; y+=rad)
		{
			glm::vec3 pp = glm::vec3(x,y,0);
			int hash = computeHash(pp);
			if(true)
			{
				Particle* list = sg->first[hash];
				while(list != NULL)
				{	
					neighbourList[count] = list->n;
					list = list->next;
					count++;
				}
			}
		}
	}
}

void SPHSystem::clearAcceleration()
{
	for(int i=0 ; i < NUM_PARTICLES; i++)
	{
		mA[i] = glm::vec3(0.f);
	}

}


void SPHSystem::computeDensity()
{
	/*
	Poly Kernel:
		(315 / 64 * pi * h^9) * (h^2 - r^2) ^ 3
	*/
	double kernel = 1.56747 /  pow(SMOOTH_LENGTH, 9);
	double h2 = SMOOTH_LENGTH * SMOOTH_LENGTH;

	//for each particle
	//find its neighbour

	for(int i = 0; i < NUM_PARTICLES ; ++i)
	{
		int list[NUM_PARTICLES];
		int count;
		neighbours(mP[i], list, count);	

		glm::dvec3 pi = (glm::dvec3)mP[i];
		//scale down to fluid world
		pi *= SIM_SCALE;
		double sum = 0;
		for(int j = 0 ; j < count ; ++j)
		{
			
			glm::dvec3 pj = (glm::dvec3)mP[list[j]];
			//scale down to fluid world
			pj *= SIM_SCALE;
			double r = glm::length(pj - pi);
			double r2 = r*r;
			if(h2 > r2 && EPSILON <= r2){
				double cube = (h2 - r2);
				sum += cube * cube * cube;
			}
		}
		mDensity[i] = sum * kernel * mParticleMass;
	}

	
}

void SPHSystem::computePressureForce()
{
	/*
	spiky kernel:
		15/(pi*h6) * (h-r)^3 
	*/
	double spikyKernel = 4.7746 * pow(SMOOTH_LENGTH, 6);
	double h = SMOOTH_LENGTH;
	double h2 = SMOOTH_LENGTH * SMOOTH_LENGTH;

	for(int i = 0; i < NUM_PARTICLES ; ++i)
	{
		int list[NUM_PARTICLES];
		int count;
		neighbours(mP[i], list, count);

		glm::dvec3 pi = (glm::dvec3)mP[i];
		double pres_i = mPressure[i];
		
		//scale down to fluid world
		pi *= SIM_SCALE;
		glm::dvec3 sum = glm::dvec3(0);
		for(int j = 0 ; j < count ; ++j)
		{

			glm::dvec3 pj = (glm::dvec3)mP[list[j]];
			double pres_j = mPressure[list[j]];
			double dens_j = mDensity[list[j]];
			//scale down to fluid world
			pj *= SIM_SCALE;
			glm::dvec3 diff = pi - pj;
			double r = glm::length(pj - pi);
			double r2 = r*r;
			if(h2 > r2 && EPSILON <= r2){
				double cube = (h - r);
				sum += diff/r *((pres_i+pres_j)/(2*dens_j)) * cube * cube * cube;
			}
		}
		mForce[i] += (sum * (mParticleMass * spikyKernel * -1.0));
	}
}


void SPHSystem::update(){
	updateGrid();
	clearAcceleration();
	computeDensity();
	computePressureForce();
	checkBoundary();
	step();
}

void SPHSystem::checkBoundary()
{
	float wallStiff = 5000.0; 
	float wallDamp = 0.9; 
	float radius = 0.004;
	float diff;
	double adj;
	glm::vec3 norm;
	float padding = 5.f;

	for(int i = 0 ; i < mP.size() ; i++)
	{
		//X Walls left
		norm = glm::vec3(1, 0, 0);
		diff = 2*radius - (mP[i].x - (mMin.x+padding));
		if(diff > EPSILON)
		{
			adj = wallStiff * diff - wallDamp* glm::dot(norm, mVprev[i]);
			mA[i] += float(adj) * norm;
		}

		//X Walls right
		norm = glm::vec3(-1, 0, 0);
		diff = 2*radius - ((mMax.x-padding) - mP[i].x);
		if(diff > EPSILON)
		{
			adj = wallStiff * diff - wallDamp* glm::dot(norm, mVprev[i]);
			mA[i] += float(adj) * norm;
		}

		//Y Walls Down
		norm = glm::vec3(0, 1, 0);
		diff = 2*radius - (mP[i].y - (mMin.y+padding));
		if(diff > EPSILON)
		{
			adj = wallStiff * diff - wallDamp* glm::dot(norm, mVprev[i]);
			mA[i] += float(adj) * norm;
		}

		//Y Walls Up
		norm = glm::vec3(0, -1, 0);
		diff = 2*radius - ((mMax.y-padding) - mP[i].y);
		if(diff > EPSILON)
		{
			adj = wallStiff * diff - wallDamp* glm::dot(norm, mVprev[i]);
			mA[i] += float(adj) * norm;
		}
	}
}

void SPHSystem::step()
{
	float dt = mDeltaT;
	for(int i = 0 ; i < mP.size() ; i++)
	{
		
		mA[i] += glm::vec3(0, -9.8, 0);
		glm::vec3 vhalf =	mVprev[i] + dt * mA[i];
		mP[i] += vhalf * dt;
		mV[i] += ( vhalf + mVprev[i] ) * 0.5f;
		mVprev[i] = vhalf;
	}
}