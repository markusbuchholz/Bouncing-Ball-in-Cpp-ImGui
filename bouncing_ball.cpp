// Markus Buchholz. 2023

#include <iostream>
#include <tuple>
#include <vector>
#include <math.h>
#include <cmath>

#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

//----------- system dynamic parameters --------------------

float v0 = 30.0;
float theta = 70.0 * M_PI / 180.0;
float g = 9.81;

float x_0 = 0.0;
float y_0 = 0.0;

float dt = 0.01;
//---------------------------------------------------------------

std::tuple<float, float> simulation(float xi, float t, float e)
{

    auto x = [=](float xi, float t)
    {
        return xi + x_0 + v0 * std::cos(theta) * t;
    };

    auto y = [=](float t, float e)
    {
        return y_0 + v0 * e * std::sin(theta) * t - 0.5 * g * t * t;
    };

    return std::make_tuple(x(xi, t), y(t, e));
}
//---------------------------------------------------------------

void plot2D(std::vector<float> X, std::vector<float> Y)
{

    plt::title("Bouncing ball ");
    plt::named_plot("ball path", X, Y);
    plt::xlabel("pos X");
    plt::ylabel("pos Y");
    plt::legend();
    plt::xlabel("pos X");
    plt::ylabel("pos Y");
    plt::show();
}

//---------------------------------------------------------------
std::tuple<bool, float, float> ball(float xi, float t, float e)
{

    auto f = simulation(xi, t, e);
    if (std::get<1>(f) >= 0.0)
    {

        return std::make_tuple(true, std::get<0>(f), std::get<1>(f));
    }
    return std::make_tuple(false, std::get<0>(f), std::get<1>(f));
}
//---------------------------------------------------------------

std::tuple<std::vector<float> , std::vector<float>> simulation()
{

    std::vector<float> x;
    std::vector<float> y;

    float xi = 0.0;
    int N = 0;

    while (N < 5)
    {
        float t = 0.0;

        while (t < 10.0)
        // for (; t < 10; t = t + dt)
        {
            auto e = 1.0 / (N + 2.0);
            auto sim = ball(xi, t, e);

            if (std::get<2>(sim) >= 0)
            {
                x.push_back(std::get<1>(sim));
                y.push_back(std::get<2>(sim));
            }
            t = t + dt;
            if (std::get<0>(sim) == false)
            {

                N += 1;
                xi = std::get<1>(sim);
                t = 100.0;
            }
        }
    }
    return std::make_tuple(x,y);
}

int main()
{
    auto sim = simulation();
    plot2D(std::get<0>(sim), std::get<1>(sim));
}
