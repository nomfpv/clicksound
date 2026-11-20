#Requires -RunAsAdministrator
Unregister-ScheduledTask -TaskName 'Mouse Click Sound' -Confirm:$false -ErrorAction SilentlyContinue
Stop-Process -Name clicksound -Force -ErrorAction SilentlyContinue
Start-Sleep 1
Remove-Item "$env:ProgramFiles\clicksound" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item "$env:WINDIR\Media\Mouse Click Down.wav", "$env:WINDIR\Media\Mouse Click Up.wav" -ErrorAction SilentlyContinue
foreach ($id in 'MouseClickDown', 'MouseClickUp') {
    Remove-Item "HKCU:\AppEvents\EventLabels\$id", "HKCU:\AppEvents\Schemes\Apps\.Default\$id" -Recurse -ErrorAction SilentlyContinue
}
