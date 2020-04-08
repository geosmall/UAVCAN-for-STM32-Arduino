#include "Arduino.h"
#include "stm32yyxx_ll.h"
#include "app_comms.h"

#define FW_MAJOR 1
#define FW_MINOR 0
#define APJ_BOARD_ID 1000

#define HAL_RAM0_START 0x20000000
#define HAL_RAM_RESERVE_START 0x00000100

/* Private variables ---------------------------------------------------------*/
const struct app_descriptor app_descriptor __attribute__((section(".app_descriptor"))) = {
  { 0x40, 0xa2, 0xe4, 0xf1, 0x64, 0x68, 0x91, 0x06 },   // uint8_t sig[8]
  0x445e3f0e,     //  uint32_t image_crc1 is the crc32 from firmware start to start of image_crc1
  0xc107d765,     //  uint32_t image_crc2 is the crc32 from the start of version_major to the end of the firmware
  0x7268,         //  uint32_t image_size total size of firmware image in bytes
  0x11111111,     //  uint32_t git hash
  FW_MAJOR,       //  uint8_t  software version MAJOR
  FW_MINOR,       //  uint8_t  software version MINOR
  APJ_BOARD_ID,   //  uint16_t board_id = APJ_BOARD_ID
  { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },   // uint8_t reserved[8]
};

const int PwmOutputPin = PA0;         // PA_0,D46/A0 -- USES TIM2
const int TestOutputPin = PA6;

unsigned long previousMillis = 0;     // stores last time output was updated
const long interval = 200;            // interval at which to print output (milliseconds)


typedef struct
{
  __IO uint32_t KR;   /*!< IWDG Key register,       Address offset: 0x00 */
  __IO uint32_t PR;   /*!< IWDG Prescaler register, Address offset: 0x04 */
  __IO uint32_t RLR;  /*!< IWDG Reload register,    Address offset: 0x08 */
  __IO uint32_t SR;   /*!< IWDG Status register,    Address offset: 0x0C */
  __IO uint32_t WINR; /*!< IWDG Window register,    Address offset: 0x10 */
} IWDG_Regs;

#define IWDGD (*(IWDG_Regs *)(IWDG_BASE))

static bool watchdog_enabled;

/*
  setup the watchdog
 */
//void stm32_watchdog_init(void)
//{
    // setup for 2s reset
    // IWDGD.KR = 0x5555;
    // IWDGD.PR = 2; // div16
    // IWDGD.RLR = 0xFFF;
    // IWDGD.KR = 0xCCCC;
    // watchdog_enabled = true;
//}

/*
  pat the dog, to prevent a reset. If not called for 1s
  after stm32_watchdog_init() then MCU will reset
 */
void stm32_watchdog_pat(void)
{
  IWDGD.KR = 0xAAAA;
}


void setup() {

  struct app_bootloader_comms *comms = (struct app_bootloader_comms *)HAL_RAM0_START;
  memset(comms, 0, sizeof(struct app_bootloader_comms));
  comms->magic = APP_BOOTLOADER_COMMS_MAGIC;

  __enable_irq();

  printInitMsg();
  println();

  // initialize digital pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PwmOutputPin, OUTPUT);
  pinMode(TestOutputPin, OUTPUT);

  CAN_HW_Init();

  uavcanInit();

  //stm32_watchdog_init();
  stm32_watchdog_pat();

  digitalWrite(LED_BUILTIN, LOW);
//  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

}

void loop() {

  unsigned long currentMillis;

  currentMillis = millis();

  stm32_watchdog_pat();

  sendCanard();
  receiveCanard();
  spinCanard();
  publishCanard();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    showRcpwmonUart();
  }

}
