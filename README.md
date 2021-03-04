# Brightness-Controller

this is a project with ATMega32 microcontroller to control brightness of a room with opening and closing the curtain and dimming the LED.
real time is computed using Timer0 of microcontroller and brightness is measured by light sensor. 
if there is dark in the room on a day, micro opens the curtain and if there is still dark, turns on the LED. and if there is too bright, 
oposite actions happen. on the night, brightness is controlled only with LED. 
it is compeletly modeled with proteus and the code is written and compiled in Code Vision AVR.