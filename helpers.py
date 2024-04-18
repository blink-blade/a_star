import cProfile
import math
import os
import sys
from _ctypes import Structure
from ctypes import CDLL, c_int, c_double

from kivy.core.window import Window
from kivy.uix.image import Image


game_map = {}
chunk_images = {}

original = [640, 360]
resolutions = [[original[0] * (x + 1), original[1] * (x + 1)] for x in range(6)]
window_size = [*resolutions[2]]
window_width = window_size[0]
window_height = window_size[1]
Window.size = window_size
profiler = cProfile.Profile()
map_size = (200, 200)


gamepad_button_mapping = {
    0: 'jump',
    1: 'b',  # controller_b
    2: 'x',
    3: 'y',
    4: 'left_shoulder',
    5: 'r_shoulder',  # controller_left_shoulder
    6: 'select',  # controller_select
    7: 'menu',
    9: 'debug', # controller_left_analog_press
    10: 'controller_right_analog_press',
    # 9: 'controller_start'
}

gamepad_hat_mapping = {
    (0, 1): 'up',
    (1, 0): 'right',
    (0, -1): 'down',
    (-1, 0): 'left'
}

keyboard_mapping = {
    32: 'jump',  # boing
    97: 'left',  # letter a
    119: 'up',  # letter w
    100: 'right',  # letter d
    115: 'down',  # letter s
    27: 'menu', # escape
    120: 'x',
    303: 'debug'
}



class AxisControl:
    def __init__(self):
        self.input = {'right': False, 'up': False, 'left': False, 'down': False}

    def filter(self, direction, value, which_controller):
        which_controller = int(which_controller)
        if self.input[direction] is not value:
            controls[direction] = value
            self.input[direction] = value


axis0 = AxisControl()
hat = AxisControl()


def on_motion(_window, begin_end, event, *_):
    key = None
    if hasattr(event, 'button'):
        if event.button == 'scrolldown':
            key = 'minus'
        elif event.button == 'scrollup':
            key = 'plus'
    if key:
        controls[key] = True if begin_end == 'begin' else False


def on_joy_axis(_, stick_id, axis_id, value, ):
    # print('on joy hat', _, stick_id, axis_id, value)
    # TODO - create some intermediary to prevent the input being flagged while it's
    #  still being held, but stick moved
    # print(value, stick_id, axis_id)
    deadzone_amount = 25000
    stick_id = int(stick_id)
    if axis_id == 0:
        if value > deadzone_amount:
            axis0.filter('right', True, stick_id)
        else:
            axis0.filter('right', False, stick_id)
        if value < -deadzone_amount:
            axis0.filter('left', True, stick_id)
        else:
            axis0.filter('left', False, stick_id)
    else:
        if value > deadzone_amount:
            axis0.filter('down', True, stick_id)
        else:
            axis0.filter('down', False, stick_id)
        if value < -deadzone_amount:
            axis0.filter('up', True, stick_id)
        else:
            axis0.filter('up', False, stick_id)

def on_joy_hat(_, _a, _b, velocity):
    # print('on joy hat', _, _a, _b, velocity)
    if velocity[1] == 1:
        hat.filter('up', True, _a)
    else:
        hat.filter('up', False, _a)
    if velocity[0] == 1:
        hat.filter('right', True, _a)
    else:
        hat.filter('right', False, _a)
    if velocity[1] == -1:
        hat.filter('down', True, _a)
    else:
        hat.filter('down', False, _a)
    if velocity[0] == -1:
        hat.filter('left', True, _a)
    else:
        hat.filter('left', False, _a)


def on_joy_ball(*args, **kwargs):
    # print('joy ball', args, kwargs)
    pass


def on_joy_button_up(_window, _unknown, button_code):
    controls[gamepad_button_mapping.get(button_code, button_code)] = False


def on_joy_button_down(_window, _unknown, button_code):
    controls[gamepad_button_mapping.get(button_code, button_code)] = True


def on_key_up(_window, keycode, *_rest):
    key_name = keyboard_mapping.get(keycode)
    if key_name:
        controls[key_name] = False


def on_key_down(_window, keycode, *_rest):
    key_name = keyboard_mapping.get(keycode)
    if key_name:
        controls[key_name] = True


controls = {k: False for k in ['up', 'down', 'left', 'tab', 'right', 'jump', 'dash', 'y', 'b', 'menu', 'map', 'x', 'aim', 'left_shoulder', 'r_shoulder', 'debug']}
