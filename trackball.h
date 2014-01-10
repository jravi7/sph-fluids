#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace std;

class TrackBall
{

private:
	int mWidth;
	int mHeight;
	glm::vec3 v1;		//init vector
	glm::vec3 v2;		//final vector
	glm::vec3 twoDto3D(int x, int y);
	float rotateStep;
	bool isDown;
	bool isMove;
	glm::mat4 currRot; 
	glm::mat4 lastRot;
	glm::mat4 transformation;
	glm::mat4 computeCurrRot();


public:
	TrackBall(int width, int height);
	void trackPoint(int x, int y);
	void trackMovement(int x, int y);
	void matrix();
	void setBtnState(bool state);
	bool getIsDown();
	glm::mat4 transform();
	~TrackBall(void);
};

