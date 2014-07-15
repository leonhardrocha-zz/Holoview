#include <osgViewer/api/Win32/GraphicsWindowWin32>
#include "JoystickManipulator.h"

LPDIRECTINPUT8 g_inputDevice;
LPDIRECTINPUTDEVICE8 g_joystick;

namespace osgGA {

static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* didInstance, VOID* )
{
    HRESULT hr = NULL;
    if ( g_inputDevice )
    {
        hr = g_inputDevice->CreateDevice( didInstance->guidInstance, &g_joystick, NULL );
    }
    if ( FAILED(hr) ) return DIENUM_CONTINUE;
    return DIENUM_STOP;
}

JoystickManipulator::JoystickManipulator()
:   _distance(1.0)
{
    HRESULT hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&g_inputDevice, NULL );
    if ( FAILED(hr) || !g_inputDevice ) return;
    
    hr = g_inputDevice->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY );
}

JoystickManipulator::~JoystickManipulator()
{
    if ( g_joystick ) 
    {
        g_joystick->Unacquire();
        g_joystick->Release();
    }
    if ( g_inputDevice ) g_inputDevice->Release();
}

osg::Matrixd JoystickManipulator::getMatrix() const
{
    osg::Matrixd matrix;
    matrix.makeTranslate( 0.0f, 0.0f, _distance );
    matrix.postMultTranslate( _center );
    return matrix;
}

osg::Matrixd JoystickManipulator::getInverseMatrix() const
{
    osg::Matrixd matrix;
    matrix.makeTranslate( 0.0f, 0.0f,-_distance );
    matrix.preMultTranslate( -_center );
    return matrix;
}

void JoystickManipulator::setByMatrix( const osg::Matrixd& matrix )
{
    setByInverseMatrix( osg::Matrixd::inverse(matrix) );
}

void JoystickManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
    osg::Vec3d eye, center, up;
    matrix.getLookAt( eye, center, up );
    
    _center = center;
    if ( _node.valid() )
        _distance = abs((_node->getBound().center() - eye).z());
    else
        _distance = abs((eye - center).length());
}

void JoystickManipulator::init( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    const osgViewer::GraphicsWindowWin32* gw =
            dynamic_cast<const osgViewer::GraphicsWindowWin32*>( ea.getGraphicsContext() );
    if ( gw && g_joystick )
    {
        DIDATAFORMAT format = c_dfDIJoystick2;
        g_joystick->SetDataFormat( &format );
        g_joystick->SetCooperativeLevel( gw->getHWND(), DISCL_EXCLUSIVE|DISCL_FOREGROUND );
        g_joystick->Acquire();
    }
}

bool JoystickManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    if ( g_joystick && ea.getEventType()==osgGA::GUIEventAdapter::FRAME )
    {
        HRESULT hr = g_joystick->Poll();
        if ( FAILED(hr) ) g_joystick->Acquire();
        
        DIJOYSTATE2 state;
        hr = g_joystick->GetDeviceState( sizeof(DIJOYSTATE2), &state );
        if ( FAILED(hr) ) return false;
        
        double dx = 0.0, dy = 0.0;
        if ( state.lX==0x0000 ) dx -= 0.01;
        else if ( state.lX==0xffff ) dx += 0.01;
        else dx -= 0.01 * (double(state.lX)/double(0xffff) - 0.5) * 2;
        
        if ( state.lY==0 ) dy -= 0.01;
        else if ( state.lY==0xffff ) dy += 0.01;
        else dy += 0.01 * (double(state.lY)/double(0xffff) - 0.5) * 2;
        
        if ( state.rgbButtons[0] )
            performMovementLeftMouseButton( 0.0, dx, dy );
        if ( state.rgbButtons[1] )
            performMovementRightMouseButton( 0.0, dx, dy );
    }
    return false;
}

void JoystickManipulator::home( double )
{
    if ( _node.valid() )
    {
        _center = _node->getBound().center();
        _distance = 2.5 * _node->getBound().radius();
    }
    else
    {
        _center.set( osg::Vec3() );
        _distance = 1.0;
    }
}

bool JoystickManipulator::performMovementLeftMouseButton(
    const double eventTimeDelta, const double dx, const double dy )
{
    _center.x() -= 10.0f * dx;
    _center.z() -= 10.0f * dy;
    return false;
}

bool JoystickManipulator::performMovementRightMouseButton(
    const double eventTimeDelta, const double dx, const double dy )
{
    _distance *= (1.0 + dy);
    if ( _distance<1.0 ) _distance = 1.0;
    return false;
}

}