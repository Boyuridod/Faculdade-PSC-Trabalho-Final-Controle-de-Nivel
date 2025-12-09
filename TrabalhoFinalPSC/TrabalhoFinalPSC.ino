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

// Índices dos registradores Modbus (holdingRegs[])
enum {
  DIST_1,  // Nível do tanque 1 (Arduino → Scada) [cm]
  DIST_2,  // Nível do tanque 2 (Arduino → Scada) [cm]

  SETPOINT_1,  // Nível desejado para o tanque 1 (Scada → Arduino) [cm]
  SETPOINT_2,  // Nível desejado para o tanque 2 (Scada → Arduino) [cm]

  OUT_PID_1,  // Saída calculada do PID 1 (Arduino → Scada) [0-255 PWM]
  OUT_PID_2,  // Saída calculada do PID 2 (Arduino → Scada) [0-255 PWM]

  KP_1,  // Ganho proporcional do PID 1 (Scada → Arduino)
  KI_1,  // Ganho integral do PID 1 (Scada → Arduino)
  KD_1,  // Ganho derivativo do PID 1 (Scada → Arduino)

  KP_2,  // Ganho proporcional do PID 2 (Scada → Arduino)
  KI_2,  // Ganho integral do PID 2 (Scada → Arduino)
  KD_2,  // Ganho derivativo do PID 2 (Scada → Arduino)

  MODO,  // Modo de operação do sistema

  HOLDING_REGS_SIZE  // Tamanho total do array (fica sempre por último)
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

#define direita 50
#define cima 150
#define baixo 300
#define esquerda 500
#define select 750
#define reset 0

int modo = 0;

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
const int ALTURA_MAX = 25;

unsigned long lastComTime = 0;
const unsigned long timeoutScada = 5000;
#define LED_STATUS 13

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

  PID1.SetOutputLimits(0, 100);
  PID2.SetOutputLimits(0, 100);

  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, HIGH);
}

