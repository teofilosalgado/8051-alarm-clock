$ConfigurationFilePath = Join-Path -Path $PSScriptRoot -ChildPath "avrdude.conf"
$BinaryFilePath = Join-Path -Path $PSScriptRoot -ChildPath "Objects/8081-alarm-clock.hex"
$FlashCommand = "avrdude -v -C $ConfigurationFilePath -c stk500v1 -p 89s51 -P COM3 -b 19200 -U flash:w:$BinaryFilePath:i"

Invoke-Expression $FlashCommand