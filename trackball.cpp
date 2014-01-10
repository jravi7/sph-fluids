#include "trackball.h"


TrackBall::TrackBall(int width, int height)
{
	mWidth = width; 
	mHeight = height;
	rotateStep = 5;
	isDown = false;
	isMove = false;
	lastRot = glm::mat4(1);
	currRot = glm::mat4(1);
}

void TrackBall::trackPoint(int x, int y)
{
	if(isDown){
		v1 = twoDto3D(x, y);
		lastRot = currRot;

	cout<<"Init Point"<<endl;
	cout<<v1.x<<endl;
	cout<<v1.y<<endl;
	cout<<v1.z<<endl;
	}
}

bool TrackBall::getIsDown()
{
	return isDown;
}

void TrackBall::trackMovement(int x, int y)
{
	if(isDown){
		v2 = twoDto3D(x, y);

	cout<<"Final Point"<<endl;
	cout<<v2.x<<endl;
	cout<<v2.y<<endl;
	cout<<v2.z<<endl;
	matrix();
	}
}

void TrackBall::setBtnState(bool state)
{
	isDown = state;
}

glm::mat4 TrackBall::computeCurrRot()
{
	glm::mat4 m = glm::mat4(1);
	if(isDown){
		glm::vec3 axis = glm::cross(v2, v1);
		glm::vec3 direction = v2 - v1; 
		float velocity = direction.length();
		float angle = velocity * rotateStep;
		m = glm::rotate(m, angle, axis);
	}
	return m; 
}

/*
compute the rotation matrix about the axis running
between v1 and v2
*/
void TrackBall::matrix()
{
	if(isDown)
	{
		currRot = computeCurrRot();
	}
	transformation = currRot * lastRot;
}

glm::mat4 TrackBall::transform()
{
	return transformation;
}

TrackBall::~TrackBall(void)
{

}

glm::vec3 TrackBall::twoDto3D(int x, int y)
{
	glm::vec3 v = glm::vec3();
	v.x = (2*(float)x - mWidth) / mWidth;
	v.y = (mHeight- 2*(float)y)/mHeight;
	float d = sqrt((v.x*v.x)+(v.y*v.y));
	d = (d<1.0f)?d:1.0f;
	v.z = sqrt(1-d*d);
	v = glm::normalize(v);
	return v;
}