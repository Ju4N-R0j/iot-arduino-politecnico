# Entrega 2 – Comunicación Bluetooth mediante Arduino

Arduino UNO R3 que recibe comandos de un carácter por comunicación serial **UART a 9600 baudios (8N1)** y enciende o apaga dos LEDs.

**Simulación pública en Wokwi:** <https://wokwi.com/projects/481442939229797377>

## Comandos

| Comando | Acción |
|---|---|
| `V` | Enciende el LED verde (el rojo se apaga) |
| `R` | Enciende el LED rojo (el verde se apaga) |
| `A` | Enciende ambos LEDs |
| `0` | Apaga ambos LEDs |
| otro | Responde `COMANDO INVALIDO` y no cambia los LEDs |

Se aceptan minúsculas. Se ignoran `\n`, `\r` y espacios. Los bytes que no son caracteres imprimibles (ASCII 32 a 126) se descartan y se informan.

## Conexiones

| Elemento | Pin Arduino | Nota |
|---|---|---|
| LED verde + 220 Ω | D8 | |
| LED rojo + 220 Ω | D9 | |
| HC-05 TXD | D2 (RX de SoftwareSerial) | solo montaje físico |
| HC-05 RXD | D3 (TX de SoftwareSerial) | con divisor 1 kΩ / 2 kΩ (el RXD del HC-05 es de 3,3 V) |
| HC-05 VCC / GND | 5V / GND | solo montaje físico |

**Limitación del simulador:** Wokwi no tiene el módulo HC-05, así que en la simulación el **monitor serial** (UART por D0/D1) hace el papel del celular más el módulo Bluetooth. El sketch también escucha por SoftwareSerial en D2/D3 para que el mismo código sirva en el montaje físico.

## Pruebas (corrida del 2026-11-22)

| Prueba | Comando | Resultado esperado | Resultado obtenido |
|---|---|---|---|
| 01 | `V` | Encender LED verde | Cumple: verde=1 rojo=0 |
| 02 | `R` | Encender LED rojo | Cumple: verde=0 rojo=1 |
| 03 | `A` | Encender ambos LED | Cumple: verde=1 rojo=1 |
| 04 | `0` | Apagar ambos LED | Cumple: verde=0 rojo=0 |
| 05 | `X` | Notificar comando inválido | Cumple: "COMANDO INVALIDO", LEDs sin cambio |
| 06 | `v` | Aceptar minúscula | Cumple: se interpretó como `V` |
| 07 | `RA0` | Tres comandos en una línea | Cumple: se ejecutaron en orden R, A, 0 |

- Registro original del monitor serial: [`registros/monitor_serial_2026-11-22.txt`](registros/monitor_serial_2026-11-22.txt)
- Corrida preliminar con el byte espurio: [`registros/corrida_preliminar_byte_espurio.txt`](registros/corrida_preliminar_byte_espurio.txt)
- Capturas de cada prueba: carpeta [`evidencias/`](evidencias)

## Tiempo de envío UART medido vs. teórico

Teórico = caracteres × 10 bits / 9600 bit/s (8N1 = 1 bit de inicio + 8 de datos + 1 de parada).

| Comando | Caracteres | Medido (ms) | Teórico (ms) | Diferencia |
|---|---|---|---|---|
| V | 67 | 69,96 | 69,79 | +0,17 ms |
| R | 66 | 68,92 | 68,75 | +0,17 ms |
| A | 68 | 71,01 | 70,83 | +0,18 ms |
| 0 | 66 | 68,92 | 68,75 | +0,17 ms |
| X | 81 | 84,58 | 84,38 | +0,20 ms |
