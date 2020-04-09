/**
 *  This little sketch will help you obtain data about the environment
 *  It will show temperature (°C) and pressure (mbar).
 * 
 *  NOTE: the use of the "delay" function is not recommended. This is due to the fact that this specific function pauses not only the looped
 *        code but also the underlayer that manages the data processing for the sensors system (Sensor Fusion Algorithm).
 *        Instead keep track of the elapsed time and manage timing with the milli() function. Look at the code for it's use.
 *        For a visual rappresentation of the code execution please take a look at the slide Nr. 9-10-11 here:
 *        https://github.com/TactigonTeam/Docs/blob/master/Programming%20The%20Tactigon.pdf
 *
 *  More informations at:
 *  https://www.thetactigon.com/arduino/doxygen/class_t___env_sens.html
 */

#include "tactigon_Env.h"

T_EnvSens envMeter;
T_EnvData envData;

int ticks;

void setup()
{
    Serial.begin(9600);
    Serial.println("Serial port initialized!");
}

void loop()
{
    //Get environment data from sensors @ 2Hz (500ms)
    /*
    * Using this if statement allow to keep track of the current time and will allow the execution of the user code
    * only if a specified amount of ticks(1 ticks is equal to 1ms) have passed.
    * This allow the SFA underlayer to continue to run without interruptions.
    */
    if (millis() >= (ticks + 500))
    {
        ticks = millis();
        //Get environment data from sensors
        envData = envMeter.getData();

        //Now print the data
        Serial.print("Temperature (°C): ");
        Serial.println(envData.temp);
        Serial.print("Pressure (mbar): ");
        Serial.println(envData.press);
        Serial.println();
    }
}