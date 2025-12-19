# 🏭 Projeto de Planta Industrial com Controle de Nível via PID

## 👥 Autores

* **Yuri Duarte** – [GitHub](https://github.com/Boyuridod)
* **José Arantes** – [GitHub](https://github.com/JoseArantes83)
* **Vinícius Gabriel** – [GitHub](https://github.com/ViniciusGRBarbosa)

---

## 📘 Descrição do Projeto

Este projeto simula uma **planta industrial**, composta por:

* um **tanque pulmão**, responsável por armazenar e enviar água para outros dois tanques auxiliares;
* dois **tanques auxiliares** com controle de nível individual;
* **controle PID** executado para regular o nível de água em cada tanque auxiliar.

O sistema utiliza:

* **Arduino** para processamento e controle das bombas;
* **SCADABR** para supervisão do processo, comunicação serial e exibição gráfica em tempo real;
* **Sensores ultrassônicos** para medir o nível dos tanques auxiliares;
* **Ponte H** para controlar a potência das bombas;
* **Fonte de 12V** para alimentar a ponte H e as bombas.

---

## ⚙️ Funcionamento Geral

1. O tanque pulmão alimenta os dois tanques auxiliares.
2. Cada tanque auxiliar possui um sensor ultrassônico que envia as medições ao Arduino.
3. O Arduino aplica um controle **PID** para determinar a potência necessária das bombas.
4. As bombas são comandadas pela **Ponte H**, permitindo variação de potência.
5. O SCADABR recebe os dados via conexão serial, exibindo gráficos, indicadores e habilitando monitoramento.

---

## 🖥️ Interface do SCADABR

*(Insira aqui imagens da interface do SCADABR)*

---

## 🏗️ Fotos do Projeto Finalizado

*(Insira aqui fotos do projeto montado)*

---

## 🎥 Vídeo Demonstrativo

Assista ao vídeo completo no YouTube:
👉 [https://youtube.com/shorts/8xrBj0CguZs?si=cVy29H34B37TuoLJ](https://youtube.com/shorts/8xrBj0CguZs?si=cVy29H34B37TuoLJ)

---

## 📂 Estrutura do Repositório

```
├── /arduino-code
│   └── controle_pid.ino
├── /imagens
│   ├── scadabr-interface.png
│   ├── projeto-finalizado-1.jpg
│   └── projeto-finalizado-2.jpg
├── README.md
```

---

## 📦 Materiais Utilizados

* Fonte DC 12V — **1 unidade**
* Ponte H L298N — **1 unidade**
* Bomba de Água 12V (RS385) — **2 unidades**
* LCD Keypad Shield — **1 unidade**
* Arduino Uno R3 — **1 unidade**
* Garrafas PET (tanques auxiliares) — **2 unidades**
* Balde de 7L (tanque pulmão) — **1 unidade**

## 🚀 Tecnologias Utilizadas

* Arduino UNO/Nano
* Sensores Ultrassônicos HC-SR04
* Ponte H (L298N ou similar)
* Bombas hidráulicas 12V
* SCADABR
* Comunicações via Serial
* Controle PID

---

## 📌 Objetivos do Projeto

* Simular uma planta industrial real usando técnicas de automação.
* Implementar controle PID prático aplicado a bombas de nível.
* Criar interface supervisória completa com SCADABR.
* Integrar hardware e software em um ambiente de automação industrial.

---

## 📄 Licença

Este projeto é de uso livre para fins educacionais e experimentais.

---

Se quiser, posso inserir as imagens diretamente no README ou ajustar o texto conforme necessário!
