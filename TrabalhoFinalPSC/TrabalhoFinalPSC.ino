#include <LiquidCrystal.h>
#include <SimpleModbusSlave.h>
#include <PID_v1.h>

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
  SETPOINT_1,  // comando PWM da bomba 1 (0–255)
  SETPOINT_2,  // comando PWM da bomba 2 (0–255)
  OUT_PID_1,   // saída PID nível 1
  OUT_PID_2,   // saída PID nível 2
  KP_1,
  KI_1,
  KD_1,
  KP_2,
  KI_2,
  KD_2,
  HOLDING_REGS_SIZE
};

uint16_t holdingRegs[HOLDING_REGS_SIZE];

const int d4 = 6, d5 = 7, d6 = 8, d7 = 9, rs = 10, en = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define backLight 12

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
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);  // timeout 30 ms

  long distance = duration * 0.034 / 2;  // cm
  if (distance > 400) distance = 400;
  if (distance < 0) distance = 0;

  return distance;
}

// VARIÁVEIS DO PID
double Input1, Output1, Set1;
double Input2, Output2, Set2;

// PID Kp Ki Kd
PID PID1(&Input1, &Output1, &Set1, 1.0, 0.0, 0.0, DIRECT);
PID PID2(&Input2, &Output2, &Set2, 1.0, 0.0, 0.0, DIRECT);

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
  Serial.begin(9600);
  modbus_configure(&Serial, 9600, SERIAL_8N1, 1, 0, HOLDING_REGS_SIZE, holdingRegs);

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
  digitalWrite(backLight, HIGH);

  // PID config
  PID1.SetMode(AUTOMATIC);
  PID2.SetMode(AUTOMATIC);

  PID1.SetOutputLimits(0, 255);
  PID2.SetOutputLimits(0, 255);
}

void loop() {
  modbus_update();

  holdingRegs[DIST_1] = readUltrasonic(TRIG_1, ECHO_1);
  holdingRegs[DIST_2] = readUltrasonic(TRIG_2, ECHO_2);

  Input1 = holdingRegs[DIST_1];
  Input2 = holdingRegs[DIST_2];

  Set1 = holdingRegs[SETPOINT_1];
  Set2 = holdingRegs[SETPOINT_2];

  // ======== ATUALIZA OS PARÂMETROS DO PID ========
  PID1.SetTunings(holdingRegs[KP_1], holdingRegs[KI_1], holdingRegs[KD_1]);
  PID2.SetTunings(holdingRegs[KP_2], holdingRegs[KI_2], holdingRegs[KD_2]);

  // ======== EXECUTA O PID ========
  PID1.Compute();
  PID2.Compute();

  int bomba1_percent = (int)Output1;
  if (bomba1_percent > 100) bomba1_percent = 100;
  ligaBomba1(bomba1_percent);

  int bomba2_percent = (int)Output2;
  if (bomba2_percent > 100) bomba2_percent = 100;
  ligaBomba2(bomba2_percent);

  // ======== LCD Atualizado a cada 100 ms ========
  if (millis() - ultimoTempo >= intervalo) {
    // ATUALIZAR PARA TECLAS MUDAREM O SETPOINT. ATUALMENTE
    // SALVE ENGANO QUEM MUDA E O SCADABR


    // CÓDIGO ANTIGO FUNCIONAL:

    // static int teclaAnt = -1;

    // int leitura = analogRead(A0);

    // int teclaNova;
    // for (teclaNova = 0;; teclaNova++) {
    //   if (leitura < teclas[teclaNova].limite) {
    //     break;
    //   }
    // }

    // if (teclaNova != teclaAnt) {
    //   lcd.setCursor(0, 1);
    //   lcd.print(teclas[teclaNova].nome);
    //   teclaAnt = teclaNova;
    // }

    // ultimoTempo = millis();

    // CÓDIGO DO CHATGPT PARA USO PID PRINTANDO VARIÁVEIS NO LCD:
    lcd.setCursor(0, 0);
    lcd.print("N1:");
    lcd.print(Input1);
    lcd.print(" SP:");
    lcd.print(Set1);

    lcd.setCursor(0, 1);
    lcd.print("N2:");
    lcd.print(Input2);
    lcd.print(" SP:");
    lcd.print(Set2);

    ultimoTempo = millis();
  }
}