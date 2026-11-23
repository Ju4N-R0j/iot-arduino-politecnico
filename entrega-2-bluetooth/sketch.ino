/*
  ============================================================================
  Entrega 2 - Internet de las Cosas (Politecnico Grancolombiano)
  Implementacion de un sistema de comunicacion Bluetooth mediante Arduino
  Autor: Juan Camilo Rojas Rojas
  Placa: Arduino UNO R3 (ATmega328P)   Simulador: Wokwi
  ============================================================================

  Que hace:
    Arduino recibe un caracter por comunicacion serial UART (9600 baudios, 8N1),
    lo interpreta como un comando y enciende o apaga dos LEDs.

      Comando  Accion
      -------  -------------------------------------------
        V      Enciende el LED verde (el rojo queda apagado)
        R      Enciende el LED rojo  (el verde queda apagado)
        A      Enciende ambos LEDs
        0      Apaga ambos LEDs
      otro     Comando invalido: se notifica y los LEDs no cambian

    Se aceptan mayusculas y minusculas. Los saltos de linea y espacios que
    envia el monitor serial se ignoran.

  Por donde llegan los comandos:
    1) Serial (pines D0/D1, USB): en Wokwi el monitor serial hace el papel del
       celular + modulo HC-05, porque el simulador no tiene el modulo Bluetooth.
    2) SoftwareSerial en D2 (RX) y D3 (TX): es donde se conecta el HC-05 en un
       montaje fisico. Se deja listo para que el mismo codigo sirva en la placa
       real sin ocupar el puerto USB.
       Conexiones HC-05 -> Arduino:
         VCC -> 5V, GND -> GND, TXD -> D2,
         RXD <- D3 a traves de un divisor de voltaje (1 kOhm y 2 kOhm) porque
         el RX del HC-05 trabaja a 3,3 V.

  Datos que se muestran por cada comando (evidencia de las pruebas):
    - tiempo de la simulacion en ms
    - comando recibido, accion ejecutada y estado de cada LED
    - tiempo de procesamiento del comando en microsegundos
    - tiempo que tardo en transmitirse la respuesta por la UART en ms
      (con su longitud en caracteres para compararlo con el valor teorico)
    - contadores de comandos validos e invalidos
    - aviso cuando se descarta un byte que no es un caracter imprimible
*/

#include <SoftwareSerial.h>

// ---------------- Configuracion de pines ----------------
const int LED_VERDE = 8;        // LED verde + resistencia de 220 ohm
const int LED_ROJO  = 9;        // LED rojo  + resistencia de 220 ohm
const int BT_RX     = 2;        // recibe lo que transmite el HC-05 (TXD)
const int BT_TX     = 3;        // transmite hacia el HC-05 (RXD)

const long BAUDIOS = 9600;      // misma velocidad en Arduino, monitor y HC-05

SoftwareSerial bluetooth(BT_RX, BT_TX);

// ---------------- Variables de estado ----------------
bool verdeEncendido = false;
bool rojoEncendido  = false;
unsigned long comandosValidos   = 0;
unsigned long comandosInvalidos = 0;
unsigned long bytesDescartados  = 0;

// Envia el mismo texto al monitor serial y al modulo Bluetooth
void responder(const String &texto) {
  Serial.println(texto);
  bluetooth.println(texto);
}

// Aplica el estado de las variables a los pines de salida
void actualizarLeds() {
  digitalWrite(LED_VERDE, verdeEncendido ? HIGH : LOW);
  digitalWrite(LED_ROJO,  rojoEncendido  ? HIGH : LOW);
}

void setup() {
  // 1. Inicializar la comunicacion serial
  Serial.begin(BAUDIOS);
  bluetooth.begin(BAUDIOS);
  // Pull-up interna en el RX del HC-05: si el modulo no esta conectado
  // (como en Wokwi) el pin queda flotando y llegan caracteres basura
  pinMode(BT_RX, INPUT_PULLUP);

  // 2. Configurar los pines digitales como salidas y arrancar apagados
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  actualizarLeds();

  responder("Sistema listo. UART 9600 baudios 8N1.");
  responder("Comandos: V=verde  R=rojo  A=ambos  0=apagar");
}

// Interpreta un comando. Devuelve true si es valido.
// En 'accion' deja el texto que describe lo que se hizo.
bool interpretarComando(char comando, String &accion) {
  switch (comando) {
    case 'V':
      verdeEncendido = true;  rojoEncendido = false;
      accion = "LED verde ENCENDIDO";
      return true;
    case 'R':
      verdeEncendido = false; rojoEncendido = true;
      accion = "LED rojo ENCENDIDO";
      return true;
    case 'A':
      verdeEncendido = true;  rojoEncendido = true;
      accion = "Ambos LED ENCENDIDOS";
      return true;
    case '0':
      verdeEncendido = false; rojoEncendido = false;
      accion = "Ambos LED APAGADOS";
      return true;
    default:
      accion = "COMANDO INVALIDO, use V, R, A o 0";
      return false;
  }
}

// Procesa un caracter que llego por uno de los dos puertos.
// 'origen' indica por donde llego: "USB" (monitor serial) o "BT" (HC-05).
void procesar(char recibido, const char *origen) {
  // 3. Ignorar fin de linea y espacios que agrega el monitor serial
  if (recibido == '\n' || recibido == '\r' || recibido == ' ') return;

  // Descartar bytes que no son caracteres imprimibles (ruido electrico,
  // pin RX flotante o basura del modulo al energizarse)
  byte b = (byte)recibido;
  if (b < 32 || b > 126) {
    bytesDescartados++;
    responder("[t=" + String(millis()) + " ms] Byte no valido 0x" + String(b, HEX) +
              " descartado (origen " + String(origen) + ")");
    return;
  }

  char comando = toupper(recibido);
  String accion;

  // 4. Interpretar el comando y medir cuanto tarda el procesamiento
  unsigned long t0 = micros();
  bool valido = interpretarComando(comando, accion);
  if (valido) actualizarLeds();          // 5. Encender / apagar los LED
  unsigned long tProceso = micros() - t0;

  // 6. Validar comandos incorrectos
  if (valido) comandosValidos++;
  else comandosInvalidos++;

  // 7. Mostrar el resultado en el monitor serial y medir el envio por la UART
  //    de hardware (solo Serial, sin incluir el SoftwareSerial)
  String linea = "[t=" + String(millis()) + " ms] Recibido '" + String(comando) + "' -> " + accion +
                 " | verde=" + String(verdeEncendido) + " rojo=" + String(rojoEncendido);
  unsigned long t1 = micros();
  Serial.println(linea);
  Serial.flush();                         // espera a que salga el ultimo bit
  unsigned long tUart = micros() - t1;
  bluetooth.println(linea);               // la misma respuesta hacia el HC-05

  responder("   proceso=" + String(tProceso) + " us | envio UART=" + String(tUart / 1000.0, 2) +
            " ms (" + String(linea.length() + 2) + " car.) | validos=" + String(comandosValidos) + " invalidos=" + String(comandosInvalidos));
}

void loop() {
  // Comandos desde el monitor serial (simula el enlace celular - HC-05)
  while (Serial.available() > 0) {
    procesar((char)Serial.read(), "USB");
  }
  // Comandos desde el modulo HC-05 en un montaje fisico
  while (bluetooth.available() > 0) {
    procesar((char)bluetooth.read(), "BT");
  }
}
