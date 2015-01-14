#include "SPHSystem.h"

#define EPSILON 0.000001

SPHSystem::SPHSystem(){}
SPHSystem::~SPHSystem(void){}

void SPHSystem::init()
{
	mMin = glm::vec3(0);
	mMax = glm::vec3(WORLD_SIZE, WORLD_SIZE, 0);
	initBuffers();
	initializePositions();
	initGrid();
	initKernels();
}

void SPHSystem::initBuffers()
{
	glGenBuffers(1, &m_vbo);
}

void SPHSystem::initKernels()
{
	mPolyKernel = 315/(64*M_PI*pow(SMOOTH_LENGTH,9.0));
	mSpikyKernel = -45/(M_PI*pow(SMOOTH_LENGTH,6));
	mViscousKernel = 45/(M_PI*pow(SMOOTH_LENGTH, 6));
}

void SPHSystem::initGrid()
{
	sg = (HashTable*)malloc(sizeof(HashTable));
	sg->size = 2 * GRID_SIZE * GRID_SIZE;
	sg->first = (Particle**)malloc(sg->size*sizeof(Particle));
	for(int i = 0; i < sg->size; i++)
	{
		sg->first[i] = NULL;
	}
}

void SPHSystem::initializePositions()
{
	TOTAL_PARTICLES = 0;
	float spacing = SMOOTH_LENGTH*0.6;
	for(float i = spacing; i < mMax.y ; i+=spacing)
	{
		for(float j = spacing; j < mMax.x ; j+=spacing)
		{
			if(TOTAL_PARTICLES < MAX_PARTICLES){
				glm::vec3 p		= glm::vec3(i, j, 0);
				glm::vec3 v		= glm::vec3(0, 0, 0);
				glm::vec3 a		= glm::vec3(0,0,0);
				glm::vec3 vprev = v - 0.5f * float(DELTA_T) * a; 
				glm::vec3 force = glm::vec3(0);
				mP.push_back(p);
				mV.push_back(v);
				mA.push_back(a);
				mVprev.push_back(vprev);
				mDensity.push_back(REST_DENSITY);
				mPressure.push_back(0);
				mForce.push_back(force);
				TOTAL_PARTICLES++;
			}

		}
	}

	
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
	int x = floor(p.x / CELL_SIZE);
	int y = floor(p.y / CELL_SIZE);
	int z = floor(p.z / CELL_SIZE);

	
	//This limits the coordinates withing the grid size along x or y
	/*
		for e.g. if x = 45
		x = x & 31
		x = 13 
		how?
		(45 - 31) - 1
		(exceeded number - limit) - 1
	*/
	x = x & int(GRID_SIZE - 1);
	y = y & int(GRID_SIZE - 1);
	
	return (int((y*GRID_SIZE)+x)) ;
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
	sg->first = (Particle**)malloc(sg->size*sizeof(Particle));
	for(int i = 0; i < sg->size; i++)
	{
		sg->first[i] = NULL;
	}
	
	//loop through each particle 
	for(int i = 0 ; i < TOTAL_PARTICLES ; i++)
	{
		glm::vec3 p = mP[i];
		int hash = computeHash(p);
		addToGrid(hash, i);

	}
}

void SPHSystem::neighbours(glm::vec3 p, int neighbourList[], int &count)
{
	float rad = CELL_SIZE;
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
	for(int i=0 ; i < TOTAL_PARTICLES; i++)
	{
		mA[i] = glm::vec3(0.f);
		mForce[i] = glm::vec3(0.f);
	}

}


