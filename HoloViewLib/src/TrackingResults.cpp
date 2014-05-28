#include "TrackingResults.h"


void TrackingResults::UpdateModelTransform()
{
	glm::quat q = glm::quat(avatar.eulerAngles);
	glm::mat4 rotationMatrix = glm::toMat4(q);

	glm::vec3 foward = glm::normalize(camera.target - camera.position);
	glm::vec3 side = glm::normalize(glm::cross(foward, camera.viewUp));
	glm::vec3 up = glm::cross(side, foward);

	glm::mat3 viewRotation(1.0f);
	viewRotation = glm::column(viewRotation, 0, side);
	viewRotation = glm::column(viewRotation, 1, up);
	viewRotation = glm::column(viewRotation, 2, foward);

	glm::quat q2 = glm::toQuat(viewRotation);
	camera.eulerAngles = glm::eulerAngles(q2);
	glm::vec3 angles = glm::degrees(camera.eulerAngles);
	glm::vec3 angles2 = glm::degrees(avatar.eulerAngles);
	avatar.position = camera.position + viewRotation * avatar.translation;
	
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), avatar.position);
	glm::mat4 scaleMatrix = glm::mat4(avatar.scale);
	Model = translationMatrix * rotationMatrix * scaleMatrix;
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
	CameraView = glm::lookAt(glm::vec3(camera.target.x, camera.target.y, 0.0f), camera.target, camera.viewUp) * Model;
}

Pose TrackingResults::GetPose()
{
	Pose result = avatar;
	glm::mat4 view = GetModel();
	
	glm::quat q = glm::toQuat(view);
	result.eulerAngles = glm::eulerAngles(q);
	
	glm::vec4 t = glm::column(view, 3);
	result.position = glm::vec3(t);

	return result;
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

void TrackingResults::SetPose(const Pose& pose)
{
	avatar = pose;
}


void TrackingResults::SetAvatarPose(const Pose& pose)
{
	avatar = pose;
}

void TrackingResults::SetCameraPose(const Pose& pose)
{
	camera = pose;
}

