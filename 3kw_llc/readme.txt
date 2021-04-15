
Folder description：
	app : application code,include "main.c"
	cmsis_core : support file from st and arm
	doc : controlboard pcb and SW FlowChart and SW Description
	libs : hal driver from st , and dsp function from arm
	output : build and output file,such as memorymap , Burnable file : 3KW_LLC_SW.hex in objects folder
	project : project files for MDK(keil 5.30)

All configuration properties are in globaldefine.h

open project :
1.open folder /project
2.open file /project/Project.uvprojx(need install keil MDK5 first)