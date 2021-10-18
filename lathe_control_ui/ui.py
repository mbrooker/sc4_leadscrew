# Setup info: https://forums.raspberrypi.com/viewtopic.php?t=272003 and https://kivy.org/doc/stable/installation/installation-rpi.html
## If you are using the official Raspberry Pi touch display, you need to configure Kivy to use it as an input source. To do this, edit the file ~/.kivy/config.ini and go to the [input] section. Add this:
## mouse = mouse
## mtdev_%(name)s = probesysfs,provider=mtdev
## hid_%(name)s = probesysfs,provider=hidinput

from kivy.app import App
from kivy.uix.tabbedpanel import TabbedPanel
from kivy.lang import Builder
from kivy.core.window import Window
import sys
import serial

ser = serial.Serial('/dev/ttyACM0',baudrate=115200)
ser.flushInput()
ser.flushOutput()

Window.size = (600, 400)

class Lathe(TabbedPanel):
    def on_touch_move(self, touch):
        print(touch)
        return True

    def on_touch_down(self, touch):
        print(touch)
        return True

class LatheApp(App):
    def build(self):
        return Lathe()

LatheApp().run()
