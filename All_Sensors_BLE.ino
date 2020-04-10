/*
 * This is a final code that uses the majority of the Tactigon board sensors and BLE capabilities:
 * 
 * 1) get IMU data and expose them on differerent BLE characteristics
 * 
 * 2) add a characteristic with input GPIOs status
 * 
 * 3) write output GPIOs depending on x,y acceleration
 * 
 * NOTE for input GPIOs: pins are pulled up so open reading is HI. Connect to GND to se a LOW read
 * 
 * refer to https://github.com/TactigonTeam/Docs/blob/master/Tactigon%20ONE%20V1%20Pinout.pdf for TactigonV1 pinout
 * 
 */

#include <tactigon_led.h>
#include <tactigon_IMU.h>
#include <tactigon_BLE.h>
#include <tactigon_IO.h>

#define G_MSEC2 9.8

T_Led rLed, bLed, gLed;

T_ACC accMeter;
T_AccData accData;

T_GYRO gyroMeter;
T_GyroData gyroData;

T_QUAT qMeter;
T_QData qData;

T_BLE bleManager;
UUID uuid;
T_BLE_Characteristic accChar, gyroChar, eChar, qChar, ioChar;

T_GPP tGPP_1, tGPP_2, tGPP_3, tGPP_4; //GPIO classes: refer to documentation for GPIO mapping

uint8_t gBtnState_3, gBtnState_4;

/*----------------------------------------------------------------------*/
void setup()
{
  //init leds
  rLed.init(T_Led::RED);
  gLed.init(T_Led::GREEN);
  bLed.init(T_Led::BLUE);
  rLed.off();
  gLed.off();
  bLed.off();

  //init GPIO class
  tGPP_1.init(T_GPP::GPP1, T_GPP::GPP_OUT); //config GPIO1 as output
  tGPP_2.init(T_GPP::GPP2, T_GPP::GPP_OUT); //config GPIO2 as output
  tGPP_3.init(T_GPP::GPP3, T_GPP::GPP_IN);  //config GPIO3 as input
  tGPP_4.init(T_GPP::GPP4, T_GPP::GPP_IN);  //config GPIO4 as input

  //init status of output GPIOs
  tGPP_1.write(0);
  tGPP_2.write(0);

  //init BLE name
  bleManager.setName("TACTI");

  //init BLE role
  bleManager.InitRole(TACTIGON_BLE_PERIPHERAL);

  //add needed characteristic passing as parameter the size (in bytes)
  //of the characteristic
  //Every characteristic is identified by a unique identifier UUID

  uuid.set("bea5760d-503d-4920-b000-101e7306b007"); //acc
  accChar = bleManager.addNewChar(uuid, 14);

  uuid.set("bea5760d-503d-4920-b000-101e7306b008"); //gyro
  gyroChar = bleManager.addNewChar(uuid, 14);

  uuid.set("bea5760d-503d-4920-b000-101e7306b009"); //eulero angles as float
  eChar = bleManager.addNewChar(uuid, 14);

  uuid.set("bea5760d-503d-4920-b000-101e7306b005"); //quaternions as float (used by Cube app)
  qChar = bleManager.addNewChar(uuid, 18);

  uuid.set("bea5760d-503d-4920-b000-101e7306b004"); //IOs char
  ioChar = bleManager.addNewChar(uuid, 4);

  //disable use of magnetometer in quat computing
  qMeter.enable();
  qMeter.useMag(0);
}

