import tkinter as tk
from tkinter import ttk
import sys
import serial
import time

ser = serial.Serial('/dev/ttyACM0',baudrate=115200, timeout=0)
ser.flushInput()
ser.flushOutput()

def transform_status_line(line):
    s = line.split(" ")
    if len(s) != 4:
        return "Unable to parse status line"
    else:
        output_rpm = float(s[0])
        input_rpm = float(s[1])
        feed = float(s[2])
        feed_type = "Half-Nut" if s[3].strip() == "0" else "Power Feed"
        return "Lathe RPM: %.1f Leadscrew RPM: %.1f Feed: %.3f %s"%(input_rpm, output_rpm, feed, feed_type)

class StatusBar(object):
    def __init__(self, root):
        self.bar = tk.Label(root, text="None yet")
        self.bar.pack(side=tk.BOTTOM, fill=tk.X)
        self.bar.after(1, self.update_bar)

    def update_bar(self):
        line = ser.readline().decode("utf-8")
        if line != "":
            print(line)
            new_status = transform_status_line(line)
            self.bar.configure(text=new_status)
        self.bar.after(1000, self.update_bar)

class LatheControl(object):
    def set_feed(self, feed):
        if feed == 'power' or feed == 'nut':
            if feed == 'power':
                ser.write(b"M101\n")
            elif feed == 'nut':
                ser.write(b"M100\n")

    def set_feed_rate(self, feed_rate):
        ser.write(b"F%f\n"%(feed_rate))
    
    def set_power_feed(self, feed_rate):
        self.set_feed('power')
        time.sleep(0.1)
        self.set_feed_rate(feed_rate)

    def set_nut_feed(self, pitch):
        self.set_feed('nut')
        time.sleep(0.1)
        self.set_feed_rate(pitch)
        
control=LatheControl()

root = tk.Tk()
root.geometry("700x400")

root.title("Electronic Leadscrew UI")

bar = StatusBar(root)

notebook = ttk.Notebook(root)
notebook.pack(pady=5, expand=True)

frame_turning = ttk.Frame(notebook)
frame_threading = ttk.Frame(notebook)

for frame in [frame_turning, frame_threading]:
    frame.pack(fill='both', expand=True)

notebook.add(frame_turning, text="Turning")
notebook.add(frame_threading, text="Threading")

# Set up turning tab
button_turning_al = ttk.Button(frame_turning, text="Aluminium Rough", command=lambda: control.set_power_feed(0.5))
button_turning_al.grid(column=0, row=0)

button_turning_al_finish = ttk.Button(frame_turning, text="Aluminium Finish", command=lambda: control.set_power_feed(0.25))
button_turning_al_finish.grid(column=1, row=0)

button_turning_fe = ttk.Button(frame_turning, text="Steel Rough", command=lambda: control.set_power_feed(0.3))
button_turning_fe.grid(column=2, row=0)
button_turning_fe_finish = ttk.Button(frame_turning, text="Steel Finish", command=lambda: control.set_power_feed(0.25))
button_turning_fe_finish.grid(column=3, row=0)

label_turning_custom = ttk.Label(frame_turning, text="Custom feed rate (mm/rev)")
label_turning_custom.grid(column=0, row=1)
turning_custom = ttk.Entry(frame_turning)
turning_custom.insert(tk.END, "0.5")
turning_custom.grid(column=1, row=1)

def update_turning_custom(amount):
    feed=float(turning_custom.get())
    if feed >= -amount:
        feed += amount
    turning_custom.delete(0, tk.END)
    turning_custom.insert(tk.END, "%.3f"%(feed))
    control.set_power_feed(feed)

turning_custom_up = ttk.Button(frame_turning, text="+", command=lambda: update_turning_custom(0.05))
turning_custom_up.grid(column=2, row=1)
turning_custom_down = ttk.Button(frame_turning, text="-", command=lambda: update_turning_custom(-0.05))
turning_custom_down.grid(column=3, row=1)

# Set up the threading tab
pitch_buttons = []
for i, pitch in enumerate([0.5, 0.7, 0.8, 1.0, 1.25, 1.5, 1.75, 2.0]):
    pitch_buttons.append(ttk.Button(frame_threading, text="%.2f"%(pitch), command=lambda p=pitch: control.set_nut_feed(p)))
    pitch_buttons[i].grid(column=i%4, row=int(i / 4))

label_thread_custom = ttk.Label(frame_threading, text="Custom thread pitch (mm/rev)")
label_thread_custom.grid(column=0, row=2)
thread_custom = ttk.Entry(frame_threading)
thread_custom.insert(tk.END, "0.8")
thread_custom.grid(column=1, row=2)

def update_threading_custom(amount):
    pitch = float(thread_custom.get())
    if pitch >= -amount:
        pitch += amount
    thread_custom.delete(0, tk.END)
    thread_custom.insert(tk.END, "%.3f"%(pitch))
    control.set_nut_feed(pitch)

thread_custom_up = ttk.Button(frame_threading, text="+", command=lambda: update_threading_custom(0.05))
thread_custom_up.grid(column=2, row=2)    

thread_custom_down = ttk.Button(frame_threading, text="-", command=lambda: update_threading_custom(-0.05))
thread_custom_down.grid(column=3, row=2)

root.mainloop()
