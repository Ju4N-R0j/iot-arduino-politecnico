/*
  Entrega 1 - Internet de las cosas
  Uso practico de Arduino: Blink con 5 salidas y 2 entradas
  Autor: Juan Camilo Rojas Rojas
  Simulador: Wokwi (Arduino UNO R3)

  Salidas: 5 LEDs en los pines 13, 12, 11, 10 y 9
  Entradas: 2 pulsadores en los pines 2 y 3 que simulan sensores
    - S1 (pin 2): cambia el modo (0 = blink de todos, 1 = secuencia)
    - S2 (pin 3): cambia la velocidad (500, 250 o 100 ms)
  Los pulsadores usan INPUT_PULLUP, entonces presionado = LOW
*/

const int LEDS[5] = {13, 12, 11, 10, 9};
const int S1 = 2;
const int S2 = 3;

const unsigned long VELOCIDADES[3] = {500, 250, 100};
int indiceVel = 0;
int modo = 0;            // 0 = blink, 1 = secuencia
int ledActual = 0;       // para el modo secuencia
bool estadoBlink = false;

// variables que se van a registrar
unsigned long pulsacionesS1 = 0;
unsigned long pulsacionesS2 = 0;
unsigned long conmutaciones = 0;   // cambios de estado en los LEDs
unsigned long ciclosLoop = 0;      // vueltas del loop en el ultimo segundo

unsigned long tAnteriorLed = 0;
unsigned long tAnteriorReporte = 0;

// antirrebote
bool ultimoS1 = HIGH, ultimoS2 = HIGH;
unsigned long tRebote1 = 0, tRebote2 = 0;
const unsigned long REBOTE_MS = 50;

void apagarTodos() {
  for (int i = 0; i < 5; i++) digitalWrite(LEDS[i], LOW);
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 5; i++) pinMode(LEDS[i], OUTPUT);
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  Serial.println("t_ms;modo;intervalo_ms;pulsaciones_s1;pulsaciones_s2;conmutaciones;ciclos_loop_s");
}

bool leerFlanco(int pin, bool &ultimo, unsigned long &tRebote) {
  bool lectura = digitalRead(pin);
  bool flanco = false;
  if (lectura != ultimo && millis() - tRebote > REBOTE_MS) {
    tRebote = millis();
    if (lectura == LOW) flanco = true;   // se acaba de presionar
    ultimo = lectura;
  }
  return flanco;
}

void loop() {
  unsigned long ahora = millis();
  ciclosLoop++;

  // entrada 1: cambia el modo
  if (leerFlanco(S1, ultimoS1, tRebote1)) {
    pulsacionesS1++;
    modo = (modo + 1) % 2;
    apagarTodos();
    ledActual = 0;
  }

  // entrada 2: cambia la velocidad
  if (leerFlanco(S2, ultimoS2, tRebote2)) {
    pulsacionesS2++;
    indiceVel = (indiceVel + 1) % 3;
  }

  // salidas: blink sin delay() para no bloquear la lectura de los sensores
  if (ahora - tAnteriorLed >= VELOCIDADES[indiceVel]) {
    tAnteriorLed = ahora;
    if (modo == 0) {
      estadoBlink = !estadoBlink;
      for (int i = 0; i < 5; i++) digitalWrite(LEDS[i], estadoBlink);
      conmutaciones += 5;
    } else {
      digitalWrite(LEDS[ledActual], LOW);
      ledActual = (ledActual + 1) % 5;
      digitalWrite(LEDS[ledActual], HIGH);
      conmutaciones += 2;
    }
  }

  // reporte por el monitor serial cada segundo
  if (ahora - tAnteriorReporte >= 1000) {
    tAnteriorReporte = ahora;
    Serial.print(ahora); Serial.print(';');
    Serial.print(modo); Serial.print(';');
    Serial.print(VELOCIDADES[indiceVel]); Serial.print(';');
    Serial.print(pulsacionesS1); Serial.print(';');
    Serial.print(pulsacionesS2); Serial.print(';');
    Serial.print(conmutaciones); Serial.print(';');
    Serial.println(ciclosLoop);
    ciclosLoop = 0;
  }
}
