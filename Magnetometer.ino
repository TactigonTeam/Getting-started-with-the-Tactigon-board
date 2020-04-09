/**
 *  This little sketch will help you obtain data about the Tactigon board's magnetometer
 *  It will show the magnetic field (Gauss) value among the X-Y-Z axis.
 * 
 *  NOTE: the use of the "delay" function is not recommended. This is due to the fact that this specific function pauses not only the looped
 *        code but also the underlayer that manages the data processing for the sensors system (Sensor Fusion Algorithm).
 *        Instead keep track of the elapsed time with the milli() function. Look at the code for it's use.
 *        For a visual rappresentation of the code execution please take a look at the slide Nr. 9-10-11 here:
 *        https://github.com/TactigonTeam/Docs/blob/master/Programming%20The%20Tactigon.pdf
 * 
 *  More informations at:
 *  https://www.thetactigon.com/arduino/doxygen/class_t___m_a_g.html
 */

#include "tactigon_IMU.h"

T_MAG magMeter;
T_MagData magData;

int ticks;

void setup()
{
    Serial.begin(9600);
    Serial.println("Serial port initialized!");
}

void loop()
{
    //Get magnetometer data from the axis @ 2Hz (500ms)
    /*
    * Using this if statement allow to keep track of the current time and will allow the execution of the user code
    * only if a specified amount of ticks(1 ticks is equal to 1ms) have passed.
    * This allow the SFA underlayer to continue to run without interruptions.
    */
    if (millis() >= (ticks + 500))
    {
        ticks = millis();
        magData = magMeter.getAxis();

        //Now print the data
        Serial.print("X-axis magnetic field (Gauss): ");
        Serial.println(magData.x);
        Serial.print("Y-axis magnetic field (Gauss): ");
        Serial.println(magData.y);
        Serial.print("Z-axis magnetic field (Gauss): ");
        Serial.println(magData.z);
        Serial.println();
    }
}