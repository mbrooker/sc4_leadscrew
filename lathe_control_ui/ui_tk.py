import tkinter as tk
from tkinter import ttk
import sys
import serial

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
        feed_type = 'Half-Nut' if s[4] == "0" else "Power Feed"
        return "Lathe RPM: %.1f Leadscrew RPM: %.1f Feed: %.3f %s"%(input_rpm, output_rpm, feed, feed_type)

class StatusBar(object):
    def __init__(self, root):
        self.bar = tk.Label(root, text="None yet")
        self.bar.pack(side=tk.BOTTOM, fill=tk.X)
        self.bar.after(1, self.update_bar)

    def update_bar(self):
        line = ser.readline().decode("utf-8")
        if line != "":
            new_status = transform_status_line(line)
            self.bar.configure(text=new_status)
        self.bar.after(100, self.update_bar)

class LatheControl(object):
    def __init__(self):
        self.feed = 'power'
        self.feed_rate = 0.25

    def set_feed(self, feed):
        if (feed != self.feed) and (feed == 'power' or feed == 'nut'):
            self.feed = feed
            if feed == 'power':
                ser.write_line("M101")
            elif feed = 'nut':
                ser.write_line("M100")

    def set_feed_rate(self, feed_rate):
        if feed_rate != self.feed_rate:
            self.feed_rate = feed_rate
            ser.write_line("F%f"%(feed_rate))
        

root = tk.Tk()

root.title("Electronic Leadscrew UI")

bar = StatusBar(root)

notebook = ttk.Notebook(root)
notebook.pack(pady=5, expand=True)

frame_rough = ttk.Frame(notebook)
frame_finish = ttk.Frame(notebook)

for frame in [frame_rough, frame_finish]:
    frame.pack(fill='both', expand=True)

notebook.add(frame_rough, text="Rough Turning")
notebook.add(frame_finish, text="Finish Turning")

root.mainloop()
