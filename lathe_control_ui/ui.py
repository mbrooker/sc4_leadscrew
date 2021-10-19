# Setup info: https://forums.raspberrypi.com/viewtopic.php?t=272003 and https://kivy.org/doc/stable/installation/installation-rpi.html
## If you are using the official Raspberry Pi touch display, you need to configure Kivy to use it as an input source. To do this, edit the file ~/.kivy/config.ini and go to the [input] section. Add this:
## mouse = mouse
## mtdev_%(name)s = probesysfs,provider=mtdev
## hid_%(name)s = probesysfs,provider=hidinput

from kivy.app import App
from kivy.uix.tabbedpanel import TabbedPanel
from kivy.clock import Clock, _default_time as time
from kivy.uix.boxlayout import BoxLayout
from kivy.lang import Builder
from kivy.core.window import Window
import sys
import serial

ser = serial.Serial('/dev/ttyACM0',baudrate=115200, timeout=0)
ser.flushInput()
ser.flushOutput()

Window.size = (600, 400)



def transform_status_line(line):
    s = line.split(" ")
    if len(s) != 5:
        return "Unable to parse status line"
    else:
        input_rpm = float(s[0])
        output_rpm = float(s[1])
        leadscrew_dir = 'CW' if s[2] == "0" else "CCW"
        feed = float(s[3])
        feed_type = 'Half-Nut' if s[4] == "0" else "Power Feed"
        return "Lathe RPM: %.1f Leadscrew RPM: %.1f %s Feed: %.3f %s"%(input_rpm, output_rpm, leadscrew_dir, feed, feed_type)

class Lathe(BoxLayout):
    pass

class LatheApp(App):
    def build(self):
        Clock.schedule_interval(self.consume, 0.5)
        self.widgets = Lathe()
        return self.widgets

    def consume(self, tm):
        line = ser.readline().decode("utf-8")
        if line != "":
            print(line)
            self.widgets.ids.status_label.text = transform_status_line(line)

LatheApp().run()
