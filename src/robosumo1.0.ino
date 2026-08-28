#include <Bluepad32.h>

// ==========================
// Definição dos pinos
// ==========================
#define AIN1 19
#define AIN2 18
#define PWMA 32

#define BIN1 17   // TX2
#define BIN2 16   // RX2
#define PWMB 33

// ==========================
// PWM config
// ==========================
const int freq = 1000;
const int canalA = 0;
const int canalB = 1;
const int resolucao = 8;

// ==========================
// Controle
// ==========================
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// ==========================
// FUNÇÕES DOS MOTORES
// ==========================
void setMotorA(int velocidade) {
  Serial.print("Motor A: ");

  if (velocidade > 0) {
    Serial.println("FRENTE");
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } 
  else if (velocidade < 0) {
    Serial.println("TRAS");
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  } 
  else {
    Serial.println("PARADO");
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }

  ledcWrite(canalA, abs(velocidade));
}

void setMotorB(int velocidade) {
  Serial.print("Motor B: ");

  if (velocidade > 0) {
    Serial.println("FRENTE");
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } 
  else if (velocidade < 0) {
    Serial.println("TRAS");
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } 
  else {
    Serial.println("PARADO");
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }

  ledcWrite(canalB, abs(velocidade));
}

// ==========================
// CONTROLE DO GAMEPAD
// ==========================
void processGamepad(ControllerPtr ctl) {

  int vel = 200;

  Serial.println("---- COMANDOS ----");

  // Motor A (setas)
  if (ctl->dpad() & DPAD_UP) {
    Serial.println("DPAD UP pressionado");
    setMotorA(vel);
  } 
  else if (ctl->dpad() & DPAD_DOWN) {
    Serial.println("DPAD DOWN pressionado");
    setMotorA(-vel);
  } 
  else {
    setMotorA(0);
  }

  // Motor B (botões)
  if (ctl->y()) { // TRIÂNGULO
    Serial.println("TRIANGULO pressionado");
    setMotorB(vel);
  } 
  else if (ctl->a()) { // X
    Serial.println("X pressionado");
    setMotorB(-vel);
  } 
  else {
    setMotorB(0);
  }
}

// ==========================
// CALLBACKS
// ==========================
void onConnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.println("Controle conectado!");
      myControllers[i] = ctl;
      break;
    }
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.println("Controle desconectado!");
      myControllers[i] = nullptr;
      break;
    }
  }
}

// ==========================
// SETUP
// ==========================
void setup() {
  Serial.begin(115200);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  // PWM A
  ledcSetup(canalA, freq, resolucao);
  ledcAttachPin(PWMA, canalA);

  // PWM B
  ledcSetup(canalB, freq, resolucao);
  ledcAttachPin(PWMB, canalB);

  // Bluetooth
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();

  Serial.println("Sistema iniciado!");
}

// ==========================
// LOOP
// ==========================
void loop() {

  if (BP32.update()) {
    for (auto ctl : myControllers) {
      if (ctl && ctl->isConnected()) {
        processGamepad(ctl);
      }
    }
  }

  delay(100);
}