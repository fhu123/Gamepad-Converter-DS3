#include <WiFi.h>
#include "USB.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"
#include "USBHIDConsumerControl.h"
#include "USBHIDSystemControl.h"
#include "USBHIDVendor.h"
#include "FirmwareMSC.h"
#include "USBHIDGamepad_Custom.h"
#include "esp_wifi.h" // Include for esp_wifi_set_mac
#include "esp_bt.h"   // Include for Bluetooth functions

// USBHID instances
USBHID HID;
USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;
USBHIDGamepad_Custom Gamepad;
USBHIDConsumerControl ConsumerControl;
USBHIDSystemControl SystemControl;
USBHIDVendor Vendor;

// Define the MAC address of the DS3 controller
const uint8_t ds3ControllerMAC[] = {0x78, 0xDD, 0x08, 0x6E, 0xA5, 0xCC};

// WiFi settings
const char* ssid = "FHUHOME";            // Your WiFi network name (SSID)
const char* password = "Umts2017Umts2017"; // Your WiFi password

const char* hostname = "gameconverterbox";
const char* serverUrl = "http://192.168.178.35:5000"; // Replace with your Raspberry Pi server address

// Set MAC address
// esp_wifi_set_mac(WIFI_IF_STA, ds3ControllerMAC); // Not needed for DHCP, comment out if not required

void setup() {
  Serial.begin(115200);
  delay(1000); // Give time for Serial to initialize

  // Set the hostname
  WiFi.setHostname(hostname);

  // Connect to WiFi using DHCP with WPA-CCMP security
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  // Wait for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;
    if (attempts > 20) { // Timeout after 10 seconds
      Serial.println("\nFailed to connect to WiFi. Check credentials.");
      return;
    }
  }

  Serial.println();
  Serial.println("Connected to WiFi");

  // Print the assigned IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set MAC address (if needed)
  // esp_wifi_set_mac(WIFI_IF_STA, ds3ControllerMAC);

  // Initialize USB
  USB.begin();

  // Initialize USB HID
  HID.begin();
  Keyboard.begin();
  Mouse.begin();
  Gamepad.begin();
  ConsumerControl.begin();
  SystemControl.begin();
  Vendor.begin();
}

void loop() {
  // Check for USB HID data
  if (HID.ready()) {
    // Read USB HID data and forward it over Bluetooth
    // Modify this part based on your HID usage
    // Example: Read joystick data and send it over Bluetooth
    // Example: Read button states and send them over Bluetooth
    // Example: Read any other HID data and send it over Bluetooth
  }

  // Check for Bluetooth data
  //while (SerialBT.available()) {
    // Read Bluetooth data and process it
    // Example: Receive commands from Bluetooth and perform actions
    //char incomingByte = SerialBT.read();
    // Example: If incomingByte is 'A', perform action A
    // Example: If incomingByte is 'B', perform action B
    // Modify this part based on your Bluetooth communication protocol
  //}

  // Other tasks
}
