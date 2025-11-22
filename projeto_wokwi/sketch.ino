// Inclusão das bibliotecas para conectividade Wi-Fi e requisições HTTP.
#include <WiFi.h>
#include <HTTPClient.h>

// --- Configurações de Rede e Plataforma FIWARE ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* fiwareHost = "http://20.246.65.237";
const int fiwarePort = 1026;
String fiwareEntityId = "PostoDeTrabalho:001";

// --- Mapeamento de Pinos dos Componentes ---
const int PINO_TRIGGER = 12;
const int PINO_ECHO = 14;
const int PINO_LED_VERMELHO = 27;
const int PINO_LED_VERDE = 26;
const int PINO_BUZZER = 25;
const int PINO_BOTAO_RESET = 33;
const int PINO_BOTAO_PAUSA = 32;
const int PINO_SENSOR_PIR = 34;

// --- Constantes de Lógica do Sistema ---
const int DISTANCIA_MIN_SEGURA = 50;
const int DISTANCIA_MAX_SEGURA = 70;
const long DURACAO_PAUSA_MS = 30000;
const long TEMPO_TOLERANCIA_AUSENCIA_MS = 30000;

// --- Variáveis Globais de Controle de Estado ---
long tempoInicioPausa = 0;
bool emPausaAlmoco = false;
bool alarmePausaAtivo = false;
bool pessoaPresente = false;
long tempoUltimaDeteccao = 0;
bool alarmeAusenciaAtivo = false;
long ultimoTempoDebounce = 0;
long delayDebounce = 50;

// --- Variáveis de Controle de Envio de Dados ---
long ultimoEnvio = 0;
const long intervaloEnvio = 5000;

//========================= FUNÇÃO DE CONFIGURAÇÃO INICIAL =========================
void setup( ) {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  pinMode(PINO_TRIGGER, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  pinMode(PINO_LED_VERMELHO, OUTPUT);
  pinMode(PINO_LED_VERDE, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(PINO_BOTAO_RESET, INPUT_PULLUP);
  pinMode(PINO_BOTAO_PAUSA, INPUT_PULLUP);
  pinMode(PINO_SENSOR_PIR, INPUT);

  desativarTodosSinais();
  tempoUltimaDeteccao = millis();
}

//========================= LOOP PRINCIPAL DE EXECUÇÃO =========================
void loop() {
  if (emPausaAlmoco) {
    gerenciarPausaAlmoco();
  } else {
    gerenciarPresenca();
  }

  if (millis() - ultimoEnvio > intervaloEnvio) {
    enviarDadosFiware();
    ultimoEnvio = millis();
  }

  delay(200);
}

//========================= FUNÇÕES DE COMUNICAÇÃO =========================
void enviarDadosFiware() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(fiwareHost ) + ":" + fiwarePort + "/v2/entities/" + fiwareEntityId + "/attrs?options=upsert";
    
    http.begin(url );
    http.addHeader("Content-Type", "application/json" );

    digitalWrite(PINO_TRIGGER, LOW); delayMicroseconds(2);
    digitalWrite(PINO_TRIGGER, HIGH); delayMicroseconds(10);
    digitalWrite(PINO_TRIGGER, LOW);
    int distanciaAtual = pulseIn(PINO_ECHO, HIGH) * 0.034 / 2;

    String statusAtual = "Monitorando";
    if (emPausaAlmoco) statusAtual = "Em Pausa";
    if (alarmeAusenciaAtivo) statusAtual = "Ausente";

    String payload = "{";
    payload += "\"distancia\":{\"type\":\"Number\",\"value\":" + String(distanciaAtual) + "},";
    payload += "\"presenca\":{\"type\":\"Boolean\",\"value\":" + String(pessoaPresente ? "true" : "false") + "},";
    payload += "\"status\":{\"type\":\"Text\",\"value\":\"" + statusAtual + "\"}";
    payload += "}";

    int httpResponseCode = http.PATCH(payload );

    if (httpResponseCode > 0 ) {
      Serial.print("Resposta HTTP: ");
      Serial.println(httpResponseCode );
    } else {
      Serial.print("Erro no envio. Código: ");
      Serial.println(httpResponseCode );
    }
    
    http.end( );
  }
}

