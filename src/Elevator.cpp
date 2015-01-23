//
//  Elevator.cpp
//  Elevator
//
//  Created by Aurelio Reis on 11/08/14.
//
//

#include "Elevator.h"

#include "cinder/Rand.h"
using namespace ci;
using namespace ci::app;


Elevator::Elevator() {
}

Elevator::~Elevator() {
}

// called when the elevator arrives on a given floor
void Elevator::FloorInterrupt(int8_t elevFloor) {
    int8_t curDir = GetDir();
    
    SetFloor(elevFloor);
    
    if (GetDestinations(elevFloor) == 0) {
        // generate a floor # to take the passenger to (in the
        // specified direction of travel)
        int8_t passengerFloor;
        if (GetFloorSummons(elevFloor) & Up) {
            passengerFloor = Rand::randInt(elevFloor + 1, MAX_FLOORS);
        } else {
            passengerFloor = Rand::randInt(0, elevFloor - 1);
        }
        passengerFloor = min(max(passengerFloor, (int8_t)0), int8_t(MAX_FLOORS - 1));
        AddPassengerDestination(passengerFloor);
    }
    SetDestinations(elevFloor, 0);
    
    // another interesting edge case: remove just the flag for the current direction
    // unless there is just a single summon flag
    if (GetFloorSummons(elevFloor) & 0x01) {
        RemoveSummonDir(elevFloor, 0x0F);
    } else {
        RemoveSummonDir(elevFloor, curDir);
    }
    
    // keep going up
    if (curDir == Up) {
        if (!floorsUp.empty()) {
            nextFloor = floorsUp.top();
            floorsUp.pop();
        } else if (!floorsDown.empty()) {
            nextFloor = floorsDown.top();
            floorsDown.pop();
            FlipDir();
        } else {
            SetDir(None);
        }
        // keep going down
    } else if (curDir == Down) {
        if (!floorsDown.empty()) {
            nextFloor = floorsDown.top();
            floorsDown.pop();
        } else if (!floorsUp.empty()) {
            nextFloor = floorsUp.top();
            floorsUp.pop();
            FlipDir();
        } else {
            SetDir(None);
        }
    }
    
    // catch edge case where up and down pressed at same time
    if (GetDir() != None && nextFloor == elevFloor) {
        FloorInterrupt(elevFloor);
    }
}

// called when a user summons the elevator
void Elevator::SummonInterrupt(int8_t elevFloor, int8_t dir) {
    if (elevFloor == (Elevator::MAX_FLOORS - 1) && dir == Elevator::Up) {
        return;
    }
    if (elevFloor == 0 && dir == Elevator::Down) {
        return;
    }
    
    int8_t curFloor = GetFloor();
    int8_t curDir = GetDir();
    const int8_t dirDif = elevFloor - curFloor;
    int8_t newDir;
    
    if (dirDif > 0) {
        newDir = Up;
    } else if (dirDif < 0) {
        newDir = Down;
    } else {
        return;
    }
    
    AddFloorSummons(elevFloor, dir);
    
    // if we're summoned to this floor remove any destination
    // since it's redundant
    if (GetDestinations(elevFloor) != 0) {
        SetDestinations(elevFloor, 0);
        return;
    }
    
    // if we aren't moving yet go ahead and move towards this floor
    if (curDir == None) {
        nextFloor = elevFloor;
        // set the direction to that of the new summoned floor so
        // that we are going the right direction when we get there
        SetDir(newDir);
    } else {
        if (curDir == Up) {
            if (elevFloor > curFloor) {
                if (dir == Down) {
                    floorsDown.push(elevFloor);
                } else if (elevFloor > nextFloor) {
                    floorsUp.push(elevFloor);
                } else {
                    floorsUp.push(nextFloor);
                    nextFloor = elevFloor;
                }
            } else {
                floorsDown.push(elevFloor);
            }
        } else if (curDir == Down) {
            if (elevFloor < curFloor) {
                if (dir == Up) {
                    floorsUp.push(elevFloor);
                } else if (elevFloor < nextFloor) {
                    floorsDown.push(elevFloor);
                } else {
                    floorsDown.push(nextFloor);
                    nextFloor = elevFloor;
                }
            } else {
                floorsUp.push(elevFloor);
            }
        }
    }
}

void Elevator::AddPassengerDestination(int8_t passengerFloor) {
    if (floorSummons[passengerFloor] != None) {
        return;
    }
    
    SetDestinations(passengerFloor, 1);
    
    int8_t curFloor = GetFloor();
    const int8_t dirDif = passengerFloor - curFloor;
    int8_t newDir;
    
    if (dirDif > 0) {
        newDir = Up;
    } else if (dirDif < 0) {
        newDir = Down;
    } else {
        return;
    }
    
    switch (newDir) {
        case Up:
            floorsUp.push(passengerFloor);
            break;
            
        case Down:
            floorsDown.push(passengerFloor);
            break;
    }
}