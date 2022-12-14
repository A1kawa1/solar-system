#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <windows.h>
using namespace std;

const double sunRadius = 20;
const double sunMass = 500;
const int X0 = GetSystemMetrics(SM_CXSCREEN) / 2;
const int Y0 = GetSystemMetrics(SM_CYSCREEN) /2;
const double g = 0.1;
const double mass_to_radius = sunRadius / (sunMass * 2);

class Star {
public:
    double xCord;
    double yCord;
    double radius;
    double xSpeed;
    double ySpeed;
    double xAccel;
    double yAccel;
    double toSunDistance;
    double mass;
    string color;

    Star(double x, double y, double vx, double vy, double ax, double ay, double starMass, string color) {
        xCord = x;
        yCord = y;
        xSpeed = vx;
        ySpeed = vy;
        xAccel = ax;
        yAccel = ay;
        mass = starMass;
        radius = mass * mass_to_radius;
        toSunDistance = 0;
        this->color = color;
    }
    ~Star() = default;

    void changePosition() {
        toSunDistance = sqrt(pow(xCord - X0, 2) + pow(yCord - Y0, 2));
        double fx = g * mass * sunMass * (X0 - xCord) / pow(toSunDistance, 3);
        double fy = g * mass * sunMass * (Y0 - yCord) / pow(toSunDistance, 3);

        xAccel = fx / mass;
        yAccel = fy / mass;

        xSpeed += xAccel;
        ySpeed += yAccel;

        xCord += xSpeed;
        yCord += ySpeed;

        radius = mass * mass_to_radius;
    }
    bool operator== (const Star& other) {
        if ((this->xCord == other.xCord) &&
            (this->yCord == other.yCord) &&
            (this->mass == other.mass)) {
            return true;
        }
        else {
            return false;
        }
    }
    friend Star operator+ (Star s1, Star s2) {
        double newXSpeed = (s1.mass * s1.xSpeed + s2.mass * s2.xSpeed) / (s1.mass + s2.mass);
        double newYSpeed = (s1.mass * s1.ySpeed + s2.mass * s2.ySpeed) / (s1.mass + s2.mass);
        if (s1.radius >= s2.radius) {
            s1.radius += s2.radius;
            s1.mass += s2.mass;
            s1.xSpeed = newXSpeed;
            s1.ySpeed = newYSpeed;
            s2.radius = -1;
            return s1;
        }
        else {
            s2.radius += s1.radius;
            s2.mass += s1.mass;
            s2.xSpeed = newXSpeed;
            s2.ySpeed = newYSpeed;
            s1.radius = -1;
            return s2;
        }
    }
};
double GetRandomNum(double min, double max, int precision)
{
    double value;
    value = rand() % (int)pow(10, precision);
    value = min + (value / pow(10, precision)) * (max - min);
    return value;
}

vector <Star> stars;
void check_crush(Star s1, Star s2) {
    if ((s1.mass == 0) || (s2.mass == 0)) {
        return;
    }
    double distance = sqrt(pow(s1.xCord - s2.xCord, 2) + pow(s1.yCord - s2.yCord, 2));
    if (distance < s1.radius + s2.radius) {
        if (s1.color == "yellow") {
            Star key = s2;
            Star sun = s1;

            std::vector<Star>::iterator itr = std::find(stars.begin(), stars.end(), key);
            int id_del = std::distance(stars.begin(), itr);

            std::vector<Star>::iterator itr_sun = std::find(stars.begin(), stars.end(), sun);
            int id_sun = std::distance(stars.begin(), itr_sun);

            stars[id_sun].mass += key.mass;
            stars[id_del].mass = 0;
        }
        else if (s2.color == "yellow") {
            Star key = s1;
            Star sun = s2;

            std::vector<Star>::iterator itr = std::find(stars.begin(), stars.end(), key);
            int id_del = std::distance(stars.begin(), itr);

            std::vector<Star>::iterator itr_sun = std::find(stars.begin(), stars.end(), sun);
            int id_sun = std::distance(stars.begin(), itr_sun);

            stars[id_sun].mass += key.mass;
            stars[id_del].mass = 0;
        }
        else {
            float Vx = (s1.mass * s1.xSpeed + s2.mass * s2.xSpeed) / (s1.mass + s2.mass);
            float Vy = (s1.mass * s1.ySpeed + s2.mass * s2.ySpeed) / (s1.mass + s2.mass);

            if (s1.mass >= s2.mass) {

                Star key = s1;
                Star del = s2;

                std::vector<Star>::iterator itr = std::find(stars.begin(), stars.end(), key);
                int id = std::distance(stars.begin(), itr);

                std::vector<Star>::iterator itr_del = std::find(stars.begin(), stars.end(), del);
                int id_del = std::distance(stars.begin(), itr_del);

                stars[id].mass += stars[id_del].mass;
                stars[id].xSpeed = Vx;
                stars[id].ySpeed = Vy;

                stars[id_del].mass = 0;
            }
            else {

                Star key = s2;
                Star del = s1;

                std::vector<Star>::iterator itr = std::find(stars.begin(), stars.end(), key);
                int id = std::distance(stars.begin(), itr);

                std::vector<Star>::iterator itr_del = std::find(stars.begin(), stars.end(), del);
                int id_del = std::distance(stars.begin(), itr_del);

                stars[id].mass += stars[id_del].mass;
                stars[id].xSpeed = Vx;
                stars[id].ySpeed = Vy;

                stars[id_del].mass = 0;

            }
        }
    }
}


extern "C" __declspec(dllexport) void generator() {
    srand(time(NULL));
    int count = 100;
    string line;
    ifstream in;
    in.open("python/data.txt");
    if (!in.is_open()) {
        cout << "error";
    }
    else {
        in >> line;
        count = stoi(line);
        cout << count;
    }
    in.close();
    for (int i = 0; i < count; i++) {
        double x = GetRandomNum(10, 1920, 2);
        double y = GetRandomNum(10, 1080, 2);
        double vx = GetRandomNum(0.1, 0.5, 2);
        double vy = GetRandomNum(0.1, 0.5, 2);
        double ax = 0;
        double ay = 0;
        double starMass = GetRandomNum(50, 200, 2);
        stars.push_back(Star(
            x, y, vx, vy, ax, ay, starMass, "white"
        ));
    }
    Star sun = Star(
        X0, Y0, 0, 0, 0, 0, sunMass, "yellow"
    );
    stars.push_back(sun);
}

extern "C" __declspec(dllexport) void dll() {
    for (int i = 0; i < size(stars); i++) {
        for (int j = i + 1; j < size(stars); j++) {
            check_crush(stars[i], stars[j]);
        }
    }
    ofstream out;
    out.open("python/data.txt");
    for (int i = 0; i < size(stars); i++) {
        if (stars[i].color != "yellow") {
            stars[i].changePosition();
        }
        if (stars[i].mass != 0) {
            out << stars[i].xCord << ' ' << stars[i].yCord << ' ' << stars[i].radius << ' ' << stars[i].mass << ' ' << stars[i].color << endl;
        }  
    }
    out.close();
}