//========================= FUNÇÕES DE LÓGICA E CONTROLE =========================
void gerenciarPresenca() {
  if (digitalRead(PINO_SENSOR_PIR) == HIGH) {
    // --- LÓGICA PARA QUANDO A PESSOA ESTÁ PRESENTE ---
    if (!pessoaPresente) {
      Serial.println("Usuário presente. Monitoramento ativado.");
      pessoaPresente = true;
    }
    // Se o alarme de ausência estava ativo, desliga-o.
    if (alarmeAusenciaAtivo) {
      Serial.println("Usuário retornou. Desativando alarme de ausência.");
      desativarAlarme();
      alarmeAusenciaAtivo = false;
    }
    
    tempoUltimaDeteccao = millis();
    
    // Somente se o alarme de ausência NÃO estiver ativo, verificamos a distância.
    if (!alarmeAusenciaAtivo) {
      monitorarDistancia();
    }
    
    verificarBotaoPausa();

  } else {
    // --- LÓGICA PARA QUANDO A PESSOA ESTÁ AUSENTE ---
    if (pessoaPresente) {
      Serial.println("Usuário ausente. Iniciando contagem de tolerância...");
      pessoaPresente = false;
      desativarTodosSinais(); // Desliga tudo ao se ausentar.
    }
    // Se o tempo de tolerância foi excedido e o alarme de ausência não está ativo.
    if (!alarmeAusenciaAtivo && (millis() - tempoUltimaDeteccao > TEMPO_TOLERANCIA_AUSENCIA_MS)) {
      Serial.println("Tempo de ausência excedido! Ativando alarme.");
      ativarAlarme();
      alarmeAusenciaAtivo = true;
    }
  }
}

void verificarBotaoPausa() {
  if (digitalRead(PINO_BOTAO_PAUSA) == LOW && (millis() - ultimoTempoDebounce) > delayDebounce) {
    iniciarPausaAlmoco();
    ultimoTempoDebounce = millis();
  }
}

void ativarAlarme() {
  digitalWrite(PINO_LED_VERMELHO, HIGH);
  tone(PINO_BUZZER, 1000);
}

void desativarAlarme() {
  digitalWrite(PINO_LED_VERMELHO, LOW);
  noTone(PINO_BUZZER);
}

void desativarTodosSinais() {
  desativarAlarme();
  digitalWrite(PINO_LED_VERDE, LOW);
}

void monitorarDistancia() {
  digitalWrite(PINO_TRIGGER, LOW); delayMicroseconds(2);
  digitalWrite(PINO_TRIGGER, HIGH); delayMicroseconds(10);
  digitalWrite(PINO_TRIGGER, LOW);
  long duracao = pulseIn(PINO_ECHO, HIGH);
  int distanciaCm = duracao * 0.034 / 2;

  if (distanciaCm >= DISTANCIA_MIN_SEGURA && distanciaCm <= DISTANCIA_MAX_SEGURA) {
    // Se a distância está OK, desliga o alarme (caso estivesse ligado por distância errada)
    // e acende o LED verde.
    desativarAlarme();
    digitalWrite(PINO_LED_VERDE, HIGH);
  } else {
    // Se a distância está errada, ativa o alarme completo (LED + Buzzer).
    digitalWrite(PINO_LED_VERDE, LOW);
    ativarAlarme();
  }
}

void iniciarPausaAlmoco() {
  Serial.println("Iniciando pausa para almoço.");
  emPausaAlmoco = true;
  alarmePausaAtivo = false;
  tempoInicioPausa = millis();
  desativarTodosSinais();
  digitalWrite(PINO_LED_VERDE, HIGH);
}

void gerenciarPausaAlmoco() {
  if (!alarmePausaAtivo && (millis() - tempoInicioPausa > DURACAO_PAUSA_MS)) {
    Serial.println("Fim da pausa! Hora de voltar.");
    alarmePausaAtivo = true;
    digitalWrite(PINO_LED_VERDE, LOW);
    ativarAlarme();
  }
  if (alarmePausaAtivo && digitalRead(PINO_BOTAO_RESET) == LOW) {
    Serial.println("Usuário confirmou o retorno.");
    desativarAlarme();
    emPausaAlmoco = false;
    alarmePausaAtivo = false;
    tempoUltimaDeteccao = millis();
    for(int i=0; i<3; i++) {
      digitalWrite(PINO_LED_VERDE, HIGH); delay(150);
      digitalWrite(PINO_LED_VERDE, LOW); delay(150);
    }
  }
}

