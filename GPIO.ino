/**
 *  This little sketch will help you set up and read the state of a button connected to one of the GPIO port (active LOW).
 *  You can cycle trough the leds each time you press the button.
 * 
 *  NOTE: the use of the "delay" function is not recommended. This is due to the fact that this specific function pauses not only the looped
 *        code but also the underlayer that manages the data processing for the sensors system (Sensor Fusion Algorithm).
 *        Instead keep track of the elapsed time and manage timing with the milli() function. Look at the code for it's use.
 *        For a visual rappresentation of the code execution please take a look at the slide Nr. 9-10-11 here:
 *        https://github.com/TactigonTeam/Docs/blob/master/Programming%20The%20Tactigon.pdf
 * 
 *  More informations at:
 *  LED Reference
 *  https://www.thetactigon.com/arduino/doxygen/class_t___led.html
 *  GPIO Reference
 *  https://www.thetactigon.com/arduino/doxygen/class_t___g_p_p.html
 */

#include "tactigon_IO.h"
#include "tactigon_led.h"

T_Led rLed, gLed, bLed;
T_GPP gpp1, gpp2, gpp3, gpp4;

//Miscellaneous variables
int PrevbuttonStatus = HIGH;
int buttonStatus;
int count = 0;

void setup(){
    //Initialize each leds class with corresponding color
    rLed.init(T_Led::RED);
    gLed.init(T_Led::GREEN);
    bLed.init(T_Led::BLUE);

    //Turn off all leds
    rLed.on();
    gLed.on();
    bLed.on();

    //Initialize gpio pins as inputs or outputs
    gpp1.init(T_GPP::GPP1, T_GPP::GPP_OUT);
    gpp2.init(T_GPP::GPP2, T_GPP::GPP_OUT);
    gpp3.init(T_GPP::GPP3, T_GPP::GPP_IN);
    gpp4.init(T_GPP::GPP4, T_GPP::GPP_IN);

    Serial.begin(9600);
    Serial.println("Serial port initialized!");
}

void loop(){
    //Retrive the state of the pin
    buttonStatus = gpp4.read();

    //Check for debounce and increment counter to cycle color on next press
    //When the button is pressed increment the counter, it will be later used to determin which led to turn on
    if (PrevbuttonStatus == HIGH && buttonStatus == LOW)
    {
        Serial.println("PRESSED!");
        if(count < 3){
            count++;
        }
        else{
            count = 0;
        }
    }

    //When the button is released proceedwith the cycle of color, turning on or off leds according to counter value
    else if (PrevbuttonStatus == LOW && buttonStatus == HIGH)
    {
        Serial.println("RELEASED");

        switch (count)
        {
        case 0:
            rLed.off();
            gLed.off();
            bLed.off();
            break;
        case 1:
            rLed.on();
            gLed.off();
            bLed.off();
            break;
        case 2:
            rLed.off();
            gLed.on();
            bLed.off();
            break;
        case 3:
            rLed.off();
            gLed.off();
            bLed.on();
            break;
        default:
            break;
        }
    }

    //Update button state "tracking" variable
    PrevbuttonStatus = buttonStatus;
}