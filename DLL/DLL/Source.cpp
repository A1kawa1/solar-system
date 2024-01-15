#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <windows.h>
using namespace std;

const double sunRadius = 30;
const double sunMass = 500;
const int X0 = GetSystemMetrics(SM_CXSCREEN) / 2;
const int Y0 = GetSystemMetrics(SM_CYSCREEN) /2;
const double g = 0.1;
const double mass_to_radius = sunRadius / (sunMass * 1.5);
const int default_num_stars = 100;

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
    Star key1 = s1;
    Star key2 = s2;

    vector<Star>::iterator itr1 = find(stars.begin(), stars.end(), key1);
    int id1 = std::distance(stars.begin(), itr1);

    vector<Star>::iterator itr2 = find(stars.begin(), stars.end(), key2);
    int id2 = std::distance(stars.begin(), itr2);

    double distance = sqrt(pow(s1.xCord - s2.xCord, 2) + pow(s1.yCord - s2.yCord, 2));
    if (distance < s1.radius + s2.radius) {
        if (s1.color == "yellow") {

            stars[id1].mass += key1.mass;
            stars[id2].mass = 0;
        }
        else if (s2.color == "yellow") {

            stars[id2].mass += key2.mass;
            stars[id1].mass = 0;
        }
        else {
            float Vx = (s1.mass * s1.xSpeed + s2.mass * s2.xSpeed) / (s1.mass + s2.mass);
            float Vy = (s1.mass * s1.ySpeed + s2.mass * s2.ySpeed) / (s1.mass + s2.mass);

            if (s1.mass >= s2.mass) {

                stars[id1].mass += stars[id2].mass;
                stars[id1].xSpeed = Vx;
                stars[id1].ySpeed = Vy;

                stars[id2].mass = 0;
            }
            else {

                stars[id2].mass += stars[id1].mass;
                stars[id2].xSpeed = Vx;
                stars[id2].ySpeed = Vy;

                stars[id1].mass = 0;

            }
        }
    }
    else {
        if ((s1.color != "yellow") && (s2.color != "yellow")) {
            double fx = g * stars[id1].mass * stars[id2].mass * (stars[id2].xCord - stars[id1].xCord) / pow(distance, 3);
            double fy = g * stars[id1].mass * stars[id2].mass * (stars[id2].yCord - stars[id1].yCord) / pow(distance, 3);
            stars[id1].xAccel = fx / stars[id1].mass;
            stars[id1].yAccel = fy / stars[id1].mass;

            stars[id1].xSpeed += stars[id1].xAccel;
            stars[id1].ySpeed += stars[id1].yAccel;

        }
    }
}


extern "C" __declspec(dllexport) void generator() {
    srand(time(NULL));
    int count;
    string line;
    ifstream in;
    in.open("python/data.txt");
    if (!in.is_open()) {
        cout << "error";
    }
    else {
        in >> line;
        try
        {
            count = stoi(line);
        }
        catch (const exception&)
        {
            count = default_num_stars;
        }
        cout << count;
    }
    in.close();
    for (int i = 0; i < count; i++) {
        double x = GetRandomNum(10, 1920, 2);
        double y = GetRandomNum(10, 1080, 2);
        double vx = GetRandomNum(-0.5, 0.5, 2);
        double vy = GetRandomNum(-0.5, 0.5, 2);
        double ax = 0;
        double ay = 0;
        double starMass = GetRandomNum(20, 60, 2);
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
    for (int i = size(stars)-1; i >= 0; i--) {
        if (stars[i].mass == 0) {
            stars.erase(next(stars.begin()) - 1 + i);
        }
    }
    ofstream out;
    out.open("python/data.txt");
    for (int i = 0; i < size(stars); i++) {
        if (stars[i].color != "yellow") {
            stars[i].changePosition();
        }
        out << stars[i].xCord << ' ' << stars[i].yCord << ' ' << stars[i].radius << ' ' << stars[i].mass << ' ' << stars[i].color << endl;
    }
    out.close();
}
