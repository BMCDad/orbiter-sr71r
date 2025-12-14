Nov. 1, 2025

SR71 Panel right 1
Contains the blend files for the right-1 panel which contains:
clock
altimeter
vsi
fuel flow
fuel quantity

By convention a file in the form <name>-<name> should have a 'Render' collection that renders that files output to a texture file.
A file without '-' is a sub-component that is linked into another file, in this case the main SR71-panelr1 blend file.

The output of this directory should then be the following texture files:
altimeter-hundreds.png
altimeter-tens.png
altimeter-thousands.png
clock-elapsed.png
clock-hour.png
clock-minute.png
clock-second.png
fuelflow-hand.png
fuelmain-hand.png
vsi-hand.png
sr71r-panelr1.png