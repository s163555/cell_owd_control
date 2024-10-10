#include <Wire.h>
#include "multi_channel_relay.h"

// Constants
const byte MCP23017_ADDR1 = 0x20; // First MCP23017 address
const byte MCP23017_ADDR2 = 0x21; // Second MCP23017 address
const int relayPins[] = {3, 4, 5, 6, 7, 8}; // Pins for controlling relays

// State variables to track the on/off state of each MCP23017 pin
byte mcpState[4] = {0b00000000, 0b00000000, 0b11000000, 0b00001111};

// Relay control object
Multi_Channel_Relay relay;

void setup() {
  Serial.setTimeout(100);
  Serial.begin(57600); 

  relay.begin(0x11); // Initialize relay
  initRelayPins();   // Initialize GPIO pins for relay
  initMCP();         // Initialize MCP23017
  
  Serial.println("Relay Control Ready");
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readString();
    processSerialCommand(data);
  }
}

// Initialize GPIO pins for relay control
void initRelayPins() {
  for (int i = 0; i < 6; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW); // Default relay state
  }
}

// Initialize MCP23017 (set all pins to output)
void initMCP() {
  setMCPDirection(MCP23017_ADDR1, 0x00);  // Set all pins of MCP23017_1 as output
  setMCPDirection(MCP23017_ADDR2, 0x00);  // Set all pins of MCP23017_2 as output
}

// Set MCP23017 IODIR registers to configure pin directions
void setMCPDirection(byte address, byte direction) {
  Wire.beginTransmission(address);
  Wire.write(0x00); // Select IODIRA register
  Wire.write(direction); // Set all pins to output
  Wire.endTransmission();
  
  Wire.beginTransmission(address);
  Wire.write(0x01); // Select IODIRB register
  Wire.write(direction); // Set all pins to output
  Wire.endTransmission();
}

// Process incoming serial commands (support multiple channels)
void processSerialCommand(String data) {
  if (data.substring(0, 5) == "*IDN?") {
    Serial.println("cell_ovd_control_v0.1");
  } else if (data.startsWith("on:") || data.startsWith("off:")) {
    String channels = data.substring(3);
    bool turnOn = data.startsWith("on");
    handleChannelControl(channels, turnOn);
  }
}

// Handle on/off for multiple channels
void handleChannelControl(String channels, bool turnOn) {
  // Split the channels string by commas and process each channel
  int start = 0;
  int index = channels.indexOf(',');
  while (index != -1) {
    int channel = channels.substring(start, index).toInt();
    controlSingleChannel(channel, turnOn);
    start = index + 1;
    index = channels.indexOf(',', start);
  }
  
  // Handle the last (or only) channel
  int channel = channels.substring(start).toInt();
  controlSingleChannel(channel, turnOn);
}

// Control a single channel (open or close)
void controlSingleChannel(int channel, bool turnOn) {
  if (channel < 23) {
    Serial.print("Control "); Serial.print(channel); Serial.print(" command received: "); Serial.println(turnOn);
    updateMCPOutput(channel, turnOn); // Pass on/off state
  } else if (channel == 35) {
    closeAllChannels(); // Special handling for channel 35 (close all)
  }
}

// Update the MCP outputs based on channel selection
void updateMCPOutput(int channel, bool turnOn) {
  int mcpAddr = (channel < 17) ? MCP23017_ADDR1 : MCP23017_ADDR2;

  // Select the correct index in the state array
  int index = (channel < 9) ? 0 : (channel < 17) ? 1 : (channel < 23) ? 2 : 3;
  int bitPos = channel % 8;

  // Update the state for the selected channel
  bitWrite(mcpState[index], bitPos, turnOn);

  // Write the updated state to the MCP23017
  Wire.beginTransmission(mcpAddr);
  Wire.write((index % 2 == 0) ? 0x12 : 0x13); // Select GPIO register
  Wire.write(mcpState[index]); // Send the updated state
  Wire.endTransmission();
}

// Control relays based on channel (on/off state)
void updateRelayState(int channel, bool state) {
  for (int i = 0; i < 6; i++) {
    digitalWrite(relayPins[i], state ? HIGH : LOW);
  }
}

// Close all channels (used for channel 35)
void closeAllChannels() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(relayPins[i], LOW); // Close all relays
  }
  // Reset the state of the MCP to close all channels
  for (int i = 0; i < 4; i++) {
    mcpState[i] = 0b00000000;
  }
  
  // Send reset state to MCP
  updateMCPOutput(0, false); // Close all channels on MCP23017
  updateMCPOutput(16, false); // Close all channels on second MCP23017
}
