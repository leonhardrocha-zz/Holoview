#ifdef _DEBUG
#include "vld.h"
#endif
#include "OpenGLHelper.h"

int PrintOglError(char *file, int line)
{    
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
		printf("\nglError #%d in file %s @ line %d: %s\n", 
			     glErr, file, line, gluErrorString(glErr));
        retCode = 1;
    }
    return retCode;
}

