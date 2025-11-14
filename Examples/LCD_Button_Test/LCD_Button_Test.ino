#include <LiquidCrystal.h>

// Define as conexões e cria o objeto para acesso
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
const int backLight = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Limites para detecção das teclas, em ordem crescente
struct {
    int limite;
    char *nome;
} teclas[] = {
    { 50, "Direita   " },
    { 150, "Cima     " },
    { 300, "Baixo    " },
    { 500, "Esquerda " },
    { 750, "Select   " },
    { 0, "Reset    " },
    { 1024, "        " } // nenhuma tecla apertada
};

void setup() {
    pinMode(backLight, OUTPUT);

    lcd.begin(16, 2);
    lcd.print("SuperMaker");

    digitalWrite(backLight, HIGH);
}

void loop() {
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

    delay(100);
}
