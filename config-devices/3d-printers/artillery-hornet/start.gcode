
G28 ;Home
M420 S1; Mesh and fade height
G92 E0 ; reset extruder
;Prime the extruder
G1 Z2.0 F3000 ; move z up little to prevent scratching of surface
G1 X200.0 Y0.0 Z0.3 F5000.0 ; move to start-line position
G1 X100.0 Y0.0 Z0.3 F1500.0 E10 ; draw 1st line
G1 X100.0 Y0.3 Z0.3 F5000.0 ; move to side a little
G1 X200.0 Y0.3 Z0.3 F1500.0 E20 ; draw 2nd line
G1 X200.0 Y0.6 Z0.3 F5000.0 ; move a little again
G1 X100.0 Y0.6 Z0.3 F1500.0 E30 ; draw 2nd line

G92 E0 ; reset extruder
G1 Z1.0 F3000 ; move z up little to prevent scratching of surface
