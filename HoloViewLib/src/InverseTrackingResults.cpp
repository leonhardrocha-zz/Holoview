#include "InverseTrackingResults.h"

Pose InverseTrackingResults::GetAvatarPose()
{
	Pose result;
	UpdateViewTransform();
    glm::quat q =glm::toQuat(View);
	result.eulerAngles = glm::eulerAngles(q);
	glm::vec4 t = glm::column(View, 3);
	result.translation = glm::vec3(t.x, t.y, t.z);
	result.scale = 1.0f;
	return result;
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