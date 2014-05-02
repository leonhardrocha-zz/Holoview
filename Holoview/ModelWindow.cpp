#include "stdafx.h"
#include "ModelWindow.h"

static const char *vertexModelShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentModelShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";


ModelWindow::ModelWindow()
    : m_program(0)
    , m_frame(0)
{

}

void ModelWindow::initialize()
{
    /*m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexModelShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentModelShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");*/

}

void ModelWindow::render()
{
    ++m_frame;
}