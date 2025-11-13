#include <SPI.h>
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

int calibratedRssi = -50;
const int pathLossExp = 4;  // Indoor
int countLoRAPackets = 0;

// RSSI variables
const int k = 10;
double medicoes[k];
float median = 0;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("LoRa Receiver");

  //Setup LoRa transceiver module
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
  // Tentar fazer parse do pacote
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Pacote recebido
    Serial.print("Received packet '");

    // Ler pacote
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }
    // Imprimir RSSI para o Serial Monitor
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    // Adicionar ao array de medicoes
    medicoes[countLoRAPackets] = LoRa.packetRssi();
    countLoRAPackets++;

    if (countLoRAPackets == k){
      median = calcularMediana(medicoes, k);
      Serial.println(median);
      // Fórmula RSSI -> Distancia
      double dist = (calibratedRssi - LoRa.packetRssi());
      dist = dist / (10. * pathLossExp);
      dist = pow(10, dist);
      Serial.print(dist);
      Serial.println(" (m)");
      // Reset median, array e counter
      median = 0;
      memset(medicoes, 0, sizeof medicoes);
      countLoRAPackets = 0;
    }
  }
}

// Função que calcula a mediana
double calcularMediana(double arr[], int n) {
    // Ordenar por ordem crescente
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Troca os elementos
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
    if (n % 2 == 1) {
        // Impar - elemento do meio
        return arr[n / 2];
    } else {
        // Par - média dos dois do meio
        return (arr[n / 2 - 1] + arr[n / 2]) / 2.0;
    }
}