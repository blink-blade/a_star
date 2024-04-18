import time
from kivy import Config
from kivy.app import App
from kivy.clock import Clock
from kivy.graphics import Fbo, PushMatrix, Color, Rectangle, PopMatrix, Scale, Rotate

# import generator

from helpers import *
values = []

# joysticks = [pygame.joystick.Joystick(x) for x in range(pygame.joystick.get_count())]
# get_axis 0, 1 are dpad, and 3, 4 are right analog
Config.set('graphics', 'resizable', True)
class GameApp(App):
    def __init__(self, **kwargs):
        super(GameApp, self).__init__(**kwargs)
        Window.bind(on_motion=on_motion)
        Window.bind(on_joy_axis=on_joy_axis, on_joy_hat=on_joy_hat, on_joy_ball=on_joy_ball,
                    on_joy_button_up=on_joy_button_up, on_joy_button_down=on_joy_button_down, on_key_up=on_key_up, on_key_down=on_key_down)
        profiler.enable()
        self.seed = 2


    def on_start(self):
        Clock.schedule_interval(self.update, 1/60)


    def update(self, dt):
        # print(Clock.get_fps())
        # self.value += 1/60
        # if self.value > 35:
        #     profiler.disable()
        #     profiler.dump_stats('myapp.profile')
        #     exit()
        if controls.get('x'):
            if controls.get('up'):
                controls.pop('up')
            if controls.get('down'):
                controls.pop('down')

if __name__ == '__main__':
    GameApp().run()

