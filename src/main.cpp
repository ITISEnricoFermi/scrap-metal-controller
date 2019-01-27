
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Packet.h"
#include "Stuff.h"


//#include <IPAddress.h>
#define UDP_TX_PACKET_MAX_SIZE 200 //modify in packet.cpp

//Functions declaration
void printWiFiInfo();
void printBufferHex(char* buff, size_t length);
void printBufferHex(String buff, size_t length);
void printBufferHex(uint8_t buff, size_t length);
void connectToWiFi(const char * ssid, const char * pwd);
void WiFiEvent(WiFiEvent_t event);
IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet);
void reply();
    
void updateBoard(Packet* packet,String mac);
void initBoard();
void processBoard();

// WiFi network name and password:
const char * networkName = "Sol 2.4";
const char * networkPswd = "U43UU8CRUR";

//IP address to send UDP data to:
IPAddress broadcast;
const int remotePort = 0xbeaf;  //51966 0xcafe
const int localPort  = 0xcafe;  //48815 0xbeaf

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

//delay between each packet
const unsigned int beatRate = 5000;

// buffers for receiving and sending data
uint8_t* packetBuffer; //buffer to hold incoming packet,

//our packet container
Packet* packet;

//board class
Board* board;

void setup()
{
  // Initialize hardware serial:
  Serial.begin(115200);

  //initialize the board based on the device type
  board = new Board();
  
  //allocates memory for udp incoming data
  packetBuffer = (uint8_t*) malloc(UDP_TX_PACKET_MAX_SIZE * sizeof(uint8_t));

  //allocates memory for packet
  packet = new Packet();

  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}

void loop()
{

  //process the board based on it's type
  board->process();

  if(!connected) return; // if not connected don't process
  
  memset(packetBuffer, 0, sizeof(uint8_t) * UDP_TX_PACKET_MAX_SIZE);
  //loop if receives data
  int packetSize = udp.parsePacket();
  if (packetSize) 
  {
    //if packet too large return
    if(packetSize > UDP_TX_PACKET_MAX_SIZE)
    {
      udp.flush();
      return;
    }

    //if packet too small return
    if(packetSize < 8)
    {
      udp.flush();
      return;
    }

    // read the packet into packetBufffer
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    
    //parse the packet and update the board
    parse(packetBuffer, packetSize, packet);
    board->update(packet,(char *)WiFi.macAddress().c_str());
    
    //print buffer content
    packet->print();

    //reply with parsed packet
    reply();

    //clear the buffer
    udp.flush();

    //release the packet
    packet->release();
  }

  //send packet at beatRate
  if(!(millis() % beatRate))
  {
    //Send a packet
    udp.beginPacket(broadcast,remotePort);
    udp.printf("{\n\t\"mac\": \"%s\",\n\t\"type\":%i\n}", WiFi.macAddress().c_str(), LAMPADA);
    udp.endPacket();

    Serial.println("[*] packet sent!"); 
    
    //wait 1ms to ensure that cond doesn't repeat
    delay(1);
  }
}

void connectToWiFi(const char * ssid, const char * pwd)
{
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

void WiFiEvent(WiFiEvent_t event)
{
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

void printWiFiInfo()
{
  if(!connected){
    Serial.println("Not connected to a network!");
    return;
  }
  Serial.println("  ~ WiFi info ~  ");
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


void printBufferHex(char* buff, size_t length)
{
  for(size_t i = 0; i < length; i++){
    Serial.print(buff[i],HEX);
    Serial.print(' ');
  }
  Serial.print('\n');
}
void printBufferHex(uint8_t* buff, size_t length)
{
  for(size_t i = 0; i < length; i++){
    Serial.print((char)buff[i],HEX);
    Serial.print(' ');
  }
  Serial.print('\n');
}
void printBufferHex(String buff, size_t length)
{
  for(size_t i = 0; i < length; i++){
    Serial.print(buff[i],HEX);
    Serial.print(' ');
  }
  Serial.print('\n');
}

void reply()
{
    Serial.println("reply!");
    //packet header
    udp.beginPacket(broadcast,remotePort);

    //packet content
    udp.print("{\n\t\"type\":\"");
    udp.write(packet->type);
    udp.print("\",\n\t\"mac\": \"");
    udp.print(packet->mac.c_str());
    udp.print("\",\n\t\"pld_len\":\"");
    udp.write(packet->payload_len);
    udp.print("\",\n\t\"payload\":\"");
    udp.write(packet->payload, packet->payload_len);
    udp.print("\"\n}");

    //packet tail
    udp.endPacket();

    //old format
    //udp.printf("{\n\t\"type\":\"%c\",\n\t\"mac\": \"%s\",\n\t\"pld_len\":\"%i\",\n\t\"payload\":\'%s\'\n}", (char)packet->type, packet->mac.c_str(), (int)packet->payload_len, (char*)packet->payload);
}

IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet)
{
  IPAddress broadcast;
  broadcast = ip | ( ~ subnet );
  return broadcast;
}