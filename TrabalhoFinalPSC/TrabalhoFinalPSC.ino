#include <SimpleModbusSlave.h>

// ====== Pinos dos Ultrassônicos ======
#define TRIG_1 2
#define ECHO_1 3
#define TRIG_2 4
#define ECHO_2 5

// ====== Pinos das Bombas ======
#define BOMBA1P 6
#define BOMBA1N 7
#define BOMBA2P 8
#define BOMBA2N 9

// ====== Registradores Modbus ======
enum {
  DIST_1,      // distância sensor 1 (cm)
  DIST_2,      // distância sensor 2 (cm)
  BOMBA2_PWM,  // comando PWM da bomba 2 (0–255)
  HOLDING_REGS_SIZE
};

uint16_t holdingRegs[HOLDING_REGS_SIZE];

// ===== Função genérica para ler ultrassônico =====
long readUltrasonic(int trigPin, int echoPin) {
  long duration;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);  // timeout 30 ms

  long distance = duration * 0.034 / 2;  // cm

  if (distance > 400) distance = 400;
  if (distance < 0) distance = 0;

  return distance;
}

void ligaBomba2(int porcentagem) {
  float porcentagemLiga = 255.0 * (porcentagem / 100.0);

  analogWrite(BOMBA2P, porcentagemLiga);
}

void setup() {
  modbus_configure(&Serial, 9600, SERIAL_8N1, 1, 0, HOLDING_REGS_SIZE, holdingRegs);

  Serial.begin(9600);

  // Ultrassônicos
  pinMode(TRIG_1, OUTPUT);
  pinMode(ECHO_1, INPUT);
  pinMode(TRIG_2, OUTPUT);
  pinMode(ECHO_2, INPUT);

  // Bombas
  pinMode(BOMBA1P, OUTPUT);
  pinMode(BOMBA1N, OUTPUT);
  pinMode(BOMBA2P, OUTPUT);
  pinMode(BOMBA2N, OUTPUT);

  analogWrite(BOMBA1P, LOW);
  analogWrite(BOMBA1N, LOW);
  analogWrite(BOMBA2P, LOW);
  analogWrite(BOMBA2N, LOW);
}

void loop() {

  // Atualiza comunicação Modbus
  modbus_update();

  // Atualiza distância lida pelos ultrassônicos
  holdingRegs[DIST_1] = readUltrasonic(TRIG_1, ECHO_1);
  holdingRegs[DIST_2] = readUltrasonic(TRIG_2, ECHO_2);

  // Controle da Bomba 2 pelo Modbus (0–100%)
  int bomba2_percent = holdingRegs[BOMBA2_PWM];
  if (bomba2_percent > 100) bomba2_percent = 100;

  ligaBomba2(bomba2_percent);

  delay(100);
}