//
//  Elevator.h
//  Elevator
//
//  Created by Aurelio Reis on 11/08/14.
//
//

#ifndef __Elevator__Elevator__
#define __Elevator__Elevator__

#include <queue>
using namespace std;


/*
 Elevator rules:
 If summoned, do pickup and drop off at destination
 If additional summons, only pick up and drop off along the current direction
 ex: pickup/dropoff all in same direction then next nearest in opposite direction and so on
 If a summons is received for the opposite direction, push it to a priority
 queue (sorted by floor to ensure order)
 Elevator is event driven:
 a floor interrupt is called when a "sensor" detects we have reached a goal floor
 a summon interrupt is called when a directional "button" is pressed
 
 Implementaton details:
 Priority queues ensure floor events are 'chained' in proper order and avoid unnecesary backtracking
 Data is tightly packed via bitfields to ensure optimal cpu cache coherency
 Simulation is decoupled from presentation and interface (MVC)
 */
class Elevator {
public:
    Elevator();
    ~Elevator();
    
    // called when the elevator arrives on a given floor
    void FloorInterrupt(int8_t elevFloor);
    
    // called when a user summons the elevator
    void SummonInterrupt(int8_t elevFloor, int8_t dir);
    
    int8_t GetDestinations(int8_t elevFloor) const {
        return (floorSummons[elevFloor] & 0xF0) >> 4;
    }
    
    void SetDestinations(int8_t elevFloor, int8_t val) {
        floorSummons[elevFloor] = (floorSummons[elevFloor] & 0x0F) | (val << 4);
    }
    
    int8_t GetFloorSummons(int8_t elevFloor) const {
        return floorSummons[elevFloor] & 0x0F;
    }
    
    void SetFloorSummons(int8_t elevFloor, int8_t val) {
        floorSummons[elevFloor] = (floorSummons[elevFloor] & 0xF0) | val;
    }
    
    void AddFloorSummons(int8_t elevFloor, int8_t val) {
        floorSummons[elevFloor] = (floorSummons[elevFloor] & 0xFF) | val;
    }
    
    void RemoveSummonDir(int8_t elevFloor, int8_t dir) {
        floorSummons[elevFloor] &= ~(dir & 0x0F);
    }
    
    void SetFloorDir(int8_t elevFloor, int8_t dir) {
        elevator = (elevFloor & 0xF) | ((dir & 0x3) << 16);
    }
    
    void SetFloor(int8_t elevFloor) {
        SetFloorDir(elevFloor, GetDir());
    }
    
    void SetDir(int8_t dir) {
        SetFloorDir(GetFloor(), dir);
    }
    
    int8_t GetFloor() const {
        return elevator & 0xF;
    }
    
    int8_t GetNextFloor() const {
        return nextFloor;
    }
    
    int8_t GetDir() const {
        return (elevator >> 16) & 0x3;
    }
    
    // Movement and floor states
    enum { None = 0, Up, Down };
    
    static const int MAX_FLOORS = 15;
    
private:
    void FlipDir() {
        int8_t dir = GetDir();
        if (dir != None) {
            SetDir(dir == Up ? Down : Up);
        }
    }
    
    void AddPassengerDestination(int8_t passengerFloor);
    
    // first 4 bits = current floor, second 4 bits = movement
    // floor values: 0-15
    // movement values: none(0), up(1), down(2)
    int32_t elevator = 0;

    // the floor to move to
    int8_t nextFloor = 0;
    
    // prioritized list of floors going up or down
    priority_queue<int8_t, std::vector<int8_t>, std::greater<int8_t>>   floorsUp;
    priority_queue<int8_t, std::vector<int8_t>, std::less<int8_t>>      floorsDown;
    
    // the state of each floor summons (none, up or down) and destinations
    int8_t floorSummons[MAX_FLOORS]{};
};

#endif /* defined(__Elevator__Elevator__) */
