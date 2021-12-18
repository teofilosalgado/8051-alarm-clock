$ConfigurationFilePath = Join-Path -Path $PSScriptRoot -ChildPath "avrdude.conf"
$BinaryFilePath = Join-Path -Path $PSScriptRoot -ChildPath "Objects/8051-alarm-clock.hex"
$FlashCommand = "avrdude -v -C ""$ConfigurationFilePath"" -c stk500v1 -p 89s51 -P COM3 -b 19200 -U flash:w:""$BinaryFilePath"":i"

Write-Output "Executing: $FlashCommand"
Invoke-Expression $FlashCommand