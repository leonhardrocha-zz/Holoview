#include "TrackingResults.h"

CameraCoordSystem TrackingResults::GetCameraCoordSystem()
{
	CameraCoordSystem coordSys;
	return coordSys;
}

void TrackingResults::UpdateModelTransform()
{
	glm::mat4 modelTranslation = glm::translate(glm::mat4(1.0f), avatar.translation +  camera.position); 

	//glm::quat quaternion = glm::quat(1.0f,
	//								 glm::radians(avatar.eulerAngles[Pitch]),
	//								 glm::radians(avatar.eulerAngles[Yaw]),
	//								 glm::radians(avatar.eulerAngles[Roll]));
	//
	//glm::mat4 modelRotation = glm::toMat4(quaternion);
	glm::mat4 modelRotation = glm::mat4(1.0f);
	modelRotation = glm::rotate(modelRotation, avatar.eulerAngles[Pitch], glm::vec3(1.0f, 0.0f, 0.0f));
	modelRotation = glm::rotate(modelRotation, avatar.eulerAngles[Yaw],   glm::vec3(0.0f, 1.0f, 0.0f));
	modelRotation = glm::rotate(modelRotation, avatar.eulerAngles[Roll],  glm::vec3(0.0f, 0.0f, 1.0f));
	
	Model =  modelTranslation * modelRotation * avatar.scale;
}

void TrackingResults::UpdateViewTransform()
{ 				
	View = glm::lookAt(camera.position, camera.target, camera.upVector);
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
	CameraView = Projection * ModelView;
}
	
AvatarPose TrackingResults::GetAvatarPose()
{
	AvatarPose result = avatar;	
	
	return result;
}

CameraPose TrackingResults::GetCameraPose()
{
	CameraPose result = camera;
		
	return result;
}

void TrackingResults::SetAvatarPose(const AvatarPose& pose)
{
	avatar = pose;
}

void TrackingResults::SetCameraPose(const CameraPose& pose)
{
	camera = pose;
}
