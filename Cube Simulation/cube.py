import serial
from vpython import *

# CHANGE TO YOUR PORT
ser = serial.Serial('COM7', 115200, timeout=0) 

scene.title = "ESP32 Cube Simulation"
scene.width = 1000
scene.height = 700
board = box(length=3, height=0.2, width=2, color=color.orange)

# Arrows
arr_x = arrow(color=color.red, shaftwidth=0.05)
arr_y = arrow(color=color.green, shaftwidth=0.05)
arr_z = arrow(color=color.blue, shaftwidth=0.05)

# Calibration & Smoothing
ZERO_X, ZERO_Y, ZERO_Z = 1860, 1860, 1860
smooth_gravity = vector(0, 1, 0)
alpha = 0.2 # Smoothing factor (0.1 = smooth/slow, 0.9 = jerky/fast)

while True:
    rate(120) # Match high refresh monitors
    
    # CRITICAL: Flush the old data so we only see the 'now'
    if ser.in_waiting > 50: 
        ser.reset_input_buffer()

    line = ser.readline().decode('utf-8', errors='ignore').strip()
    if line:
        parts = line.split(',')
        if len(parts) == 3:
            try:
                # 1. Get raw data
                raw_x = int(parts[0]) - ZERO_X
                raw_y = int(parts[1]) - ZERO_Y
                raw_z = int(parts[2]) - ZERO_Z

                # 2. Map to VPython Vector
                new_gravity = vector(-raw_x, raw_z, raw_y)
                
                # 3. Apply Low-Pass Filter (Smoothing)
                # Formula: current = (new * alpha) + (old * (1 - alpha))
                smooth_gravity = (new_gravity * alpha) + (smooth_gravity * (1.0 - alpha))
                
                # 4. Update Model
                board.up = smooth_gravity
                
                # Update Arrows
                arr_x.axis = board.axis.norm() * 2
                arr_y.axis = board.up.norm() * 1.5
                arr_z.axis = board.axis.cross(board.up).norm() * 1.5
            except:
                continue