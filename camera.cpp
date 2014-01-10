#include "camera.h"
#include <cmath>

#define M_PI 3.14159265358979323846


Camera::Camera(void):
	mpos(0.0f, 0.0f, 1.0f),
	mhAngle(0.0f), 
	mvAngle(0.0f), 
	mFov(60.f), 
	mNear(0.01f), 
	mFar(100.f), 
	mAsp(4.0/3.0f)
{
	
}

	Camera::~Camera(void)
	{
	}

	static const float MaxVerticalAngle = 85.0f; //must be less than 90 to avoid gimbal lock

	static inline float RadiansToDegrees(float radians) {
		return radians * 180.0f / (float)M_PI;
	}

	const glm::vec3& Camera::position()
	{
		return mpos; 
	}

	void Camera::setPosition(const glm::vec3& position)
	{
		mpos = position;
	}

	void Camera::offsetPosition(const glm::vec3& offset)
	{
		mpos += offset; 
	}

	float Camera::fov()
	{
		return mFov; 
	}

	void Camera::setFov(float fov)
	{
		assert(fov > 0.0f && fov < 180.f, "FOV cannot be less 0 or greater than 180");
		mFov = fov; 
	}

	float Camera::nearPlane()
	{
		return mNear; 
	}

	float Camera::farPlane()
	{
		return mFar; 
	}

	void Camera::setNearFar(float near, float far)
	{
		assert(near>0.0f); 
		assert(far> near);
		mNear = near; 
		mFar = far; 
	}

	glm::mat4 Camera::orientation(){
		glm::mat4 orientation; 
		orientation = glm::rotate(orientation, mvAngle, glm::vec3(1, 0, 0));
		orientation = glm::rotate(orientation, mhAngle, glm::vec3(0,1,0));
		return orientation; 
	}

	void Camera::offsetOrientation(float upAngle, float rightAngle)
	{
		mhAngle += rightAngle; 
		mvAngle += upAngle; 
		normalizeAngles();
	}

	void Camera::lookAt(glm::vec3 position)
	{
		assert(position != mpos); 
		glm::vec3 direction = glm::normalize(position - mpos);
		mvAngle = RadiansToDegrees(asinf(-direction.y));
		mhAngle = -RadiansToDegrees(atan2f(-direction.x, -direction.z));
		normalizeAngles();
	}

	float Camera::viewportAspRat(){
		return mAsp; 
	}

	void Camera::setAspRatio(float viewAspectRatio){
		assert(viewAspectRatio > 0.0); 
		mAsp = viewAspectRatio;
	}

	glm::vec3 Camera::forward()
	{
		glm::vec4 forward = glm::inverse(orientation()) * glm::vec4(0,0,-1,1);
		return glm::vec3(forward);
	}

	glm::vec3 Camera::right() 
	{
		glm::vec4 right = glm::inverse(orientation())*glm::vec4(1,0,0,1); 
		return glm::vec3(right);
	}

	glm::vec3 Camera::up()
	{
		glm::vec4 up = glm::inverse(orientation()) * glm::vec4(0,1,0,1);
		return glm::vec3(up);
	}

	glm::mat4 Camera::matrix(){
		return projection() * view();
	}

	glm::mat4 Camera::projection(){
		return glm::perspective(mFov,mAsp, mNear, mFar);
	}

	glm::mat4 Camera::view()
	{
		return orientation() * glm::translate(glm::mat4(), -mpos);
	}

	void Camera::normalizeAngles() {
		mhAngle = fmodf(mhAngle, 360.0f);
		//fmodf can return negative values, but this will make them all positive
		if(mhAngle < 0.0f)
	        mhAngle += 360.0f;
	
	    if(mvAngle > MaxVerticalAngle)
			mvAngle = MaxVerticalAngle;
		else if(mvAngle < -MaxVerticalAngle)
	        mvAngle = -MaxVerticalAngle;
	}

	void Camera::setVelocity(float speed)
	{
		mVel = speed;
	}

	float Camera::velocity()
	{
		return mVel; 
	}

	void Camera::init()
	{
		setMouseCoords(mWindowWidth/2, mWindowHeight/2);
	}

	void Camera::setWindowCoords(int w, int h)
	{
		mWindowWidth = w;
		mWindowHeight= h;
	}

	void Camera::setMouseCoords(int x, int y)
	{
		mMouseX = x; 
		mMouseY = y; 
	}

	void Camera::onMouseMove(int x, int y)
	{
			float deltaX = (x - mMouseX);
			float deltaY = (y - mMouseY);
			mMouseX = x; 
			mMouseY = y;	
			offsetOrientation(deltaY, deltaX);
	}