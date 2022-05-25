G91 ; relative pos
G1 E-6 F9000; retract 6
G90 ; absolute 
G1 X200 Y200 F10000 ; Go to corner

M104 S0 T0 ; turn off temperature
M140 S0 ; turn off bed
M84 ; disable motors

M300 S440 P200 ; tune, sounds a bit.
M300 S660 P250
M300 S880 P300
