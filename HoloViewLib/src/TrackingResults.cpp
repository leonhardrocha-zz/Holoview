#include "TrackingResults.h"

void TrackingResults::UpdateModelTransform()
{
	glm::mat4 modelTranslation = glm::translate(glm::mat4(1.0f), avatar.translation); 
	glm::quat quaternion = glm::quat(avatar.eulerAngles);
	glm::mat4 modelRotation = glm::toMat4(quaternion);
		
	Model =  modelTranslation * modelRotation * avatar.scale;
}
void TrackingResults::UpdateViewTransform()
{ 				
	View = glm::lookAt(camera.position, camera.target, camera.upVector);
}

void TrackingResults::UpdateProjectionTransform()
{ 				
	Projection = glm::perspective(45.0f, 4.0f/3.0f, 0.01f, 100.0f);
}

void TrackingResults::UpdateTransforms()
{
	UpdateModelTransform();
	UpdateViewTransform();
	UpdateProjectionTransform();

	ModelView = View * Model;
	Transform = Projection * ModelView;
}
	
AvatarPose TrackingResults::GetAvatarPose()
{
	AvatarPose result;
	 
	result.eulerAngles[Pitch] = avatar.eulerAngles.x;
	result.eulerAngles[Yaw]   = avatar.eulerAngles.y;
	result.eulerAngles[Roll]  = avatar.eulerAngles.z;
	result.translation[Xaxis] = avatar.translation.x;
	result.translation[Yaxis] = avatar.translation.y;
	result.translation[Zaxis] = avatar.translation.z;
	result.scale = avatar.scale[0][0];
	
	return result;
}

CameraPose TrackingResults::GetCameraPose()
{
	CameraPose result;
		
	result.position[Xaxis] = camera.position.x;
	result.position[Yaxis] = camera.position.y;
	result.position[Zaxis] = camera.position.z;
	result.target[Xaxis] = camera.target.x;
	result.target[Yaxis] = camera.target.y;
	result.target[Zaxis] = camera.target.z;
	result.upVector[Xaxis] = camera.upVector.x;
	result.upVector[Yaxis] = camera.upVector.y;
	result.upVector[Zaxis] = camera.upVector.z;

	return result;
}
