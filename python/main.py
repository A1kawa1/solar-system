import ctypes
import pygame
from pygame import *
from time import sleep

pygame.init()
screen = pygame.display.set_mode((0,0), pygame.FULLSCREEN)
timer = pygame.time.Clock()

lib=ctypes.CDLL('DLL/x64/Debug/DLL.dll')
lib.generator()


done = False
previous_mass = 0
while not done:
    timer.tick(50)
    for e in pygame.event.get():
        if e.type == QUIT:
            done = True
            break

    lib.dll()
    stars = []
    with open('python/data.txt') as file:
        strings = file.readlines()
        for string in strings:
            string = string[:-1].split(sep=' ')
            star = list(map(float, string[:4])) + [string[4]]
            stars.append(star)
    sun = stars[-1]
    screen.fill((0, 0, 0))
    for star in stars[:-1]:
        pygame.draw.circle(
            surface=screen,
            color=star[4],
            center=(star[0], star[1]),
            radius=star[2]
        )
    if previous_mass == sun[3]:
        pygame.draw.circle(
            surface=screen,
            color=sun[4],
            center=(sun[0], sun[1]),
            radius=sun[2]
        )
        pygame.display.update()
    else:
        previous_mass = sun[3]
        pygame.draw.circle(
            surface=screen,
            color='red',
            center=(sun[0], sun[1]),
            radius=sun[2]
        )
        pygame.display.update()
        sleep(0.05)