#include "TrackingResults.h"

CameraCoordSystem TrackingResults::GetCameraCoordSystem()
{
	CameraCoordSystem coordSys;
	return coordSys;
}

void TrackingResults::UpdateModelTransform()
{
	glm::mat4 rotation = glm::toMat4(avatar.eulerAngles);
	//glm::mat4 rotation = glm::mat4(1.0f);
	//rotation = glm::rotate(rotation, avatar.eulerAngles.x, glm::vec3(1,0,0));
	//rotation = glm::rotate(rotation, avatar.eulerAngles.y, glm::vec3(0,1,0));
	//rotation = glm::rotate(rotation, avatar.eulerAngles.z, glm::vec3(0,0,1));
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), camera.position + avatar.translation);
	Model = translation * rotation * avatar.scale;
}

void TrackingResults::UpdateViewTransform()
{ 				
	View = glm::lookAt(camera.position, camera.target, camera.viewUp);
}

void TrackingResults::UpdateProjectionTransform()
{
	Projection = glm::perspective(45.0f, 4.0f/3.0f, 0.01f, 10.0f);
}

void TrackingResults::UpdateModelViewTransform()
{
	ModelView = View * Model;
}

void TrackingResults::UpdateCameraViewTransform()
{
	CameraView = glm::translate(ModelView, avatar.translation);
}
	
Pose TrackingResults::GetAvatarPose()
{
	Pose result = avatar;	
	
	return result;
}

Pose TrackingResults::GetCameraPose()
{
	Pose result = camera;	
	
	return result;
}


void TrackingResults::SetAvatarPose(const Pose& pose)
{
	avatar = pose;
}

void TrackingResults::SetCameraPose(const Pose& pose)
{
	camera = pose;
}

