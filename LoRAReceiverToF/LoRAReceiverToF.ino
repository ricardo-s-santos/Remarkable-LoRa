#include <SPI.h>
#include <LoRa.h>
#include <hal/cpu_hal.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  //replace the LoRa.begin(---E-) argument with your location's frequency
  //433E6 for Asia
  //868E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(868E6)) {
    Serial.println(".");
    delay(500);
  }
  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  // try to parse packet
  LoRa.setSyncWord(0xF3);
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //uint32_t start_cycles = cpu_hal_get_cycle_count();
    LoRa.setSyncWord(0xF2);
    //Send LoRa packet to transmitter again
    LoRa.beginPacket();
    LoRa.print("helloTx");
    LoRa.endPacket();
    /*
    uint32_t end_cycles = cpu_hal_get_cycle_count();
    uint32_t elapsed_cycles = end_cycles - start_cycles;
    uint64_t TOA = (uint64_t)elapsed_cycles * 1000000000ULL / (240000000ULL);
    Serial.println();
    Serial.print("TOA: ");
    Serial.print(TOA);
    Serial.print(" nanoseconds");
    */
  }
}