void loop() {

  bool pacoteRecebido = modbus_update();

  if (pacoteRecebido) {
    lastComTime = millis();
  }

  // --- LED STATUS ---
  if (millis() - lastComTime > timeoutScada)
    digitalWrite(LED_STATUS, HIGH);  // sem Scada → LED acende
  else
    digitalWrite(LED_STATUS, LOW);  // com Scada → LED apaga

  long dist1 = readUltrasonic(TRIG_1, ECHO_1);
  long dist2 = readUltrasonic(TRIG_2, ECHO_2);

  long nivel1 = ALTURA_MAX - dist1;
  long nivel2 = ALTURA_MAX - dist2;

  if (nivel1 < 0) nivel1 = 0;
  if (nivel1 > ALTURA_MAX) nivel1 = ALTURA_MAX;

  if (nivel2 < 0) nivel2 = 0;
  if (nivel2 > ALTURA_MAX) nivel2 = ALTURA_MAX;

  holdingRegs[DIST_1] = nivel1;
  holdingRegs[DIST_2] = nivel2;

  modo = holdingRegs[MODO];

  Input1 = nivel1;
  Input2 = nivel2;

  Set1 = holdingRegs[SETPOINT_1];
  Set2 = holdingRegs[SETPOINT_2];

  if (modo == 1) {
    if (Set1 > 100) Set1 = 100;
    if (Set2 > 100) Set2 = 100;

    holdingRegs[SETPOINT_1] = Set1;
    holdingRegs[SETPOINT_2] = Set2;
  } else {
    if (Set1 > ALTURA_MAX) Set1 = ALTURA_MAX;
    if (Set2 > ALTURA_MAX) Set2 = ALTURA_MAX;

    holdingRegs[SETPOINT_1] = Set1;
    holdingRegs[SETPOINT_2] = Set2;
  }

  // ======== ATUALIZA OS PARÂMETROS DO PID ========
  // Conversão: valores recebidos vêm x10 → usamos /10.0
  double Kp1 = holdingRegs[KP_1] / 10.0;
  double Ki1 = holdingRegs[KI_1] / 10.0;
  double Kd1 = holdingRegs[KD_1] / 10.0;

  double Kp2 = holdingRegs[KP_2] / 10.0;
  double Ki2 = holdingRegs[KI_2] / 10.0;
  double Kd2 = holdingRegs[KD_2] / 10.0;

  PID1.SetTunings(Kp1, Ki1, Kd1);
  PID2.SetTunings(Kp2, Ki2, Kd2);

  int bomba1_percent;
  int bomba2_percent;
  if (modo == 0) {
    PID1.Compute();
    PID2.Compute();

    bomba1_percent = (int)Output1;
    bomba2_percent = (int)Output2;
  } else {
    bomba1_percent = holdingRegs[SETPOINT_1];
    bomba2_percent = holdingRegs[SETPOINT_2];
  }

  // 30 a 100
  if (bomba1_percent < 0) bomba1_percent = 0;
  if (bomba1_percent > 100) bomba1_percent = 100;

  if (bomba2_percent < 0) bomba2_percent = 0;
  if (bomba2_percent > 100) bomba2_percent = 100;

  ligaBomba1(bomba1_percent);
  ligaBomba2(bomba2_percent);

  // Mostrar no Serial valores recebidos e enviados
  // Serial.print("DIST1=");
  // Serial.print(holdingRegs[DIST_1]);
  // Serial.print("  DIST2=");
  // Serial.print(holdingRegs[DIST_2]);
  // Serial.print("  SP1=");
  // Serial.print(holdingRegs[SETPOINT_1]);
  // Serial.print("  SP2=");
  // Serial.print(holdingRegs[SETPOINT_2]);
  // Serial.print("  KP1=");
  // Serial.print(holdingRegs[KP_1]);
  // Serial.print("  KI1=");
  // Serial.print(holdingRegs[KI_1]);
  // Serial.print("  KD1=");
  // Serial.print(holdingRegs[KD_1]);
  // Serial.print("  KP2=");
  // Serial.print(holdingRegs[KP_2]);
  // Serial.print("  KI2=");
  // Serial.print(holdingRegs[KI_2]);
  // Serial.print("  KD2=");
  // Serial.print(holdingRegs[KD_2]);

  // Serial.print("  OUT1=");
  // Serial.print(Output1);
  // Serial.print("  OUT2=");
  // Serial.println(Output2);


  // ======== LCD Atualizado a cada 100 ms ========
  if (millis() - ultimoTempo >= intervalo) {
    // ATUALIZAR PARA TECLAS MUDAREM O SETPOINT. ATUALMENTE
    // SALVE ENGANO QUEM MUDA E O SCADABR


    // CÓDIGO ANTIGO FUNCIONAL:

    static int teclaAnt = -1;

    int leitura = analogRead(A0);

    int teclaNova;
    for (teclaNova = 0;; teclaNova++) {
      if (leitura < teclas[teclaNova].limite) {
        break;
      }
    }

    if (teclaNova != teclaAnt) {
      if (teclas[teclaAnt].limite == select) {
        Set1++;

        if (modo == 1) {
          if (Set1 > 100) Set1 = 100;
        } else {
          if (Set1 > ALTURA_MAX) Set1 = ALTURA_MAX;
        }

        holdingRegs[SETPOINT_1] = Set1;
      } else if (teclas[teclaAnt].limite == esquerda) {
        Set1--;

        if (Set1 < 0) Set1 = 0;

        holdingRegs[SETPOINT_1] = Set1;
      } else if (teclas[teclaAnt].limite == cima) {
        Set2++;

        if (modo == 1) {
          if (Set2 > 100) Set2 = 100;
        } else {
          if (Set2 > ALTURA_MAX) Set2 = ALTURA_MAX;
        }

        holdingRegs[SETPOINT_2] = Set2;
      } else if (teclas[teclaAnt].limite == baixo) {
        Set2--;

        if (Set2 < 0) Set2 = 0;

        holdingRegs[SETPOINT_2] = Set2;
      } else if (teclas[teclaAnt].limite == direita) {
        modo++;

        if (modo > 1) modo = 0;

        holdingRegs[MODO] = modo;
      }

      teclaAnt = teclaNova;
    }

    // CÓDIGO DO CHATGPT PARA USO com PID PRINTANDO VARIÁVEIS NO LCD:
    lcd.setCursor(0, 0);
    lcd.print("O1:");
    lcd.print(Output1);
    lcd.print(" O2:");
    lcd.print(Output2);

    lcd.setCursor(0, 1);
    lcd.print("S1:");
    lcd.print(holdingRegs[SETPOINT_1]);
    lcd.print(" S2:");
    lcd.print(holdingRegs[SETPOINT_2]);
    if (modo) {
      lcd.print(" MAN. ");
    } else {
      lcd.print(" AUTO.");
    }

    ultimoTempo = millis();
  }
}
