#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "FMOD.hpp"

#include "Elevator.h"
#include "ElevatorPresentation.h"

using namespace ci;
using namespace ci::app;
using namespace std;


/*
 How to play:
    Left click on a floor for up button
    Right click on a floor for down button
 */

class ElevatorApp : public AppNative {
  public:
    ~ElevatorApp() {
        delete elevatorPresention;
    }

	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

    FMOD::System	*mSystem;
    FMOD::Sound    	*mMusic;
    FMOD::Sound    	*mBeep;
    FMOD::Channel	*mChannel;
    
    Elevator elevator;
    ElevatorPresention *elevatorPresention;
    double lastTime;
    
private:
    void initSound();
};

void ElevatorApp::initSound() {
    FMOD::System_Create( &mSystem );
    mSystem->init( 32, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL );
    
    mSystem->createSound( getAssetPath( "bensound-theelevatorbossanova.mp3" ).string().c_str(), FMOD_SOFTWARE, NULL, &mMusic );
    mMusic->setMode( FMOD_LOOP_NORMAL );
    
    mSystem->createSound( getAssetPath( "lift_arrive_beep.wav" ).string().c_str(), FMOD_SOFTWARE, NULL, &mBeep );
    
    mSystem->playSound( FMOD_CHANNEL_FREE, mMusic, false, &mChannel );
}

void ElevatorApp::setup()
{
    initSound();
    
    elevatorPresention = new ElevatorPresention();
    lastTime = cinder::app::getElapsedSeconds();

    elevatorPresention->playBeep = [&]() { mSystem->playSound( FMOD_CHANNEL_FREE, mBeep, false, &mChannel ); };
}

void ElevatorApp::mouseDown( MouseEvent event )
{
    int8_t dir = Elevator::None;
    if (event.isRightDown()) {
        dir = Elevator::Down;
    } else if (event.isLeftDown()) {
        dir = Elevator::Up;
    }
    
    if (dir != Elevator::None) {
        float pos = toPixels(event.getY());
        int8_t elevFloor = elevatorPresention->WindowPosToFloor(pos);
        
        elevator.SummonInterrupt(elevFloor, dir);
    }
}

void ElevatorApp::update()
{
    double currentTime = cinder::app::getElapsedSeconds();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    elevatorPresention->update((float)deltaTime, elevator);
}

void ElevatorApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

    elevatorPresention->draw();
}

CINDER_APP_NATIVE( ElevatorApp, RendererGl )
