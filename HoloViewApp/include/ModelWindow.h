#include "OpenGLWindow.h"
#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <OpenGLHelper.h>

class ModelWindow : public OpenGLWindow
{
public:
    ModelWindow();

    void initialize();
    void render();

private:
    GLuint loadShader(GLenum type, const char *source);

    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
	AssetReader asset;
    QOpenGLShaderProgram *m_program;
    int m_frame;
};
