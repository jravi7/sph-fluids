#include "SPHSystem.h"


SPHSystem::SPHSystem(int count){
	mN = count;
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
}

void SPHSystem::setBoundary(glm::vec3 min, glm::vec3 max)
{
	mMin = min;
	mMax = max;
}

void SPHSystem::initializePositions()
{
	float cellSize = 0.5f;
	int count = 0;
	glm::vec3 center = (mMin+mMax)/2.f;
	for(float i = center.y ; i < mMax.y ; i++)
	{
		for(float j = center.x ; j < mMax.x ; j++)
		{
			if(count < mN){
				glm::vec3 p = glm::vec3(j*cellSize, i*cellSize, 0);
				glm::vec3 v = glm::vec3(rand1(-10,10), rand1(-10,10), 0);
				mP.push_back(p);
				mV.push_back(v);
			}
			count++;
		}
	}

	sg = (HashTable*)malloc(sizeof(HashTable));
	sg->size = 200;
	sg->first = (Particle**)malloc(200*sizeof(Particle));
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
	int gridDeltaX = mMax.x / 10;
	int gridDeltaY = mMax.y / 10;
	int x = p.x / 10;
	int y = p.y / 10;
	return ((y*10)+x);
	//std::cout<<"x: "<<x<<std::endl;
	//std::cout<<"y: "<<y<<std::endl;
	//std::cout<<"Hash: "<<(y*10)+x<<std::endl;
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
	sg->first = (Particle**)malloc(5000*sizeof(Particle));
	for(int i = 0; i < sg->size; i++)
	{
		sg->first[i] = NULL;
	}
	
	//loop through each particle 
	for(int i = 0 ; i < mN ; i++)
	{
		glm::vec3 p = mP[i];
		int hash = computeHash(p);
		addToGrid(hash, i);

	}
}

void SPHSystem::addToStack(Particle* &head, int index)
{
	if(head != NULL)
	{
		//declare new element
		Particle* newParticle = (Particle*)malloc(sizeof(Particle));
		newParticle->n = index;
		//set 
		newParticle->next = head;
		head=newParticle;
	}
	else{
		head = (Particle*)malloc(sizeof(Particle));
		head->n = index; 
		head->next = NULL;
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
					//addToStack(neighbourList, list->n);
					neighbourList[count] = list->n;
					list = list->next;
					count++;
				}
			}
		}
	}
}

void SPHSystem::update(){
	updateGrid();
	checkBoundary();
	step();
}

void SPHSystem::checkBoundary()
{
	for(int i = 0 ; i < mP.size() ; i++)
	{
		if(mP[i].x < mMin.x || mP[i].x > mMax.x)
		{
			mV[i].x *= -1;
		}
		if(mP[i].y < mMin.y || mP[i].y > mMax.y)
		{
			mV[i].y *= -1;
		}
	}
}

void SPHSystem::step()
{
	float dt = 0.004;
	for(int i = 0 ; i < mP.size() ; i++)
	{
		//std::cout<<"Neighbour of Particle:"<<i<<std::endl;
		int list[5000];
		int count;
		neighbours(mP[i], list, count);
		mP[i] += mV[i] * dt;
	}
}