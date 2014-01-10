#pragma once
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
	glm::vec3 mpos;
	float mhAngle; 
	float mvAngle; 
	float mFov; 
	float mNear; 
	float mFar;
	float mAsp; 
	float mVel;
	void normalizeAngles();
	int mWindowWidth;
	int mWindowHeight;
	

public:
	int mMouseX;
	int mMouseY;

	Camera(void);
	~Camera(void);
	void init();

	const glm::vec3& position(); 
	void setPosition(const glm::vec3& position); 
	void offsetPosition(const glm::vec3& offset); 
	float fov();
	void setFov(float fov); 

	float nearPlane(); 
	float farPlane(); 

	glm::mat4 view(); 
	glm::mat4 projection();
	glm::mat4 matrix();
	glm::vec3 up();
	glm::vec3 right();
	glm::vec3 forward();
	void setAspRatio(float aspRat);
	float viewportAspRat();
	void lookAt(glm::vec3 position);
	void offsetOrientation(float upAngle, float rightAngle);
	glm::mat4 orientation(); 
	void setNearFar(float near, float far);
	void setVelocity(float speed);
	float velocity();

	void setWindowCoords(int w, int h);
	void setMouseCoords(int x, int y);
	void Camera::onMouseMove(int x, int y);
	

};

