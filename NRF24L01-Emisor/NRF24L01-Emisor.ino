#include <SPI.h>
#include <RF24.h>

// Pines para nRF24 en ideaspark ESP32 OLED Board
const int CE = 2;
const int CSN = 4;

// Pines SPI del board
const int PIN_SCK = 14;
const int PIN_MOSI = 13;
const int PIN_MISO = 12;

RF24 radio(CE, CSN);
const uint64_t direccion_escritura = 0xE0E0F0F0E1LL;
const uint64_t direccion_lectura = 0xE0E0F0F0E2LL;

int buttonPin = 5;
int lectura_actual = 0;
int lectura_anterior = 0;
int estado = 0;

void setup() {
  Serial.begin(115200);
  delay(1500);
  
  Serial.println("\n========== EMISOR nRF24 ==========");
  
  pinMode(buttonPin, INPUT);
  
  // Inicializar SPI
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, CSN);
  delay(100);
  
  Serial.println("Iniciando nRF24...");
  
  if (!radio.begin()) {
    Serial.println("❌ Error: nRF24 no inicializó");
    while (1) delay(1000);
  }
  
  Serial.println("✓ nRF24 inicializó");
  
  // Configuración crítica
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  
  // Abrir tubo de escritura
  radio.openWritingPipe(direccion_escritura);
  radio.openReadingPipe(1, direccion_lectura);
  
  radio.stopListening();
  delay(100);
  
  Serial.println("✓ Configuración completada");
  Serial.print("  Escribiendo a: 0x");
  Serial.println(direccion_escritura, HEX);
  Serial.println("\n¡Presiona botón en GPIO 5!\n");
}

void loop() {
  lectura_actual = digitalRead(buttonPin);
  
  // Detección de flanco (solo cuando se presiona)
  if ((lectura_actual == HIGH) && (lectura_anterior == LOW)) {
    estado = 1; // Encender
    Serial.print("► Botón presionado → Encendiendo LED");
    
    // Enviar dato
    if (radio.write(&estado, sizeof(estado))) {
      Serial.println(" ✓ Enviado");
    } else {
      Serial.println(" ✗ No enviado");
    }
    delay(200);
  }
  
  // Detección de flanco (cuando se suelta)
  if ((lectura_actual == LOW) && (lectura_anterior == HIGH)) {
    estado = 0; // Apagar
    Serial.print("► Botón soltado → Apagando LED");
    
    // Enviar dato
    if (radio.write(&estado, sizeof(estado))) {
      Serial.println(" ✓ Enviado");
    } else {
      Serial.println(" ✗ No enviado");
    }
    delay(200);
  }
  
  lectura_anterior = lectura_actual;
  delay(50);
}