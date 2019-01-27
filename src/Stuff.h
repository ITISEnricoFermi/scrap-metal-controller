#ifndef STUFF_H
#define STUFF_H
#include "Packet.h"
#include <Arduino.h>
#include <AnalogWrite.h>
#define LAMPADA 0x41 // => A

class Board
{
private:
    bool buffCompare(uint8_t* buff, char* str, uint8_t len);
public:
    int state = 0x1;
    //const int rePin = 32;
    const int R = 25;
    const int G = 23;
    const int B = 32;
    uint8_t RState = 0;
    uint8_t GState = 0;
    uint8_t BState = 0;
    //const int LAMPADA = 0x41;

    void update(Packet* packet, char* mac);
    Board();
    void process();
};

#endif