/**
 *  This little sketch will help you obtain Euler's angle and quaternion data from the Tactigon board
 *  It will show the roll (rad), pitch (rad), yaw (rad) and W-X-Y-Z quaternions components both compressed and uncompressed.
 * 
 *  NOTE: the use of the "delay" function is not recommended. This is due to the fact that this specific function pauses not only the looped
 *        code but also the underlayer that manages the data processing for the sensors system (Sensor Fusion Algorithm).
 *        Instead keep track of the elapsed time and manage timing with the milli() function. Look at the code for it's use.
 *        For a visual rappresentation of the code execution please take a look at the slide Nr. 9-10-11 here:
 *        https://github.com/TactigonTeam/Docs/blob/master/Programming%20The%20Tactigon.pdf
 *
 *  More informations at:
 *  https://www.thetactigon.com/arduino/doxygen/class_t___q_u_a_t.html
 */

#include "tactigon_IMU.h"

T_QUAT quatMeter;
T_QData quatData;


void setup()
{
    //Enable(1)/disable(0) use of magnetometer in quaternions computing
    quatMeter.useMag(0);

    //Enable quaternions computing
    quatMeter.enable();

    Serial.begin(9600);
    Serial.println("Serial port initialized!");
}

void loop()
{
    static int ticks = 0;

    //Get rotations and quaternions data from the axis @ 50Hz (20ms)
    /*
    * Using this if statement allow to keep track of the current time and will allow the execution of the user code
    * only if a specified amount of ticks(1 ticks is equal to 1ms) have passed.
    * This allow the SFA underlayer to continue to run without interruptions.
    */
    if (millis() >= (ticks + 20))
    {
        ticks = millis();

        quatData = quatMeter.getQs();

        //Now print the data
        Serial.print("Roll (rad): ");
        Serial.println(quatData.roll);
        Serial.print("Pitch (rad): ");
        Serial.println(quatData.pitch);
        Serial.print("Yaw (rad): ");
        Serial.println(quatData.yaw);
        Serial.print("W quaternions component: ");
        Serial.println(quatData.q0f);
        Serial.print("X quaternions component: ");
        Serial.println(quatData.q1f);
        Serial.print("Y quaternions component: ");
        Serial.println(quatData.q2f);
        Serial.print("Z quaternions component: ");
        Serial.println(quatData.q3f);
        Serial.println();
    }
}

//convert [rad] to [deg]
float radToDeg(float rad)
{
  return rad*180/PI;
}

/*
 * remap roll in order to have 0 deg in horizontal position
 * and +180,-180 periodicity around this zero position
 */
float rollZeroRemap(float r)
{
  if( (r > -180) && (r < 90) )
    r = r + 90;
  else
    r = -270 + r;

  return r;
}
