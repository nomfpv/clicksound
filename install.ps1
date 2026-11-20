#Requires -RunAsAdministrator
param(
    [Parameter(Mandatory)][string]$Down,
    [Parameter(Mandatory)][string]$Up
)
$ErrorActionPreference = 'Stop'
$dir = "$env:ProgramFiles\clicksound"

New-Item -ItemType Directory -Force $dir | Out-Null
Copy-Item "$PSScriptRoot\clicksound.exe" $dir -Force
Copy-Item $Down "$env:WINDIR\Media\Mouse Click Down.wav" -Force
Copy-Item $Up "$env:WINDIR\Media\Mouse Click Up.wav" -Force

foreach ($e in @{ id = 'MouseClickDown'; name = 'Mouse Click Down' }, @{ id = 'MouseClickUp'; name = 'Mouse Click Up' }) {
    $wav = "$env:WINDIR\Media\$($e.name).wav"
    $key = "HKCU:\AppEvents\Schemes\Apps\.Default\$($e.id)"
    New-Item -Force "HKCU:\AppEvents\EventLabels\$($e.id)" -Value $e.name | Out-Null
    New-Item -Force "$key\.Current" -Value $wav | Out-Null
    New-Item -Force "$key\.Default" -Value $wav | Out-Null
}

$user = "$env:USERDOMAIN\$env:USERNAME"
Register-ScheduledTask -TaskName 'Mouse Click Sound' -Force `
    -Action (New-ScheduledTaskAction -Execute "$dir\clicksound.exe") `
    -Trigger (New-ScheduledTaskTrigger -AtLogOn -User $user) `
    -Principal (New-ScheduledTaskPrincipal -UserId $user -LogonType Interactive -RunLevel Highest) `
    -Settings (New-ScheduledTaskSettingsSet -ExecutionTimeLimit 0 -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries `
        -RestartCount 3 -RestartInterval (New-TimeSpan -Minutes 1) -Priority 4) | Out-Null
Start-ScheduledTask -TaskName 'Mouse Click Sound'
