//#include "stm32yyxx_ll.h"

#define RC_NUM_CHANNELS_TO_PRINT    4

extern "C" void printInitMsg(void) {
  Serial1.begin(115200);
  Serial1.print("Initializing...");
  Serial1.print(HAL_RCC_GetHCLKFreq());
  Serial1.println();
}

extern "C" void printArray(int16_t arr[]) {

  char str[20];
  itoa(arr[0], str, 10);
  Serial1.print("ESC Array: ");
  Serial1.print(str);
  for (int i = 1; i < RC_NUM_CHANNELS_TO_PRINT; i++) {
    itoa(arr[i], str, 10);
    Serial1.print(", ");
    Serial1.print(str);
  }
  Serial1.println();
}

extern "C" void println(char* str) {
  Serial1.print(str);
  Serial1.println();
}

extern "C" void printstr(char* str) {
  Serial1.print(str);
}

extern "C" void printint(uint32_t num) {
  Serial1.print(num, HEX);
}
