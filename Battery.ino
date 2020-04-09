/*
 *  This little sketch will help you obtain data about the battery powering the Tactigon board
 *  It will show charging percentage, current (mA) and voltage (V).
 * 
 *  NOTE: the use of the "delay" function is not recommended. This is due to the fact that this specific function pauses not only the looped
 *        code but also the underlayer that manages the data processing for the sensors system (Sensor Fusion Algorithm).
 *        Instead keep track of the elapsed time and manage timing with the milli() function. Look at the code for it's use.
 *        For a visual rappresentation of the code execution please take a look at the slide Nr. 9-10-11 here:
 *        https://github.com/TactigonTeam/Docs/blob/master/Programming%20The%20Tactigon.pdf
 * 
 *  More informations at:
 *  https://www.thetactigon.com/arduino/doxygen/class_t___battery.html
 */

#include "tactigon_Battery.h"

T_Battery battMeter;
T_BattData battData;

int ticks;

void setup()
{
    Serial.begin(9600);
    Serial.println("Serial port initialized!");
}

void loop()
{
    //Get battery data from the sensors @ 2Hz (500ms)
    /*
    * Using this if statement allow to keep track of the current time and will allow the execution of the user code
    * only if a specified amount of ticks(1 ticks is equal to 1ms) have passed.
    * This allow the SFA underlayer to continue to run without interruptions.
    */
    if (millis() >= (ticks + 500))
    {
        ticks = millis();
        battData = battMeter.getData();

        //Now print the data
        Serial.print("State of charge: ");
        Serial.println(battData.soc);
        Serial.print("Charging current (mA): ");
        Serial.println(battData.i_val);
        Serial.print("Charging voltage (V): ");
        Serial.println(battData.v_val);
        Serial.println();
    }
}