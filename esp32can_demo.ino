/*
 * ESP32 CAN-Bus demo
 * 
 * www.skpang.co.uk
 * 
 * v1.0 May 2018
 * 
 * For use with CAN-Bus FeatherWing 
 * http://skpang.co.uk/catalog/canbus-featherwing-for-esp32-p-1556.html
 * 
 * and ESP-32 Feather board
 * http://skpang.co.uk/catalog/adafruit-huzzah32-esp32-feather-board-p-1530.html
 * 
 * 
 */
#include <ESP32CAN.h>
#include <CAN_config.h>

CAN_device_t CAN_cfg;
int i = 0;
long d = 0;
CAN_frame_t rx_frame;

 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {

  rx_frame.FIR.B.FF = CAN_frame_std;
  rx_frame.MsgID = 0x101;
  rx_frame.FIR.B.DLC = 8;
  rx_frame.data.u8[0] = 'h';
  rx_frame.data.u8[1] = 'e';
  rx_frame.data.u8[2] = 'l';
  rx_frame.data.u8[3] = 'l';
  rx_frame.data.u8[4] = 'o';
  rx_frame.data.u8[5] = 'c';
  rx_frame.data.u8[6] = 'a';
  rx_frame.data.u8[7] = i++;
    
  ESP32Can.CANWriteFrame(&rx_frame);
 
}

void setup() {

    Serial.begin(115200);
    Serial.println("CAN-Bus demo");
    CAN_cfg.speed=CAN_SPEED_500KBPS;
    CAN_cfg.tx_pin_id = GPIO_NUM_25;
    CAN_cfg.rx_pin_id = GPIO_NUM_26;
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    //start CAN Module
    ESP32Can.CANInit();

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 1000000, true);
    timerAlarmEnable(timer);
     
}

void loop() {
 
  if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){

      //do stuff!
      if(rx_frame.FIR.B.FF==CAN_frame_std)
        printf("New standard frame");
      else
        printf("New extended frame");

      if(rx_frame.FIR.B.RTR==CAN_RTR)
        printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      else{
        printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
        for(int i = 0; i < 8; i++){
          printf("%c\t", (char)rx_frame.data.u8[i]);
        }
        printf("\n");
      }
   }
    
}
