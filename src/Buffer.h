#include <stdio.h>
#include <Arduino.h>

struct Buffer{
private:
    uint8_t m_type;
    String m_mac;
    String m_payload;
    int length;


public:
    Buffer(char* packet);
    uint8_t getType(){return m_type;}
    String getMac(){return m_mac;}
    String getPayload(){return m_payload;}
    int getLength(){return length;}
    void print();
};