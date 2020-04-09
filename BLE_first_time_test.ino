/**
 *  This little sketch will help you transmit data over Bluetooth Low Energy using UUID and Characteristics.
 *  In this specific case the Tactigon is configured as Peripheral Device (Notify).
 *  More specifically the Tactigon will be configured as a "beacon" that broadcast its identifier and data to nearby devices.
 * 
 *  NOTE: the use of the "delay" function is not recommended. This is due to the fact that this specific function pauses not only the looped
 *        code but also the underlayer that manages the data processing for the sensors system (Sensor Fusion Algorithm).
 *        Instead keep track of the elapsed time and manage timing with the milli() function. Look at the code for it's use.
 *        For a visual rappresentation of the code execution please take a look at the slide Nr. 9-10-11 here:
 *        https://github.com/TactigonTeam/Docs/blob/master/Programming%20The%20Tactigon.pdf
 * 
 *  More informations at:
 *  https://www.thetactigon.com/arduino/doxygen/class_t___b_l_e.html
 */

#include "tactigon_BLE.h"
#include "tactigon_IMU.h"

T_BLE bleManager;
UUID uuid;
T_BLE_Characteristic bleChar;

//Variou variable to keep track of the time elapsed, a flag to toggle the data and the buffer in which the data will be stored
int ticks;
bool flag = true;
unsigned char buffData[14];

void setup(){
    //Initialize the Tactigon as a Peripheral Device
    bleManager.InitRole(TACTIGON_BLE_PERIPHERAL);

    //We set a name so that we can recognize our board later on more easily
    bleManager.setName("BLE");

    //Set up the UUID for our custom characteristic
    uuid.set("7ac71000-503d-4920-b000-acc000000001");

    //Add the newely created characteristic to the device mentioning the UUID and thew length of the data buffer that we're going to use
    bleChar = bleManager.addNewChar(uuid, 14);

    Serial.begin(9600);
    Serial.println("Serial port initialized!");
}

void loop(){
    //This if statement keep track of the ms elapsed so that we update the data that are broadcasted @ 1Hz (1000ms)
    /*
    * Using this if statement allow to keep track of the current time and will allow the execution of the user code
    * only if a specified amount of ticks(1 ticks is equal to 1ms) have passed.
    * This allow the SFA underlayer to continue to run without interruptions.
    */
    if(millis() >= (ticks + 1000)){

        ticks = millis();

        //Crate a dummy buffer that will be broadcasted
        create_buffer(flag);

        //Change the variable so that next time the data will be different
        flag = !flag;

        //Update the characteristic with the newely created bufferData
        bleChar.update(buffData);
    }
}

//Simple function that will populate the data buffer based on a flag based argument
void create_buffer(bool select){
    short data;

    //The following will "chose" the data
    if (select == true){
        data = 0xAA;
    }
    else{
        data = 0xFF;
    }

    //Here we can write the data in the data buffer that will be later sent
    //This is the base structure that will be used for future BLE enabled examples, such as Accelerometer and Gyroscope data broadcast
    memcpy(&buffData[0], &data, 2);
    memcpy(&buffData[2], &data, 2);
    memcpy(&buffData[4], &data, 2);
    memcpy(&buffData[6], &data, 2);

    memcpy(&buffData[8], &data, 2);
    memcpy(&buffData[10], &data, 2);
    memcpy(&buffData[12], &data, 2);
}