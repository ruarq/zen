#pragma once

#include "App.hpp"

enum FractalId : int
{
	FractalId_Mandelbrot,
	FractalId_Octopus,
	FractalId_Custom
};

class FractalApp : public Zen::App
{
private:
	void OnInit() override
	{
		appName = "Fractals";

		maxIterations = 64;
		zoom = 100.0;
		fractal = FractalId_Mandelbrot;

		camera = {
			(-canvas->width / 2.0f) / zoom,
			(-canvas->height / 2.0f) / zoom
		};

		// Generate color palette
		for (int i = 255; i > 0; --i)
		{
			const float factor = i / 55.0f;
			colorPalette.push_back({
				(uint8_t)(factor * 255),
				(uint8_t)(factor * 100),
				(uint8_t)(factor * 50),
				255
			});
		}
	}

	void OnUpdate() override
	{
		ImGui::Begin("Properties");
		{
			ImGui::Text("Zoom %f", zoom);
			ImGui::Text("Camera (%f, %f)", camera.x, camera.y);
			
			ImGui::Text("Fractal");
			{
				ImGui::RadioButton("Mandelbrot", (int *)&fractal, FractalId_Mandelbrot);
				ImGui::RadioButton("Octopus", (int *)&fractal, FractalId_Octopus);
				ImGui::RadioButton("Custom", (int *)&fractal, FractalId_Custom);
				if (fractal == FractalId_Custom)
				{
					ImGui::Text("Not yet supported");
				}
			}

			ImGui::SliderInt("Iterations", (int *)&maxIterations, 1, 1 << 11);
			ImGui::Text("Average %.3f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();

		DrawFractal();
	}

	void OnEvent() override
	{
		HandlePanAndZoom();
	}

	void DrawFractal()
	{
		auto iter_fractal = [&](const auto &complex) {
			switch (fractal)
			{
				case FractalId_Mandelbrot: return Zen::Fractals::Mandelbrot::Iter(complex, maxIterations);
				case FractalId_Octopus: return Zen::Fractals::Octopus::Iter(complex, maxIterations);
				case FractalId_Custom: return 0ul;
				default: return Zen::Fractals::Mandelbrot::Iter(complex, maxIterations);
			}
		};

		for (int y = 0; y < canvas->height; ++y)
		{
			for (int x = 0; x < canvas->width; ++x)
			{
				const auto pos = ScreenToWorld({ x, y });
				const auto start = Zen::Complex64(pos.x, pos.y);

				const auto iterations = iter_fractal(start);
				const auto color = colorPalette[(iterations / (float)maxIterations) * (colorPalette.size() - 1)];
				canvas->DrawPoint(x, y, color);
			}
		}
	}

	void HandlePanAndZoom()
	{
		// only zoom or pan if mouse is in viewport
		if (mousePos.x >= 0
			&& mousePos.y >= 0
			&& mousePos.x <= canvas->width
			&& mousePos.y <= canvas->height)
		{
			// pan
			if (leftMouseDown)
			{
				camera.x -= mouseDelta.x / zoom;
				camera.y -= mouseDelta.y / zoom;
			}

			// zoom
			if (mouseWheel != 0)
			{
				const auto mouseBeforeZoom = ScreenToWorld(mousePos);

				if (mouseWheel > 0)
				{
					zoom *= 1.1f;
				}
				else
				{
					zoom *= 0.9f;
				}

				const auto mouseAfterZoom = ScreenToWorld(mousePos);
				camera.x += (mouseBeforeZoom.x - mouseAfterZoom.x);
				camera.y += (mouseBeforeZoom.y - mouseAfterZoom.y);
			}
		}
	}

	Zen::Vec2 WorldToScreen(const Zen::Vec2f &worldCoord) const
	{
		return Zen::Vec2 {
			(Zen::Vec2::Value_t)((worldCoord.x - camera.x) * zoom),
			(Zen::Vec2::Value_t)((worldCoord.y - camera.y) * zoom)
		};
	}

	Zen::Vec2f ScreenToWorld(const Zen::Vec2 &screenCoord) const
	{
		return Zen::Vec2f {
			screenCoord.x / zoom + camera.x,
			screenCoord.y / zoom + camera.y
		};
	}

private:
	Zen::Vec2f camera;
	SDL_Rect fractalView;

	size_t maxIterations;
	double zoom;

	FractalId fractal;
	std::vector<SDL_Color> colorPalette;
};
