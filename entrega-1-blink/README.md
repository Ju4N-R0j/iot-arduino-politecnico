# Entrega 1 – Blink con 5 salidas y 2 entradas

Arduino UNO R3 con 5 LEDs (D13 a D9, cada uno con 220 Ω) y 2 pulsadores (D2 y D3 con `INPUT_PULLUP`) que simulan sensores.

- **S1 (D2):** cambia el modo (0 = los 5 LEDs parpadean juntos, 1 = secuencia).
- **S2 (D3):** cambia el intervalo (500, 250 y 100 ms).
- Se usa `millis()` en lugar de `delay()` para no bloquear la lectura de los pulsadores, con antirrebote de 50 ms.
- Cada segundo envía por el puerto serial: tiempo, modo, intervalo, pulsaciones, conmutaciones y ciclos del loop.

## Archivos

- `sketch.ino` y `diagram.json`: código y circuito para Wokwi.
- `medir_recursos.ps1`: script de PowerShell que registra cada segundo el uso de CPU y RAM del PC (contadores de Windows) mientras corre la simulación.
- `registros/`: CSV originales generados por el script.

## Nota sobre los registros

En esta entrega solo se alcanzó a registrar parte del escenario de reposo: `recursos_1_reposo.csv` tiene 21 segundos de los 60 previstos. Los escenarios con la simulación corriendo no se midieron, por eso no hay CSV de esos casos.
