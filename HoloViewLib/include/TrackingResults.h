#ifndef _TrackingResults_H
#define _TrackingResults_H

#ifdef _DEBUG
//#define GLM_MESSAGES
#endif
#define GLM_FORCE_RADIANS
#include <glm/fwd.hpp>
#include <glm/glm.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>

enum PoseAngles { Pitch = 0, Yaw, Roll };
enum PoseTranlation { Xaxis = 0, Yaxis, Zaxis };

struct Pose
{
	Pose() :	
		eulerAngles(glm::vec3(0.0f)),
		translation(glm::vec3(0.0f)),
		scale(1.0f),
		position(glm::vec3(0.0f)),
		target(glm::vec3(0.0f)),
		viewUp(glm::vec3(0.0f, 1.0f, 0.0f))

	{	};

	Pose(Pose &parent) :	
		eulerAngles(parent.eulerAngles),
		translation(parent.translation),
		scale(parent.scale),
		position(parent.position),
		target(parent.target),
		viewUp(parent.viewUp)
	{	};

	float scale;
	glm::quat rotation;
	glm::vec3 eulerAngles;
	glm::vec3 translation; 
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 viewUp;
};

class TrackingResults
{
public:
	TrackingResults() : 
		  CameraView(glm::mat4(1.0f)),
		  ModelView(glm::mat4(1.0f)),
		  Projection(glm::mat4(1.0f)),
		  View(glm::mat4(1.0f)),
		  Model(glm::mat4(1.0f))
	{
	}
	virtual void SetPose(const Pose& pose);
    virtual void SetAvatarPose(const Pose& pose);
	virtual void SetCameraPose(const Pose& pose);
	virtual Pose GetPose();
	virtual Pose GetAvatarPose();
	virtual Pose GetCameraPose();

	glm::mat4 GetModel() 
	{ 
		UpdateModelTransform(); 
		return Model; 
	}

	glm::mat4 GetView() 
	{ 
		UpdateViewTransform(); 
		return View; 
	}

	glm::mat4 GetProjection() 
	{ 
		UpdateProjectionTransform(); 
		return Projection; 
	}

	glm::mat4 GetModelView()
	{
		UpdateModelTransform();
		UpdateViewTransform();
		UpdateModelViewTransform();
		return ModelView;
	}

	glm::mat4 GetCameraView() 
	{ 
		UpdateModelTransform();
		UpdateViewTransform();
		UpdateModelViewTransform();
		UpdateCameraViewTransform();
		return CameraView; 
	}

	int trackerId;
protected:

	virtual void UpdateModelTransform();
	virtual void UpdateViewTransform();		
	virtual void UpdateProjectionTransform();
	virtual void UpdateModelViewTransform();	
	virtual void UpdateCameraViewTransform();
	glm::mat4 CameraView;
	glm::mat4 ModelView;
	glm::mat4 Projection; 
	glm::mat4 Model;
	glm::mat4 View;

	Pose avatar;
	Pose camera;
};



#endif