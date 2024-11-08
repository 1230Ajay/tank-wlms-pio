#include "WifiService.h"
#include <MySocketService.h>
#include "helper.h"

// Create a SocketService instance
SocketService socketService;
WiFiService wiFiService;
Helper helper;

bool internet_status = false;
int battery_percentage = 0;
int tank_level = 0;
// Push data to server timing
unsigned long device_update_time = millis();
const unsigned long device_update_duration = 1000 * 60; // 1 minute
int prevLevel = 0;
bool sensor_status = false;

// Restart device timing
unsigned long device_restart_time = 0;                       // Initialized to 0
const unsigned long device_restart_duration = 1000 * 60 * 2; // 2 minutes

void setup()
{

  // Pin definitions
  pinMode(BATTERY, INPUT);
  pinMode(MODEM_RELAY, OUTPUT);
  pinMode(INTERNET_LED, OUTPUT);
  pinMode(SENSOR_STATUS,OUTPUT);

  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);


  // Initial pin states
  digitalWrite(MODEM_RELAY, LOW);
  digitalWrite(INTERNET_LED, LOW);
  digitalWrite(SENSOR_STATUS,LOW);

  Serial.begin(115200);
  helper.printOnLCD();
  // Connect to Wi-Fi and then to Socket.IO
  
  helper.updateLCD(battery_percentage,sensor_status,tank_level,wiFiService.isConnected());
  wiFiService.begin();
  socketService.connect(HOST_ID, HOST_PORT);

}

void loop()
{
  unsigned long current_time = millis();
  bool wifi_status = wiFiService.isConnected();
  // Check if Wi-Fi is connected
  if (!wifi_status)
  {
    internet_status = false;
    Serial.println("Connecting to Wi-Fi...");
  }
  else
  {
    internet_status = true;
  }

  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  int s3 = digitalRead(S3);

  bool sensor_current_status = helper.sensorStatus(s1, s2, s3);
  int currentLevel = helper.tankLevel(s1, s2, s3);
  bool socket_connection = socketService.isConnected();

  digitalWrite(SENSOR_STATUS, sensor_current_status);
  digitalWrite(INTERNET_LED, socket_connection);


  helper.updateLCD(battery_percentage,sensor_status,currentLevel,wifi_status);

  // Check if the socket is connected
  if (socket_connection)
  {
    // Reset device_restart_time if the socket is connected
    device_restart_time = 0;
    // Indicate connection with LED

    // Read sensor data and battery

    battery_percentage = helper.batteryPercentage();
    // Update LCD display

    delay(500);
    if (currentLevel != prevLevel || sensor_current_status != sensor_status)
    {
      sensor_status = sensor_current_status;
      tank_level = currentLevel;
      prevLevel = currentLevel;
      socketService.send_log(battery_percentage, prevLevel, sensor_status);
      Serial.println(prevLevel);
    }
  }
  else
  {
    // If the socket is disconnected, start or continue the restart timer
    if (device_restart_time == 0)
    {
      device_restart_time = current_time; // Set the restart time on first disconnect
    }

    // Handle device restart if the time exceeds the restart duration
    if (current_time - device_restart_time > device_restart_duration)
    {
      helper.restartDevice(MODEM_RELAY);
      device_restart_time = 0; // Reset the restart time after restarting
    }
  }

  // Update the SocketService loop to handle socket events
  socketService.loop();
}
