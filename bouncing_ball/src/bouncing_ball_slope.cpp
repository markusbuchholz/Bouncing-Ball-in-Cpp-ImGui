// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include "ball.h"
#include <tuple>
#include <thread>
#include <chrono>
#include <vector>
#include <math.h>

//---------------------------------------------------------------

std::vector<Point> points = {
	{{100, 300}, 10}};

//----------- system dynamic parameters --------------------

float v0 = 100.0;
float theta = 70.0 * M_PI / 180.0;
float g = 9.81;

float alfa = 5.0 * M_PI /180.0;
float x_0 = 0.0;
float y_0 = 0.0;

float dt = 0.01;
//---------------------------------------------------------------

std::tuple<float, float> simulation(float xi, float t, float e, float yi)
{

    auto x = [=](float xi, float t)
    {
        return xi + x_0 + v0 * std::cos(theta) * t;
    };

    auto y = [=](float t, float e, float yi)
    {
        return yi + y_0 + v0 * e * std::sin(theta) * t - 0.5 * g * t * t;
    };

    return std::make_tuple(x(xi, t), y(t, e, yi));
}

//---------------------------------------------------------------
std::tuple<bool, float, float> ball(float xi, float t, float e, float yi)
{

    auto f = simulation(xi, t, e, yi);

    auto c = - std::get<0>(f) * std::sin(alfa);

    if (std::get<1>(f) >= c ) 
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
    float yi = 0.0;
    int N = 0;

    while (N < 6)
    {
        float t = 0.0;

        while (t < 100.0)
        {
            float e = (float)1.0 / (N + 2.0);
            auto sim = ball(xi, t, e, yi);

            auto c = - std::get<1>(sim) * std::sin(alfa);

            if (std::get<2>(sim) >= c ) 
            {
                x.push_back(std::get<1>(sim));
                y.push_back(std::get<2>(sim));
            }
            t = t + dt;
            if (std::get<0>(sim) == false)
            {

                N += 1;
                yi =  std::get<2>(sim) * std::cos(alfa);
                xi = std::get<1>(sim) - c + yi;
                t = 110.0;
            }
        }
    }
    return std::make_tuple(x,y);
}

int main(int argc, char const *argv[])
{

	int w = 1280;
	int h = 720;
	float scale_x = 0.5;
	float scale_y = 2.0;
	std::string title = "Bouncing ball";
	initImgui(w, h, title);

	// simulation state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(89.0f / 255.0, 88.0f / 255.0, 87.0f / 255.0, 1.00f);
	ImVec4 ray_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

	// compute ball path
	auto sim = simulation();

	// Main loop
	int ii = 0;
	bool flag = true;
	std::vector<ImVec2> tail;

	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		//----------------------------------------

		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();


		ImVec2 ball = {(float)points[0].position.x + std::get<0>(sim)[ii] * scale_x, (float)points[0].position.y - std::get<1>(sim)[ii] * scale_y};


		float radius = 5;
		float size_line = 3;

		draw_list->AddCircleFilled(ball, radius, ImColor(ray_color));

		tail.push_back(ball);


		for (auto &ii : tail)
		{

			draw_list->AddCircleFilled(ii, 0.5, ImColor(ray_color));
		}

		ii++;
		if (ii >= std::get<0>(sim).size())
		{
			//flag = false;
		}

		ImGui::End();
		//----------------------------------------

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}
