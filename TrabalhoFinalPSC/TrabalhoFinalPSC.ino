#include <LiquidCrystal.h>
#include <SimpleModbusSlave.h>

#define TRIG_1 A5
#define ECHO_1 A4
#define TRIG_2 A3
#define ECHO_2 A2

#define BOMBA1P 5  //in1 ponte H
#define BOMBA1N 4  //in2 ponte H
#define BOMBA2P 3  //in3 ponte H
#define BOMBA2N 2  //in4 ponte H

enum {
  DIST_1,      // distância sensor 1 (cm)
  DIST_2,      // distância sensor 2 (cm)
  BOMBA1_PWM,  // comando PWM da bomba 1 (0–255)
  BOMBA2_PWM,  // comando PWM da bomba 2 (0–255)
  HOLDING_REGS_SIZE
};

const int d4 = 6, d5 = 7, d6 = 8, d7 = 9, rs = 10, en = 11;
const int backLight = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

uint16_t holdingRegs[HOLDING_REGS_SIZE];

struct {
  int limite;
  char *nome;
}

teclas[] = {
  { 50, "Direita   " },
  { 150, "Cima     " },
  { 300, "Baixo    " },
  { 500, "Esquerda " },
  { 750, "Select    " },
  { 0, "Reset       " },
  { 1024, "            " }
};

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

void ligaBomba1(int porcentagem) {
  float porcentagemLiga = 255.0 * (porcentagem / 100.0);

  analogWrite(BOMBA1P, porcentagemLiga);
}

void ligaBomba2(int porcentagem) {
  float porcentagemLiga = 255.0 * (porcentagem / 100.0);

  analogWrite(BOMBA2P, porcentagemLiga);
}

unsigned long ultimoTempo = 0;
const unsigned long intervalo = 100;  // 1 segundo em ms

void setup() {
  modbus_configure(&Serial, 9600, SERIAL_8N1, 1, 0, HOLDING_REGS_SIZE, holdingRegs);

  Serial.begin(9600);

  pinMode(TRIG_1, OUTPUT);
  pinMode(ECHO_1, INPUT);
  pinMode(TRIG_2, OUTPUT);
  pinMode(ECHO_2, INPUT);

  pinMode(BOMBA1P, OUTPUT);
  pinMode(BOMBA1N, OUTPUT);
  pinMode(BOMBA2P, OUTPUT);
  pinMode(BOMBA2N, OUTPUT);
  pinMode(backLight, OUTPUT);

  analogWrite(BOMBA1P, LOW);
  analogWrite(BOMBA1N, LOW);
  analogWrite(BOMBA2P, LOW);
  analogWrite(BOMBA2N, LOW);

  lcd.begin(16, 2);
  lcd.print("SuperMaker");

  digitalWrite(backLight, HIGH);
}

void loop() {
  modbus_update();

  holdingRegs[DIST_1] = readUltrasonic(TRIG_1, ECHO_1);
  holdingRegs[DIST_2] = readUltrasonic(TRIG_2, ECHO_2);

  int bomba2_percent = holdingRegs[BOMBA2_PWM];
  if (bomba2_percent > 100) bomba2_percent = 100;
  ligaBomba2(bomba2_percent);

  int bomba1_percent = holdingRegs[BOMBA1_PWM];
  if (bomba1_percent > 100) bomba1_percent = 100;
  ligaBomba1(bomba1_percent);

  if (millis() - ultimoTempo >= intervalo) {

    static int teclaAnt = -1;

    int leitura = analogRead(A0);

    int teclaNova;
    for (teclaNova = 0;; teclaNova++) {
      if (leitura < teclas[teclaNova].limite) {
        break;
      }
    }

    if (teclaNova != teclaAnt) {
      lcd.setCursor(0, 1);
      lcd.print(teclas[teclaNova].nome);
      teclaAnt = teclaNova;
    }

    ultimoTempo = millis();
  }
}