void SPHSystem::computeDensity()
{
	/*
	Poly Kernel:
		(315 / 64 * pi * h^9) * (h^2 - r^2) ^ 3
	*/
	double h2 = SMOOTH_LENGTH * SMOOTH_LENGTH;

	//for each particle
	//find its neighbour

	for(int i = 0; i < TOTAL_PARTICLES ; ++i)
	{
		int list[MAX_PARTICLES];
		int count;
		neighbours(mP[i], list, count);	

		glm::vec3 pi = mP[i];
		//scale down to fluid world
		//pi *= SIM_SCALE;
		double sum = 0;
		mDensity[i]=0;
		mPressure[i]=0;
		for(int j = 0 ; j < count ; ++j)
		{
			
			glm::vec3 pj = mP[list[j]];
			//scale down to fluid world
			//pj *= SIM_SCALE;
			double r = glm::length(pj - pi);
			double r2 = r*r;
			if(h2 > r2 && EPSILON <= r2){
				double cube = (h2 - r2);
				sum += cube * cube * cube;
			}
		}
		mDensity[i] += sum * mPolyKernel * PARTICLE_MASS;
		mPressure[i] += GAS_CONSTANT * (pow(mDensity[i]/REST_DENSITY,7)-1);
		//mPressure[i] += GAS_CONSTANT * (mDensity[i]-REST_DENSITY);
	}

	
}

void SPHSystem::computePressureForce()
{
	double h = SMOOTH_LENGTH;
	double h2 = SMOOTH_LENGTH * SMOOTH_LENGTH;

	for(int i = 0; i < TOTAL_PARTICLES ; ++i)
	{
		int list[MAX_PARTICLES];
		int count;
		neighbours(mP[i], list, count);

		glm::vec3 pi = mP[i];
		glm::vec3 vi = mV[i];
		float pres_i = mPressure[i];
		
		//scale down to fluid world
		//pi *= SIM_SCALE;
		glm::vec3 sumPressure = glm::vec3(0);
		glm::vec3 sumViscous= glm::vec3(0);
		for(int j = 0 ; j < count ; ++j)
		{

			glm::vec3 pj = (glm::vec3)mP[list[j]];
			glm::vec3 vj = (glm::vec3)mV[list[j]];
			float pres_j = mPressure[list[j]];
			float dens_j = mDensity[list[j]];
			//scale down to fluid world
			//pj *= SIM_SCALE;
			glm::vec3 diff = pi - pj;
			float r = glm::length(pj - pi);
			float r2 = r*r;
			if(h2 > r2 && EPSILON <= r2){
				float square = (h - r);
				sumPressure += diff/r *((pres_i+pres_j)/(2*dens_j)) * square * square;
				sumViscous += (vj - vi)/dens_j * (square);
			}
		}
		mForce[i] += (sumPressure * (float(PARTICLE_MASS) * mSpikyKernel * -1.f));
		mForce[i] += (sumViscous * (float(VISCOSITY * PARTICLE_MASS) * mViscousKernel));
	}
}


void SPHSystem::neighbourSearch()
{
	for(int i = 0; i < TOTAL_PARTICLES ; ++i)
	{
		int list[MAX_PARTICLES];
		int count;
		neighbours(mP[i], list, count);
		/*std::cout<<"Neighbour of "<<i<<":";
		for(int j = 0 ; j < count ; ++j)
		{
			std::cout<<j<<" ";
		}
		std::cout<<std::endl;*/
	}
}

void SPHSystem::update(){
	updateGrid();
	/* This function is only meant to check performance
	Do not use it for actual simulation*/
	//neighbourSearch(); 
	
	clearAcceleration();
	computeDensity();
	computePressureForce();
	checkBoundary();
	step();
}

void SPHSystem::checkBoundary()
{
	float wallStiff = 500.0; 
	float wallDamp = 0.5; 
	float radius = SMOOTH_LENGTH*0.6;
	float diff;
	double adj;
	glm::vec3 norm;
	float padding = SMOOTH_LENGTH*0.7;

	for(int i = 0 ; i < TOTAL_PARTICLES ; i++)
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
	float dt = DELTA_T;
	for(int i = 0 ; i < TOTAL_PARTICLES ; i++)
	{
		mA[i] += mForce[i] /mDensity[i];
		mA[i] += glm::vec3(0, GRAVITY, 0);
		mV[i] += mA[i] * dt;
		//glm::vec3 vhalf =	mVprev[i] + dt * mA[i];
		mP[i] += mV[i] * dt;
		//mV[i] += ( vhalf + mVprev[i] ) * 0.5f;
		//mVprev[i] = vhalf;
	}
}