# medir_recursos.ps1
# Registra el uso de CPU y RAM del PC (indicadores del sistema operativo) mientras corre la simulacion en Wokwi.
# Uso (en PowerShell, dentro de esta carpeta):
#   powershell -ExecutionPolicy Bypass -File .\medir_recursos.ps1 -Escenario "1_reposo" -Segundos 60
# Escenarios sugeridos:
#   1_reposo            -> navegador cerrado o sin Wokwi
#   2_wokwi_sin_simular -> Wokwi abierto pero la simulacion detenida
#   3_blink_500ms       -> simulando, modo blink a 500 ms
#   4_secuencia_100ms   -> simulando, modo secuencia a 100 ms (presionar S1 y S2 dos veces)
# Genera un archivo recursos_<escenario>.csv en la misma carpeta.

param(
  [string]$Escenario = "prueba",
  [int]$Segundos = 60,
  [string[]]$Navegadores = @("chrome", "msedge", "firefox", "brave", "opera")
)

$nucleos = (Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors
$salida = Join-Path $PSScriptRoot ("recursos_" + $Escenario + ".csv")
"segundo;escenario;cpu_total_pct;ram_usada_pct;ram_usada_mb;cpu_navegador_pct;ram_navegador_mb" | Out-File -FilePath $salida -Encoding utf8

Write-Host "Midiendo '$Escenario' durante $Segundos s ($nucleos nucleos logicos)..."
for ($s = 1; $s -le $Segundos; $s++) {
  $cpu = (Get-CimInstance Win32_PerfFormattedData_PerfOS_Processor -Filter "Name='_Total'").PercentProcessorTime
  $os = Get-CimInstance Win32_OperatingSystem
  $ramTotalMB = [math]::Round($os.TotalVisibleMemorySize / 1024, 0)
  $ramLibreMB = [math]::Round($os.FreePhysicalMemory / 1024, 0)
  $ramUsadaMB = $ramTotalMB - $ramLibreMB
  $ramPct = [math]::Round(100 * $ramUsadaMB / $ramTotalMB, 1)

  $procNav = Get-CimInstance Win32_PerfFormattedData_PerfProc_Process |
    Where-Object { $n = $_.Name; ($Navegadores | Where-Object { $n -like "$_*" }).Count -gt 0 }
  $cpuNav = 0; $ramNavMB = 0
  if ($procNav) {
    $cpuNav = [math]::Round((($procNav | Measure-Object PercentProcessorTime -Sum).Sum) / $nucleos, 1)
    $ramNavMB = [math]::Round((($procNav | Measure-Object WorkingSetPrivate -Sum).Sum) / 1MB, 0)
  }

  "$s;$Escenario;$cpu;$ramPct;$ramUsadaMB;$cpuNav;$ramNavMB" | Out-File -FilePath $salida -Append -Encoding utf8
  Write-Host ("[{0,3}s] CPU {1,3}% | RAM {2}% | Navegador CPU {3}% RAM {4} MB" -f $s, $cpu, $ramPct, $cpuNav, $ramNavMB)
  Start-Sleep -Milliseconds 850
}
Write-Host "Listo. Archivo: $salida"
