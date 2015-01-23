//
//  ElevatorPresentation.cpp
//  Elevator
//
//  Created by Aurelio Reis on 11/08/14.
//
//

#include "ElevatorPresentation.h"
#include "cinder/app/AppNative.h"

#include "Elevator.h"

using namespace ci;
using namespace ci::app;
using namespace std;

ElevatorPresention::ElevatorPresention()
    :   buildingWidth(getWindowWidth() / 4),
        halfBuildingWidth(buildingWidth / 2.0f),
        floorHeight(getWindowHeight() / Elevator::MAX_FLOORS)
 {
    
}

void ElevatorPresention::setTargetFloor(float pos) {
    targetFloorPos = floorf( pos / floorHeight ) * floorHeight + ( floorHeight * 0.5f );
}

int ElevatorPresention::WindowPosToFloor(float windowPos) {
    return floorf( ( getWindowHeight() - windowPos ) / floorHeight );
}

float ElevatorPresention::FloorToPos(int elevFloor) {
    return getWindowHeight() - elevFloor * floorHeight - ( floorHeight * 0.5f );
}

void ElevatorPresention::update(float deltaTime, Elevator &_elevator) {
    elevator = &_elevator;

    const float elevatorSpeed = 50.0f;
    
    if (elevator->GetDir() != Elevator::None) {
        float targetPos = FloorToPos(elevator->GetNextFloor());
        targetFloorPos = targetPos;
        
        // move the simulation forward while not at destination
        if (targetFloorPos != elevPos) {
            float dif = (targetFloorPos - elevPos);
            if (fabs(dif) < 3.0f) {
                elevPos = targetFloorPos;
                elevator->FloorInterrupt(WindowPosToFloor(elevPos));
                playBeep();
            } else {
                float vel = dif >= 0.1f ? 1.0f : (dif <= -0.1f ? -1.0f : 0.0f);
                elevPos += vel * elevatorSpeed * deltaTime;
            }
            
            elevator->SetFloor(WindowPosToFloor(elevPos));
        }
    }
}

void ElevatorPresention::drawBackground() {
    gl::color( Color( 1.0f, 0.0f, 0.0f ) );
    Rectf screenRect( 0.0f, 0.0f, getWindowWidth(), getWindowHeight() );
    
    glBegin(GL_QUADS);
    gl::color( Color( 0.5f, 0.75f, 1.0f ) );
    gl::vertex( screenRect.x2, screenRect.y1 );
    gl::vertex( screenRect.x1, screenRect.y1 );
    gl::color( Color( 0.85f, 0.95f, 1.0f ) );
    gl::vertex( screenRect.x1, screenRect.y2 );
    gl::vertex( screenRect.x2, screenRect.y2 );
    glEnd();
    
    float stride = getWindowWidth() / 12.0f;
    for (auto i = 0; i < 13; ++i) {
        gl::color( Color( 0.5f, 0.8f, 0.2f ) );
        gl::drawSolidEllipse( Vec2f( i * stride, getWindowHeight() - 10.0f ), 64, 128);
    }
}

void ElevatorPresention::drawBuilding() {
    Vec2f pos( getWindowWidth() / 2, getWindowHeight() / 2 );
    Rectf r( 0.0f, 0.0f, buildingWidth, getWindowHeight() );
    r.offsetCenterTo( pos );
    
    // building fill
    gl::color( Color( 0.8f, 0.8f, 0.8f ) );
    gl::drawSolidRoundedRect( r, 10.0f, 50.0f );
    
    // building outline
    gl::lineWidth( 20.0f );
    gl::color( Color( 0.3f, 0.3f, 0.3f ) );
    gl::drawStrokedRoundedRect( r, 10.0f, 50.0f );
}

void ElevatorPresention::drawFloors() {
    const float halfScreen = getWindowWidth() / 2.0f;
    
    gl::lineWidth( 5.0f );
    gl::color( Color( 0.3f, 0.3f, 0.3f ) );
    
    // floor lines
    for (auto i = 0; i < Elevator::MAX_FLOORS + 1; ++i) {
        Vec2f start( halfScreen - halfBuildingWidth, i * 32.0f );
        Vec2f end( halfScreen + halfBuildingWidth, i * 32.0f );
        gl::drawLine( start, end );
    }
}

