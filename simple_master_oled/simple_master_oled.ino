/**
 *  Modbus master example 1:
 *  The purpose of this example is to query an array of data
 *  from an external Modbus slave device. 
 *  The link media can be USB or RS232.
 *
 *  Recommended Modbus slave: 
 *  diagslave http://www.modbusdriver.com/diagslave.html
 *
 *  In a Linux box, run 
 *  "./diagslave /dev/ttyUSB0 -b 19200 -d 8 -s 1 -p none -m rtu -a 1"
 * 	This is:
 * 		serial port /dev/ttyUSB0 at 19200 baud 8N1
 *		RTU mode and address @1
 */

#include <ModbusRtu.h>
#include <Wire.h>
#include <U8g2lib.h> 

#define OLED_SDA  4     //D4
#define OLED_SCL 5     //D3
#define OLED_RST  2      //D1

uint16_t au16data[16];
uint8_t u8state;

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA , OLED_RST);
const char *text = "Codvid-19 Temperature";  // scroll this text from right to left

Modbus master(0,Serial,0); // this is master and RS-232 or USB-FTDI

modbus_t telegram;

unsigned long u32wait;

void setup() {
  Serial.begin( 19200 ); // baud-rate at 19200
  master.start();
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u32wait = millis() + 1000;
  u8state = 0; 
   //start oled
      u8g2.begin();
      u8g2.clearBuffer();          // clear the internal memory
      //u8g2.setFont(u8g2_font_16x13B_mf); // choose a suitable font
      u8g2.setFont(u8g2_font_8x13B_mf); // choose a suitable font
      u8g2.drawStr(0,10,text);  // write something to the internal memory
      u8g2.sendBuffer();          // transfer internal memory to the display
      //end oled
}

void loop() {
  switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
    telegram.u8id = 1; // slave address
    telegram.u8fct = 3; // function code (this one is registers read)
    telegram.u16RegAdd = 1; // start address in slave
    telegram.u16CoilsNo = 4; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino

    master.query( telegram ); // send query (only once)
    u8state++;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() + 1000; 
    }
    break;
  }
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_8x13B_mf); // choose a suitable font
  //u8g2.setFont(u8g2_font_10x20_mn);  // font size 16 pixel high
  //u8g2.setFont(u8g2_font_inr30_mn);
  u8g2.setCursor(0,10);
  u8g2.print(au16data[0]);
  u8g2.setCursor(40,10);
  u8g2.print(au16data[1]);
  u8g2.sendBuffer();          // transfer internal memory to the display

}
