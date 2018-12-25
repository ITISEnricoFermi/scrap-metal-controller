#include "Buffer.h"

Buffer::Buffer(char* packet){
    int offset = 0;

    //1 byte type
    //6 byte mac address
    //1 byte payload length
    //0 ~ 128 byte payload

    m_type = packet [offset++];

    uint8_t mac[6];

    for(int i = 0; i < 6; i++, offset++){
        mac[i] = packet[offset];
    }
    
    char macStr[18] = { 0 };
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    m_mac = String(macStr);
    length = (int)packet[offset++];
    char payload[length];
    for(int i = 0 ; i < length; i++){
        payload[i] = packet[offset + i];
    }    
    m_payload = String(payload);

}


void Buffer::print(){
    Serial.println("-----------------");
    Serial.print("type: ");
    Serial.println(m_type);
    
    Serial.print("mac: ");
    Serial.println(m_mac);
    
    Serial.println("payload:");
    Serial.println(m_payload);
    Serial.println("-----------------");
}
