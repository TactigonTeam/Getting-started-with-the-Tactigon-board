/**
 *  This little sketch will help you obtain data about the Tactigon board's gyroscope
 *  It will show the angular speed (rad/s) value among the X-Y-Z axis.
 * 
 *  NOTE: the use of the "delay" function is not recommended. This is due to the fact that this specific function pauses not only the looped
 *        code but also the underlayer that manages the data processing for the sensors system (Sensor Fusion Algorithm).
 *        Instead keep track of the elapsed time and manage timing with the milli() function. Look at the code for it's use.
 *        For a visual rappresentation of the code execution please take a look at the slide Nr. 9-10-11 here:
 *        https://github.com/TactigonTeam/Docs/blob/master/Programming%20The%20Tactigon.pdf
 * 
 *  More informations at:
 *  https://www.thetactigon.com/arduino/doxygen/class_t___g_y_r_o.html
 */

#include "tactigon_IMU.h"

T_GYRO gyroMeter;
T_GyroData gyroData;

int ticks;

void setup()
{
    //The following line can be used if the Gyroscope data returned seems wrong or not accurate
    //It runs in the background for about 10 seconds and will calibrate the gyro
    //gyroMeter.startCalib();

    Serial.begin(9600);
    Serial.println("Serial port initialized!");
}

void loop()
{
    //Get gyroscope data from the axis @ 50Hz (20ms)
    /*
    * Using this if statement allow to keep track of the current time and will allow the execution of the user code
    * only if a specified amount of ticks(1 ticks is equal to 1ms) have passed.
    * This allow the SFA underlayer to continue to run without interruptions.
    */
    if (millis() >= (ticks + 20))
    {
        ticks = millis();

        gyroData = gyroMeter.getAxis();

        //Now print the data
        Serial.print("X-axis angular speed (rad/s): ");
        Serial.println(gyroData.x);
        Serial.print("Y-axis angular speed (rad/s): ");
        Serial.println(gyroData.y);
        Serial.print("Z-axis angular speed (rad/s): ");
        Serial.println(gyroData.z);
        Serial.println();
    }
}