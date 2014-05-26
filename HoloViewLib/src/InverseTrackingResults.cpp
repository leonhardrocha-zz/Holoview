#include "InverseTrackingResults.h"

Pose InverseTrackingResults::GetAvatarPose()
{
	Pose result;
	UpdateViewTransform();
	result.eulerAngles = glm::toQuat(View);
	glm::vec4 t = glm::column(View, 3);
	result.translation = glm::vec3(t.x, t.y, t.z);
	result.scale = glm::mat4(1.0f/avatar.scale[0][0]);
	return result;
}


void InverseTrackingResults::SetAvatarPose(const Pose& pose)
{
	avatar = pose;
}


void InverseTrackingResults::UpdateModelTransform()
{
	Model = glm::affineInverse(m_parent.GetModel());
}

void InverseTrackingResults::UpdateViewTransform()
{ 				
	View = glm::affineInverse(m_parent.GetView());
}

void InverseTrackingResults::UpdateProjectionTransform()
{
	Projection = glm::inverse(m_parent.GetProjection());
}

void InverseTrackingResults::UpdateModelViewTransform()
{
	ModelView = glm::affineInverse(m_parent.GetModelView());
}

void InverseTrackingResults::UpdateCameraViewTransform()
{
	CameraView = ModelView * Projection;
}