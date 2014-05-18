#ifndef _TrackingResults_H
#define _TrackingResults_H

#ifdef _DEBUG
//#define GLM_MESSAGES
#endif
#include <glm/fwd.hpp>
#include <glm/glm.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

enum PoseAngles { Pitch = 0, Yaw, Roll };
enum PoseTranlation { Xaxis = 0, Yaxis, Zaxis };

struct AvatarPose
{
	float eulerAngles[3];
	float translation[3]; 
	float scale;
};

struct CameraPose
{
	float position[3];
	float target[3];
	float upVector[3];
};

struct AvatarPoseGlm
{
	AvatarPoseGlm() :	
		eulerAngles(glm::vec3(0.0f)),
		translation(glm::vec3(0.0f)),
		scale(glm::mat4(1.0f))
	{
	}

	void SetAvatarPose(AvatarPose& pose)
	{
		eulerAngles = glm::vec3(pose.eulerAngles[Pitch], pose.eulerAngles[Yaw], pose.eulerAngles[Roll]);
		translation = glm::vec3(pose.translation[Xaxis], pose.translation[Yaxis], pose.translation[Zaxis]);
		scale = glm::mat4(scale);
	}

	glm::vec3 eulerAngles;
	glm::vec3 translation; 
	glm::mat4 scale;
};

struct CameraPoseGlm
{
	CameraPoseGlm() :	
		position(glm::vec3(0.0f)),
		target(glm::vec3(0.0f)),
		upVector(glm::vec3(0.0f, 1.0f, 0.0f))
	{
	}


	void SetCameraPose(CameraPose& pose)
	{
		position = glm::vec3(pose.position[Xaxis], pose.position[Yaxis], pose.position[Zaxis]);
		target = glm::vec3(pose.target[Xaxis], pose.target[Yaxis], pose.target[Zaxis]);
		upVector =  glm::vec3(pose.upVector[Xaxis], pose.upVector[Yaxis], pose.upVector[Zaxis]);
	}
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 upVector;
};

class TrackingResults
{	
public:
	TrackingResults()
	{
		UpdateTransforms();
	}

	virtual void UpdateModelTransform();
	virtual void UpdateViewTransform();		
	virtual void UpdateProjectionTransform();
	void UpdateTransforms();	
	
	AvatarPose GetAvatarPose();
	CameraPose GetCameraPose();

	AvatarPoseGlm avatar;
	CameraPoseGlm camera;

	glm::mat4 Transform;
	glm::mat4 ModelView;
	glm::mat4 Projection; 
	glm::mat4 Model;
	glm::mat4 View;

protected:


};

#endif