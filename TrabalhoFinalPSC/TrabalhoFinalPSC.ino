#include <LiquidCrystal.h>
#include <SimpleModbusSlave.h>

#define TRIG_1 6
#define ECHO_1 7
#define TRIG_2 8
#define ECHO_2 9

enum {
  DIST_1,                   // distância sensor 1 (cm)
  DIST_2,                   // distância sensor 2 (cm)
  BOMBA2_PWM,               // comando PWM da bomba 2 (0–255)
  HOLDING_REGS_SIZE
};

#define BOMBA1P 4           //in1 ponte H
#define BOMBA1N 5           //in2 ponte H
#define BOMBA2P 3           //in3 ponte H
#define BOMBA2N 2           //in4 ponte H

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
const int backLight = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

struct {
    int limite;
    char *nome;
}

teclas[] = {
    { 50,   "Direita   " },
    { 150,  "Cima     " },
    { 300,  "Baixo    " },
    { 500,  "Esquerda " },
    { 750,  "Select    " },
    { 0,    "Reset       " },
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

void ligaBomba1(int porcentagem){
    float porcentagemLiga = 255.0 * (porcentagem / 100.0);

    analogWrite(BOMBA1P, porcentagemLiga);
}

void ligaBomba2(int porcentagem){
    float porcentagemLiga = 255.0 * (porcentagem / 100.0);

    analogWrite(BOMBA2P, porcentagemLiga);
}

// Fazer o timer funcionar aqui pra NÃO USAR DELAY EM HIPÓTESE ALGUMA
// Delay atrasa a leitura dos sensores e a atuação dos componentes
// O Arduino Uno opera a uma frequência de clock de 16 MHz (16.000.000 de ciclos de clock por segundo)
// Se quisermos ter 0.1s de delay para ler o botão precisamos de 1.600.000 ou 1.000.000
int timer = 1000000;

void setup(){
    Serial.begin(9600);

    modbus_configure(&Serial, 9600, SERIAL_8N1, 1, 0, HOLDING_REGS_SIZE, holdingRegs);

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

void loop(){
    modbus_update();

    holdingRegs[DIST_1] = readUltrasonic(TRIG_1, ECHO_1);
    holdingRegs[DIST_2] = readUltrasonic(TRIG_2, ECHO_2);

    int bomba2_percent = holdingRegs[BOMBA2_PWM];
    if (bomba2_percent > 100){
        bomba2_percent = 100;
    }

    ligaBomba2(bomba2_percent);

    if(timer == 0){
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

        timer = 
    }
    else{
        timer--;
    }

}