/*
 *  This little sketch will help you obtain data about the Tactigon board's accelerometer
 *  It will show the accelleration (m/s2) value among the X-Y-Z axis.
 * 
 *  NOTE: the use of the "delay" function is not recommended. This is due to the fact that this specific function pauses not only the looped
 *        code but also the underlayer that manages the data processing for the sensors system (Sensor Fusion Algorithm).
 *        Instead keep track of the elapsed time and manage timing with the milli() function. Look at the code for it's use.
 *        For a visual rappresentation of the code execution please take a look at the slide Nr. 9-10-11 here:
 *        https://github.com/TactigonTeam/Docs/blob/master/Programming%20The%20Tactigon.pdf
 * 
 *  More informations at:
 *  https://www.thetactigon.com/arduino/doxygen/class_t___a_c_c.html
 */

#include "tactigon_IMU.h"

T_ACC accMeter;
T_AccData accData;

int ticks;

void setup()
{
    Serial.begin(9600);
    Serial.println("Serial port initialized!");
}

void loop()
{
    //Get acceleration data from the axis @ 50Hz (20ms)
    /*
    * Using this if statement allow to keep track of the current time and will allow the execution of the user code
    * only if a specified amount of ticks(1 ticks is equal to 1ms) have passed.
    * This allow the SFA underlayer to continue to run without interruptions.
    */

    if (millis() >= (ticks + 20))
    {
        ticks = millis();

        accData = accMeter.getAxis();

        //Now print the data
        Serial.print("X-axis accelleration (m/s2): ");
        Serial.println(accData.x);
        Serial.print("Y-axis accelleration (m/s2): ");
        Serial.println(accData.y);
        Serial.print("Z-axis accelleration (m/s2): ");
        Serial.println(accData.z);
        Serial.println();
    }
}

/*
*Little function that allow to have acceleration values in the G scale
*A little if statement has been added in order to avoid division by zero if the read value from the accelerometer is 0
*NOTE:This function is optional and NOT included in the default Tactigon libraries.
*/
float ms2tog(float input_val)
{
    if (input_val != 0)
    {
        return (input_val / 9.81);
    }
    else
    {
        return 0;
    }
}