#ifndef _MATRIX_HELPER
#define _MATRIX_HELPER

#ifdef _DEBUG
#define GLM_MESSAGES
#endif
#include <glm/fwd.hpp>
#include <glm/glm.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct GLMViewParam
{
	glm::vec3 eulerAngles;
	glm::vec3 Translation; 
	glm::mat4 scale;
	glm::vec3 cameraPosition;
	glm::vec3 cameraTarget;
	glm::vec3 upVector;
};


class MatrixHelper
{

	static glm::mat4 Transform( glm::vec3 const & eulerAngles, 
								glm::vec3 const & Translation, 
								glm::mat4 const & scale,
								glm::vec3 const & cameraPosition,
								glm::vec3 const & cameraTarget,
								glm::vec3 const & upVector)
	{ 		
		glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f); 
		glm::mat4 ModelTranslation = glm::translate(glm::mat4(1.0f), Translation); 
		glm::quat quaternion = glm::quat(eulerAngles);
		glm::mat4 ModelRotation = glm::toMat4(quaternion);
		glm::mat4 Model = ModelRotation * ModelTranslation * scale;
		glm::mat4 View = glm::lookAt(
			cameraPosition, // the position of your camera, in world space
			cameraTarget,   // where you want to look at, in world space
			upVector        // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
		);
	
		return Projection * View * Model;
	}
};
#endif