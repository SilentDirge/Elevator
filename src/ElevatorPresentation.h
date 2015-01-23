//
//  ElevatorPresentation.h
//  Elevator
//
//  Created by Aurelio Reis on 11/08/14.
//
//

#ifndef Elevator_ElevatorPresentation_h
#define Elevator_ElevatorPresentation_h

// forward decls
class Elevator;


////////////////////////////////////////////////////////////////////////
//
class ElevatorPresention {
friend class ElevatorApp;
    
public:
    ElevatorPresention();
    ~ElevatorPresention() {}
    
    void setTargetFloor(float pos);
    int WindowPosToFloor(float windowPos);
    float FloorToPos(int elevFloor);
    
    void update(float deltaTime, Elevator &elevator);
    
    void draw() {
        drawBackground();
        drawBuilding();
        drawFloors();
        drawShaft();
        drawElevator();
        drawDestinations();
        drawArrows();
    }
    
private:
    void drawBackground();
    void drawBuilding();
    void drawFloors();
    void drawShaft();
    void drawElevator();
    void drawDestinations();
    void drawArrows();
    void drawUpArrow(int elevFloor);
    void drawDownArrow(int elevFloor);
    
    float buildingWidth;
    const float halfBuildingWidth;
    float floorHeight;
    float elevWidth = 30.0f;
    float elevPos = FloorToPos(0);
    float targetFloorPos = elevPos;
    Elevator *elevator = nullptr;
    
    // sfx lambda (better done via system but for this demo its fine)
    std::function<void()> playBeep;
};

#endif