void ElevatorPresention::drawShaft() {
    Vec2f pos( getWindowWidth() / 2, getWindowHeight() / 2 );
    
    // elevator shaft
    Rectf shaftRect( 0.0f, 0.0f, elevWidth, getWindowHeight() );
    shaftRect.offsetCenterTo( pos );
    gl::lineWidth( 2.0f );
    gl::color( Color( 0.2f, 0.2f, 0.2f ) );
    gl::drawSolidRect( shaftRect );
}

void ElevatorPresention::drawElevator() {
    const float halfScreen = getWindowWidth() / 2.0f;
    
    //        float elevPos = getWindowHeight() - ( curFloor * floorHeight + floorHeight / 2.0f );
    float newElevPos = elevPos;
    Rectf elevRect( 0.0f, 0.0f, elevWidth, floorHeight );
    elevRect.offsetCenterTo( Vec2f( halfScreen, newElevPos ) );
    
    // elevator
    gl::color( Color( 0.8f, 0.6f, 0.5f ) );
    gl::drawSolidRoundedRect( elevRect, 3.0f );
}

void ElevatorPresention::drawDestinations() {
    for (auto floor = 0; floor < Elevator::MAX_FLOORS; ++floor) {
        if (elevator->GetDestinations(floor) == 1) {
            const float halfScreen = getWindowWidth() / 2.0f;
            
            float elevPos = getWindowHeight() - ( floor * floorHeight + floorHeight / 2.0f );
            
            gl::lineWidth( 3.0f );
            gl::color( Color( 0.8f, 0.3f, 0.5f ) );
            
            gl::drawSolidCircle(Vec2f(halfScreen + 52, elevPos), floorHeight / 2.0f);
        }
    }
}

void ElevatorPresention::drawArrows() {
    for (auto floor = 0; floor < Elevator::MAX_FLOORS; ++floor) {
        auto summons = elevator->GetFloorSummons(floor);
        
        if (summons & Elevator::Up) {
            drawUpArrow(floor);
        }
        if (summons & Elevator::Down) {
            drawDownArrow(floor);
        }
    }
}

void ElevatorPresention::drawUpArrow(int elevFloor) {
    const float halfScreen = getWindowWidth() / 2.0f;
    
    float elevPos = getWindowHeight() - ( elevFloor * floorHeight + floorHeight );
    
    gl::lineWidth( 3.0f );
    gl::color( Color( 0.6f, 0.6f, 0.0f ) );
    
    Vec2f start( halfScreen - 32, elevPos );
    Vec2f end( halfScreen - 32, elevPos + floorHeight );
    gl::drawLine( start, end );
    
    start = Vec2f( halfScreen - 32, elevPos );
    end = Vec2f( halfScreen - 8 - 32, elevPos + floorHeight / 2.0f );
    gl::drawLine( start, end );
    
    start = Vec2f( halfScreen - 32, elevPos );
    end = Vec2f( halfScreen + 8 - 32, elevPos + floorHeight / 2.0f );
    gl::drawLine( start, end );
}

void ElevatorPresention::drawDownArrow(int elevFloor) {
    const float halfScreen = getWindowWidth() / 2.0f;
    
    float elevPos = getWindowHeight() - ( elevFloor * floorHeight + floorHeight );
    
    gl::lineWidth( 3.0f );
    gl::color( Color( 0.0f, 0.6f, 0.6f ) );
    
    Vec2f start( halfScreen + 32, elevPos );
    Vec2f end( halfScreen + 32, elevPos + floorHeight );
    gl::drawLine( start, end );
    
    start = Vec2f( halfScreen - 8 + 32, elevPos + floorHeight / 2.0f );
    end = Vec2f( halfScreen + 32, elevPos + floorHeight );
    gl::drawLine( start, end );
    
    start = Vec2f( halfScreen + 8 + 32, elevPos + floorHeight / 2.0f );
    end = Vec2f( halfScreen + 32, elevPos + floorHeight );
    gl::drawLine( start, end );
}

