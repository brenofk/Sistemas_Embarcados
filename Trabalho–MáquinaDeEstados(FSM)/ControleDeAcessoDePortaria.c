// Definição dos pinos
#define BOTAO1 2      // botão para destravar porta 1
#define SENSOR1 3     // sensor porta 1 (simulado com botão)
#define LED1_R 4      // LED vermelho porta 1
#define LED1_G 5      // LED verde porta 1

#define BOTAO2 6      // botão para destravar porta 2
#define SENSOR2 7     // sensor porta 2 (simulado com botão)
#define LED2_R 8      // LED vermelho porta 2
#define LED2_G 9      // LED verde porta 2

// Estados da FSM
typedef enum {
  AMBAS_FECHADAS,
  P1_DESTRAVADA,
  P1_ABERTA,
  P2_DESTRAVADA,
  P2_ABERTA
} Estado;

Estado estadoAtual = AMBAS_FECHADAS;

// Controle de tempo
unsigned long tempoInicio = 0;
const unsigned long TIMEOUT = 10000; // 10 segundos

// Variáveis para borda dos botões de abertura
bool botao1Anterior = HIGH;
bool botao2Anterior = HIGH;

// Funções para controlar LEDs (simulando eletroímãs)
void travaPorta(int ledR, int ledG) {
  digitalWrite(ledR, HIGH); // vermelho aceso
  digitalWrite(ledG, LOW);  // verde apagado
}

void destravaPorta(int ledR, int ledG) {
  digitalWrite(ledR, LOW);  // vermelho apagado
  digitalWrite(ledG, HIGH); // verde aceso
}

// Setup
void setup() {
  // Configura LEDs
  pinMode(LED1_R, OUTPUT);
  pinMode(LED1_G, OUTPUT);
  pinMode(LED2_R, OUTPUT);
  pinMode(LED2_G, OUTPUT);

  // LEDs começam vermelhos (portas trancadas)
  travaPorta(LED1_R, LED1_G);
  travaPorta(LED2_R, LED2_G);

  // Configura botões de abertura
  pinMode(BOTAO1, INPUT_PULLUP);
  pinMode(BOTAO2, INPUT_PULLUP);

  // Sensores simulados com botões
  pinMode(SENSOR1, INPUT_PULLUP); // HIGH = porta fechada, LOW = porta aberta
  pinMode(SENSOR2, INPUT_PULLUP);

  Serial.begin(9600);
}

// Loop principal
void loop() {
  // Leitura atual dos botões de abertura
  bool botao1Atual = digitalRead(BOTAO1);
  bool botao2Atual = digitalRead(BOTAO2);

  // Detecta borda de descida para destravar porta
  if (botao1Anterior == HIGH && botao1Atual == LOW && estadoAtual == AMBAS_FECHADAS) {
    destravaPorta(LED1_R, LED1_G);
    tempoInicio = millis();
    estadoAtual = P1_DESTRAVADA;
  }

  if (botao2Anterior == HIGH && botao2Atual == LOW && estadoAtual == AMBAS_FECHADAS) {
    destravaPorta(LED2_R, LED2_G);
    tempoInicio = millis();
    estadoAtual = P2_DESTRAVADA;
  }

  botao1Anterior = botao1Atual;
  botao2Anterior = botao2Atual;

  // FSM
  switch (estadoAtual) {
    case P1_DESTRAVADA:
      // Timeout
      if (millis() - tempoInicio >= TIMEOUT) {
        travaPorta(LED1_R, LED1_G);
        estadoAtual = AMBAS_FECHADAS;
      }
      // Porta aberta (sensor pressionado)
      else if (digitalRead(SENSOR1) == LOW) {
        estadoAtual = P1_ABERTA;
      }
      break;

    case P1_ABERTA:
      // Porta fechou (sensor solto)
      if (digitalRead(SENSOR1) == HIGH) {
        travaPorta(LED1_R, LED1_G);
        estadoAtual = AMBAS_FECHADAS;
      }
      break;

    case P2_DESTRAVADA:
      if (millis() - tempoInicio >= TIMEOUT) {
        travaPorta(LED2_R, LED2_G);
        estadoAtual = AMBAS_FECHADAS;
      }
      else if (digitalRead(SENSOR2) == LOW) {
        estadoAtual = P2_ABERTA;
      }
      break;

    case P2_ABERTA:
      if (digitalRead(SENSOR2) == HIGH) {
        travaPorta(LED2_R, LED2_G);
        estadoAtual = AMBAS_FECHADAS;
      }
      break;

    case AMBAS_FECHADAS:
      // LEDs já vermelhos
      break;
  }
}
