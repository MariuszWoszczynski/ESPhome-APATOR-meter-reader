#include "esphome.h"
#include "Arduino.h"
#include "ArduinoJson.h"
#include "vector"
#include "crc.hpp"
#include "mbus_packet.hpp"
#include "wmbus_utils.h"
#include "utils.hpp"
#include "aes.h"
#include "3outof6.hpp"
#include "util.h"
#include "mbus_defs.hpp"
#include "tmode_rf_settings.hpp"
#include "rf_mbus.hpp"


uint8_t MBpacket[291];
int rssi = 0;

//****************************CC1101 config************************************
uint8_t mosi = 13; 
uint8_t miso = 12;
uint8_t clk = 14;
uint8_t cs = 2;
uint8_t gdo0 = 5;
uint8_t gdo2 = 4;

//*****************************************************************************

//**********************determine the ID of your water meter (from a sticker)******************
int ApatorID = 0x1111111;    		//put your Id instead 1111111	 					
//*****************************************************************************

namespace MyTextData {
      TextSensor *my_text_sensor = new TextSensor();
    }
    
class MyTextSensor :public Component, public TextSensor {
      public:
        TextSensor *my_text_sensor = MyTextData::my_text_sensor;
    };
  
class MySensor :public Component, public Sensor {
  public: 
    Sensor *my_sensor_state = new Sensor();
  protected: HighFrequencyLoopRequester high_freq_;
 

void setup() {
  this->high_freq_.start();
  memset(MBpacket, 0, sizeof(MBpacket));
  rf_mbus_init(mosi, miso, clk, cs, gdo0, gdo2);
}



void loop() {

  if (rf_mbus_task(MBpacket, rssi, gdo0, gdo2)) {

    uint8_t lenWithoutCrc = crcRemove(MBpacket, packetSize(MBpacket[0]));    
    std::vector<uchar> frame(MBpacket, MBpacket+lenWithoutCrc);
    std::string sf = bin2hex(frame);

    char dll_id[9];
    sprintf(dll_id + 0, "%02X", frame[7]);
    sprintf(dll_id + 2, "%02X", frame[6]);
    sprintf(dll_id + 4, "%02X", frame[5]);
    sprintf(dll_id + 6, "%02X", frame[4]);
    
    int MeterID = strtol(dll_id, NULL, 16);
    char ID_text[32];
    itoa(MeterID, ID_text, 16);
    
    ESP_LOGI("Info", "Package received");
    //ESP_LOGI("Info", "Meter ID (DEC) = %d", MeterID);
    ESP_LOGI("Info", "Meter ID (HEX) = %X", MeterID);
    ESP_LOGI("Info", "Signal strenght: %d", rssi);
    
    
    if (MeterID == ApatorID) {
         
        std::vector<uchar> key;
        key.assign(16,0);
        std::vector<uchar>::iterator pos;
        pos = frame.begin();
        uchar iv[16];
        int i=0;
        for (int j=0; j<8; ++j) { iv[i++] = frame[2+j]; }
        for (int j=0; j<8; ++j) { iv[i++] = frame[11]; }
        pos = frame.begin() + 15;
        int num_encrypted_bytes = 0;
        int num_not_encrypted_at_end = 0;
        
        if (decrypt_TPL_AES_CBC_IV(frame, pos, key, iv, &num_encrypted_bytes, &num_not_encrypted_at_end)) {
          std::vector<uchar>::iterator fv;
          fv = std::find(pos, frame.end(), 0x10);
          if (fv != frame.end()){
            int v_temp;
            memcpy(&v_temp, &fv[1], 4);
            if ((v_temp > 0) and (v_temp < 10000000)) {     //data filter
              ESP_LOGI("Info", "Meter state: %d L", v_temp);
              my_sensor_state->publish_state(v_temp);
              MyTextData::my_text_sensor->publish_state(ID_text);
            }
          }
        }
    } else {
        ESP_LOGI("Info", "Incompatible ID\n");
    }
    memset(MBpacket, 0, sizeof(MBpacket));
  }
}
};




