#include "InverseTrackingResults.h"

void InverseTrackingResults::UpdateModelTransform()
{
	Model = glm::affineInverse(m_parent.GetModel());
}

CameraCoordSystem InverseTrackingResults::GetCameraCoordSystem()
{
	CameraCoordSystem coordSys(GetModelView());
	return coordSys;
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
	ModelView = Model * View;
}

void InverseTrackingResults::UpdateCameraViewTransform()
{
	CameraView = ModelView * Projection;
}