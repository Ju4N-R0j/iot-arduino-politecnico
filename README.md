# Internet de las Cosas – Uso práctico de Arduino

Proyecto del módulo **Internet de las Cosas (IoT)** – Politécnico Grancolombiano.
Autor: **Juan Camilo Rojas Rojas** (trabajo individual).

El proyecto tiene tres entregas. Cada una está en su carpeta, con el código fuente documentado, la configuración del simulador, los registros originales y las evidencias.

| Entrega | Semana | Tema | Carpeta | Simulación |
|---|---|---|---|---|
| 1 | 3 | Estado del arte, blink con 5 salidas y 2 entradas | [`entrega-1-blink`](entrega-1-blink) | Wokwi |
| 2 | 5 | Comunicación Bluetooth (UART) y control de 2 LEDs | [`entrega-2-bluetooth`](entrega-2-bluetooth) | [Wokwi](https://wokwi.com/projects/481442939229797377) |
| Final | 7 | Wi-Fi con ESP8266 y envío de datos por IP | (pendiente) | |

## Cómo abrir una simulación

1. Entrar a <https://wokwi.com> y crear un proyecto nuevo de **Arduino Uno**.
2. Reemplazar el contenido de `sketch.ino` y `diagram.json` por los de la carpeta de la entrega.
3. Presionar (Start the simulation) y usar el monitor serial que aparece abajo.

También se puede abrir directamente el enlace público de Wokwi de cada entrega (tabla de arriba).

## Estructura

```
entrega-1-blink/
  sketch.ino, diagram.json      código y circuito
  medir_recursos.ps1            script de medición de CPU/RAM (Windows)
  registros/                    CSV originales generados por el script
entrega-2-bluetooth/
  sketch.ino, diagram.json      código y circuito
  registros/                    texto original del monitor serial
  evidencias/                   capturas de cada prueba
  figuras/                      diagramas usados en el artículo
```
