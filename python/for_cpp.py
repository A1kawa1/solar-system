from typing import ClassVar
from math import sqrt
import pygame
from pygame import *
import random
from dataclasses import dataclass
from time import sleep

pygame.init()
screen = pygame.display.set_mode((0,0), pygame.FULLSCREEN)
timer = pygame.time.Clock()
info = pygame.display.Info()

M0 = 500
#Sun position
X0 = info.current_w // 2
Y0 = info.current_h // 2
R0=30
@dataclass
class Star:
    x: float
    y: float
    radius: float
    mass: float
    vx: float
    vy: float
    ax: float
    ay: float
    color: str
    r_for_sun: ClassVar[float]
    G: ClassVar = 0.1
    def position(self):
        self.r_for_sun = sqrt((self.x - X0)**2 + (self.y - Y0)**2)
        Fx = self.G * self.mass * M0 * (X0 - self.x) / self.r_for_sun**3
        Fy = self.G * self.mass * M0 * (Y0 - self.y) / self.r_for_sun**3

        self.ax = Fx / self.mass
        self.ay = Fy / self.mass

        self.vx += self.ax
        self.vy += self.ay

        self.x += self.vx
        self.y += self.vy

    def __del__(self):
        del self


def check_crush(stars, self, other):
    sun = stars[-1]
    if self is None or other is None:
        return
    distance = sqrt((self.x - other.x)**2 + (self.y - other.y)**2)
    if distance < self.radius + other.radius:
        if self == sun:
            stars[-1].mass += other.mass
            id_del = stars.index(other)
            stars[id_del] = None
        elif other == sun:
            stars[-1].mass += self.mass
            id_del = stars.index(self)
            stars[id_del] = None
        else:
            Vx = (self.mass * self.vx + other.mass * other.vx) / (self.mass + other.mass)
            Vy = (self.mass * self.vy + other.mass * other.vy) / (self.mass + other.mass)
            if self.mass >= other.mass:
                self.radius += other.radius
                self.mass += other.mass
                self.vx = Vx
                self.vy = Vy
                id_del = stars.index(other)
                stars[id_del] = None
            else:
                other.radius += self.radius
                other.mass += self.mass
                other.vx = Vx
                other.vy = Vy
                id_del = stars.index(self)
                stars[id_del] = None


stars = [
    Star(
        x=random.uniform(10, 1920),
        y=random.uniform(10, 1080),
        radius=random.uniform(1, 3),
        mass=random.uniform(20, 100),
        color='white',
        vx=random.uniform(0.1, 0.5),
        vy=random.uniform(0.1, 0.5),
        ax=0,
        ay=0.0
    ) for _ in range(256)
]

sun = Star(
    x=X0,
    y=Y0,
    radius=R0,
    mass=M0,
    color='yellow',
    vx=0,
    vy=0,
    ax=0,
    ay=0
)

stars.append(sun)
done = False
previous_mass = 0
while not done:
    timer.tick(50)
    for e in pygame.event.get():
        if e.type == QUIT:
            done = True
            break
    screen.fill((0, 0, 0))

    for i in range(0, len(stars)):
        for j in range(i+1, len(stars)):  
            check_crush(stars, stars[i], stars[j])
    for el in stars:
        if el is None:
            stars.remove(el)
    for star in stars:
        if star == sun:
            continue
        star.position()
        pygame.draw.circle(
            surface=screen,
            color=star.color,
            center=(star.x, star.y),
            radius=star.radius
        )
    if previous_mass == sun.mass:
        pygame.draw.circle(
            surface=screen,
            color=sun.color,
            center=(sun.x, sun.y),
            radius=sun.radius
        )
        pygame.display.update()
    else:
        previous_mass = sun.mass
        pygame.draw.circle(
            surface=screen,
            color='red',
            center=(sun.x, sun.y),
            radius=sun.radius
        )
        pygame.display.update()
        sleep(0.05)