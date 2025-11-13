#include <SPI.h>
#include <LoRa.h>
#include <hal/cpu_hal.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

int counter = 0;

unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long TOA = 0;
unsigned long processingTimeReceiver = 31465820;  //Tempo de receber e enviar mensagem de volta do Receptor (ns)
unsigned long distance = 0;
unsigned long tempoProcessamentoTotal = 100072703 + processingTimeReceiver; // Instruções desde envio até receber + tempo do receptor

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("LoRa Sender");

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
  Serial.print("Sending packet: ");
  Serial.println(counter);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();
  uint32_t start_cycles = cpu_hal_get_cycle_count();
  //startTime = micros();  // Note the time when the message was sent

  LoRa.setSyncWord(0xF2);
  // Switch back to receive mode
  LoRa.receive();
  delay(100);
  counter++;
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    uint32_t end_cycles = cpu_hal_get_cycle_count();
    //endTime = micros();  // Note the time when the message was sent
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }
    // Calculate the TOA and print it to the serial monitor
    //TOA = endTime - startTime;
    uint32_t elapsed_cycles = end_cycles - start_cycles;
    uint64_t TOA = (uint64_t)elapsed_cycles * 1000000000ULL / (240000000ULL);
    Serial.println();
    Serial.print("TOA: ");
    Serial.print(TOA);
    Serial.print(" nanoseconds");
    Serial.println();
    Serial.println(TOA - tempoProcessamentoTotal);

    // Calculate the distance based on TOA and print it to the serial monitor
    // TOA - delay - tempoProcessamento do receptor - tempoProcessamento do emissor
    distance = ((TOA / 2.0)) * 3.33;  // ns / m
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" meters");
  }
  delay(1000);
  Serial.println("Passei");
}