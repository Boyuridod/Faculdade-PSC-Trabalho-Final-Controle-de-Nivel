#include <ModbusRtu.h>

Modbus slave(1, Serial, 0); // 1 = Slave ID

// Cria um array com registradores Modbus
uint16_t au16data[20];

void setup() {
  Serial.begin(9600);
  slave.start();

  // Inicializa algumas variáveis
  au16data[0] = 50;  // Setpoint (holding register)
  au16data[10] = 0;  // Leitura analógica (input register)
}

void loop() {
  // Lê/escreve registros Modbus
  slave.poll(au16data, 20);

  // Leitura analógica simples
  int leitura = analogRead(A0);          // 0–1023
  float nivel = leitura * 100.0 / 1023.0;
  au16data[10] = (uint16_t) nivel;       // Input register
}
