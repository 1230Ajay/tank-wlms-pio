#ifndef BATTERY_SERVICE_H
#define BATTERY_SERVICE_H
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <constants.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
// Include the pin definitions
class Helper
{

public:
    

    int batteryPercentage()
    {
        int batteryVoltage = (analogRead(BATTERY) * 3.6 * 5) / 4096.0;  // Convert analog reading to voltage
        int batteryPercentage = map(batteryVoltage, 9.4, 14.2, 0, 100); // Map voltage range to percentage
        // Clamp the percentage value between 0 and 100
        batteryPercentage = constrain(batteryPercentage, 0, 100);
        return batteryPercentage;
    }

    void restartDevice(int pin)
    {
        digitalWrite(pin, HIGH);
        delay(1000 * 30);
        digitalWrite(pin, LOW);
    }


    int tankLevel(int s1, int s2, int s3)
    {
        if (s1)
        {
            return 100; // If s1 is HIGH, return 100, and ignore s2 and s3
        }
        else if (s2)
        {
            return 30; // If s1 is LOW but s2 is HIGH, return 30, and ignore s3
        }
        else if (s3)
        {
            return 10; // If both s1 and s2 are LOW but s3 is HIGH, return 10
        }
        else
        {
            return 0; // If none are HIGH, return 0
        }
    }

    bool sensorStatus(int s1, int s2, int s3)
    {
        if (s1 && (!s2 || !s3))
        {
            return false; 
        }
        else if (s2 && !s3)
        {
            return false; 
        }
        else
        {
            return true; 
        }
    }


    
    void printOnLCD()
    {

        lcd.init();
        lcd.backlight();
        lcd.clear();

        // Center align text on initialization
        lcd.setCursor(1, 0);
        lcd.print("INDIAN RAILWAY");

        lcd.setCursor(0, 1);
        lcd.print("TANK WATER LEVEL");

        lcd.setCursor(-4, 2);
        lcd.print("MONITORING SYSTM");

        lcd.setCursor(-4, 3);
        lcd.print("ROBOKRITI INDIA");

        delay(1000 * 30);
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("Tank WLMS: ");
        lcd.print(LOCATION);

        lcd.setCursor(0, 1);
        lcd.print("Bat:   ");

        lcd.setCursor(-4, 2);
        lcd.print("Sensor Status:   ");
        
        lcd.setCursor(-4, 3);
        lcd.print("Tank Level :   ");
        lcd.setCursor(8, 3);

    }

    void updateLCD(int battery_percentage, bool sensor_status, int tank_level,bool internet_status)
    {
        lcd.setCursor(4, 1);
        lcd.print(battery_percentage);
        lcd.print("%  ");

        lcd.setCursor(9, 1);
        lcd.print("WiFi:");

        if (internet_status)
        {
            lcd.print("OK");
        }
        {
            lcd.print("ER");
        }

        lcd.setCursor(10, 2);

        if (sensor_status)
        {
            lcd.print("OK");
        }
        else
        {
            lcd.print("ER");
        }

        lcd.setCursor(-4, 3);
        lcd.print("Tank Level :   ");
        lcd.setCursor(8, 3);

  
        lcd.print(tank_level);

        lcd.print("%  ");
    }

};

#endif