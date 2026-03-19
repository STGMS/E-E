#include <SPI.h>
#include <RF24.h>

const int CE = 9;
const int CSN = 10;

RF24 radio(CE, CSN);
const uint64_t direccion_escritura = 0xE0E0F0F0E1LL;
const uint64_t direccion_lectura = 0xE0E0F0F0E2LL;

int valor = 0;
int LED = 6;

void setup() {
  Serial.begin(115200);
  delay(1500);
  
  Serial.println("\n========== RECEPTOR nRF24 ==========");
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  Serial.println("Iniciando nRF24...");
  
  if (!radio.begin()) {
    Serial.println("❌ Error: nRF24 no inicializó");
    while (1) delay(1000);
  }
  
  Serial.println("✓ nRF24 inicializó");
  
  // Configuración crítica (IGUAL al emisor)
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  
  // Abrir tubos
  radio.openWritingPipe(direccion_lectura);
  radio.openReadingPipe(1, direccion_escritura);
  
  radio.startListening();
  delay(100);
  
  Serial.println("✓ Configuración completada");
  Serial.println("  LED en pin 6");
  Serial.println("\nEsperando datos...\n");
}

void loop() {
  if (radio.available()) {
    radio.read(&valor, sizeof(valor));
    
    Serial.print("✓ Recibido: ");
    Serial.println(valor);
    
    if (valor == 1) {
      digitalWrite(LED, HIGH);
      Serial.println("  → LED ENCENDIDO");
    } else if (valor == 0) {
      digitalWrite(LED, LOW);
      Serial.println("  → LED APAGADO");
    }
  }
  
  delay(50);
}