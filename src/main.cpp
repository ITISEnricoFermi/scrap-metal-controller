
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Packet.h"
//#include <IPAddress.h>
#define UDP_TX_PACKET_MAX_SIZE 512

//Functions declaration
void printWiFiInfo();
void printBufferHex(char* buff, size_t length);
void printBufferHex(String buff, size_t length);
void printBufferHex(uint8_t buff, size_t length);
void connectToWiFi(const char * ssid, const char * pwd);
void WiFiEvent(WiFiEvent_t event);
IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet);
void reply();

// WiFi network name and password:
const char * networkName = "Sol 2.4";
const char * networkPswd = "U43UU8CRUR";

//IP address to send UDP data to:
IPAddress broadcast;
const int remotePort = 0xcafe;  //51966
const int localPort  = 0xbeaf;  //48815

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

//delay between each packet
const unsigned int beatRate = 5000;


// buffers for receiving and sending data
uint8_t packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
size_t offset = 0;
Packet packet;


void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  

  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}

void loop(){
  if(!connected) return;


  int packetSize = udp.parsePacket();
  if (packetSize) {
    if(packetSize > UDP_TX_PACKET_MAX_SIZE){
    //clear the buffer
    memset(packetBuffer, 0, sizeof(char) * UDP_TX_PACKET_MAX_SIZE);
    return;
    }
    if(packetSize < 8){
      Serial.println("packet not valid!");
      memset(packetBuffer, 0, sizeof(uint8_t) * packetSize);
      return;
    }

    //Serial.print("Received packet of size ");
    //Serial.println(packetSize);

    // read the packet into packetBufffer
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    parse(packetBuffer, packetSize, packet);
    packet.print();
    reply();
    //Serial.println("Content:");
    //printBufferHex(packet.getPayload(), packet.getLength());
    //printBufferHex(packetBuffer, packetSize);
    //Serial.println(packetBuffer);

    //clear the buffer
    memset(packetBuffer, 0, sizeof(uint8_t) * packetSize);

  }
  //send data only if connected
  if(connected && !(millis() % beatRate)){

    //Send a packet
    //udp.begin(WiFi.localIP(),localPort);
    udp.beginPacket(broadcast,remotePort);
    udp.printf("{\n\t\"mac\": \"%s\"\n}", WiFi.macAddress().c_str());
    udp.endPacket();
    Serial.println("packet sent!");
    
    //wait 1ms to ensure that cond doesn't repeat
    delay(1);
  }
  //Print connection info
  //printWiFiInfo();

  //Wait for 1 second
  //delay(1000);
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);
  WiFi.setHostname("lampada-salone");
  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP()); 
          //initializes the UDP state
          udp.begin(localPort); 
          //This initializes the transfer buffer
          connected = true;
          broadcast = calculateBroadcast(WiFi.localIP(),WiFi.subnetMask());
          printWiFiInfo();
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}

void printWiFiInfo(){
  if(!connected){
    Serial.println("Not connected to a network!");
    return;
  }
  Serial.println("  ~ WiFi info ~  ");
  //Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP Mac Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Broadcast IP: ");
  Serial.println(broadcast);
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.println("--------------------------");
}


void printBufferHex(char* buff, size_t length){
  for(size_t i = 0; i < length; i++){
    Serial.print(buff[i],HEX);
    Serial.print(' ');
  }
  Serial.print('\n');
}
void printBufferHex(uint8_t* buff, size_t length){
  for(size_t i = 0; i < length; i++){
    Serial.print((char)buff[i],HEX);
    Serial.print(' ');
  }
  Serial.print('\n');
}
void printBufferHex(String buff, size_t length){
  for(size_t i = 0; i < length; i++){
    Serial.print(buff[i],HEX);
    Serial.print(' ');
  }
  Serial.print('\n');
}

void reply(){
    Serial.println("reply!");

    udp.beginPacket(broadcast,remotePort);
    udp.printf("{\n\t\"type\":\"%c\",\n\t\"mac\": \"%s\",\n\t\"pld_len\":\"%i\",\n\t\"payload\":\"%s\"\n}", (char)packet.type, packet.mac.c_str(), (int)packet.payload_len, packet.payload.c_str());
    udp.endPacket();
}

IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet){
  IPAddress broadcast;
  broadcast = ip | ( ~ subnet );
  return broadcast;
}