/*----------------------------------------------------------------------*/
void loop()
{
  static int ticks = 0;
  unsigned char buffData[24];

  //read input GPIO with debouncing (updating globals gBtnState_3 and gBtnState_4)
  readInputIOs();

  //update BLE characteristics @ 50Hz (20 millisec)
  if (millis() >= (ticks + 20))
  {
    ticks = millis();

    //get IMU data
    //refer to API page https://www.thetactigon.com/arduino/doxygen/annotated.html
    //for data format
    //
    accData = accMeter.getAxis();   //accelerations data
    gyroData = gyroMeter.getAxis(); //angular speed data
    qData = qMeter.getQs();         //quaternions and Euler Angles data

    //update acc characteristic (acc values are float):   timestamp-unused (2bytes) | acc.x (4bytes) | acc.y (4bytes) | acc.z (4bytes)
    memset(buffData, 0, sizeof(buffData));
    memcpy(&buffData[2], &accData.x, 4);
    memcpy(&buffData[6], &accData.y, 4);
    memcpy(&buffData[10], &accData.z, 4);
    accChar.update(buffData);

    //update gyro characteristic (gyro values are float):   timestamp-unused (2bytes) | gyro.x (4bytes) | gyro.y (4bytes) | gyro.z (4bytes)
    memset(buffData, 0, sizeof(buffData));
    memcpy(&buffData[2], &gyroData.x, 4);
    memcpy(&buffData[6], &gyroData.y, 4);
    memcpy(&buffData[10], &gyroData.z, 4);
    gyroChar.update(buffData);

    //update Euler characteristic (angles values are float):   timestamp-unused (2bytes) | roll (4bytes) | pitch (4bytes) | yaw (4bytes)
    memset(buffData, 0, sizeof(buffData));
    memcpy(&buffData[2], &qData.roll, 4);
    memcpy(&buffData[6], &qData.pitch, 4);
    memcpy(&buffData[10], &qData.yaw, 4);
    eChar.update(buffData);

    //update quaternions characteristic (quaternions values are float):   timestamp-unused (2bytes) | q0 (4bytes) | q1 (4bytes) | q2 (4bytes) | q3 (4bytes)
    memset(buffData, 0, sizeof(buffData));
    memcpy(&buffData[2], &qData.q0f, 4);
    memcpy(&buffData[6], &qData.q1f, 4);
    memcpy(&buffData[10], &qData.q2f, 4);
    memcpy(&buffData[14], &qData.q3f, 4);
    qChar.update(buffData);

    //update input IO characteristic (one byte per IO):   timestamp-unused (2bytes) | gpio3 (1bytes) | gpio4 (1bytes)
    memset(buffData, 0, sizeof(buffData));
    buffData[2] = gBtnState_3;
    buffData[3] = gBtnState_4;
    ioChar.update(buffData);

    //manage output GPIOs
    gpoHandling(accData.x, accData.y);
  }

  //manage leds depending on BLE pairing status
  ledHandling();
}

/*--------------------------------------------------------------
 * 
 * manage leds depending on BLE pairing status
 * 
 -------------------------------------------------------------*/
void ledHandling()
{
  int bleStat;

  bleStat = bleManager.getStatus();

  if (bleStat == 1)
  {
    //paired
    rLed.off();
    blinkLed(bLed, 300);
  }
  else
  {
    //not paired
    bLed.off();
    blinkLed(rLed, 500);
  }
}

/*--------------------------------------------------------------
 * 
 * set output GPIO1 depending on x acc
 * set output GPIO2 depending on y acc
 * 
 -------------------------------------------------------------*/
void gpoHandling(float ax, float ay)
{
  if (abs(ax) > G_MSEC2 / 2)
    tGPP_1.write(1);
  else
    tGPP_1.write(0);

  if (abs(ay) > G_MSEC2 / 2)
    tGPP_2.write(1);
  else
    tGPP_2.write(0);
}

/*--------------------------------------------------------------
 * 
 * read input IOs (3 and 4) with debouncing
 * 
 -------------------------------------------------------------*/
void readInputIOs(void)
{
  static uint32_t debounceGpp4 = 0;
  static uint32_t debounceGpp3 = 0;
  static int debounceTime = 300;
  static uint8_t btn3 = 0;
  static uint8_t btn4 = 0;

  //debounce on GPP4
  if (millis() >= debounceGpp4 + debounceTime)
  {
    debounceGpp4 = millis();
    btn4 = tGPP_4.read();
    Serial.println(btn4);
  }

  //debounce on GPP3
  if (millis() >= debounceGpp3 + debounceTime)
  {
    debounceGpp3 = millis();
    btn3 = tGPP_3.read();
  }

  //update globals
  gBtnState_3 = btn3;
  gBtnState_4 = btn4;
}

/*--------------------------------------------------------------
 * 
 * blink led
 * 
 -------------------------------------------------------------*/
void blinkLed(T_Led blinkingLed, int period_msec)
{
  static unsigned int ticksLed = 0;
  static int stp = 0;

  if (millis() >= (ticksLed + period_msec))
  {
    ticksLed = millis();

    //blink blinkingLed
    if (stp == 0)
      blinkingLed.on();
    else if (stp == 1)
      blinkingLed.off();

    stp = (stp + 1) % 2;
  }
}
