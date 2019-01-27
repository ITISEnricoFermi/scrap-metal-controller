#ifndef PACKET_H
#define PACKET_H
#include <stdio.h>
#include <Arduino.h>

struct Packet
{
    uint8_t type = 0;
    String mac = "ff:ff:ff:ff:ff:ff";
    uint8_t payload_len = 0;
    uint8_t* payload;
    Packet();
    void release();
    void print();
};

bool parse(uint8_t* buffer, size_t len, Packet* packet);

#endif