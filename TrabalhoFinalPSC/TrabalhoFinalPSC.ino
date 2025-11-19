#define BOMBA1P 4
#define BOMBA1N 5
#define BOMBA2P 3
#define BOMBA2N 2

void ligaBomba2(int porcentagem){
    float porcentagemLiga = 255.0 * (porcentagem / 100.0);

    analogWrite(BOMBA2P, porcentagemLiga);
}

void setup(){
    Serial.begin(9600);

    pinMode(BOMBA1P, OUTPUT);
    pinMode(BOMBA1N, OUTPUT);
    pinMode(BOMBA2P, OUTPUT);
    pinMode(BOMBA2N, OUTPUT);

    analogWrite(BOMBA1P, LOW);
    analogWrite(BOMBA1N, LOW);
    analogWrite(BOMBA2P, LOW);
    analogWrite(BOMBA2N, LOW);
}

void loop(){
    ligaBomba2(100);
}