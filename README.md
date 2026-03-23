# Multicore_1_KIT_TC397_TFT
fork and unzip from https://github.com/tccxy/Tc397freertos_lwip

# Infenion AURIX Developer Studio install
https://softwaretools.infineon.com/tools/com.ifx.tb.tool.aurixide

# This project is for multi core board but not for SMP 
> It means FreeRTOS is ported on only CPU0. If you createTask at other cpu, the Task is run at CPU0, not run at own CPUn  
> It means actually this project run AMP(only CPU0 run with FreeRTOS, the others run baremetal, Non-OS.)  

if you want test CAN, Eth jobs, you have to Uncomment ```AppSrc\Include\pub.h``` Line 43~46

## TODO
1. When using Eth Task without unconnect LAN port, the board freeze at Eth Task. => We have to check the cause
