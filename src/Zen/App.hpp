#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
#include "../imgui/imgui_impl_sdlrenderer.h"

#include "Complex.hpp"
#include "Fractals.hpp"

namespace Zen
{

enum FractalId : int
{
	FractalId_Mandelbrot,
	FractalId_Octopus,
	FractalId_Custom
};

template<typename T>
struct BasicVec2
{
	using Value_t = T;

	T x = 0, y = 0;
};

using Vec2 = BasicVec2<int>;
using Vec2f = BasicVec2<double>;

struct App
{
public:
	App(const Vec2 &size);
	~App();

public:
	auto Run() -> void;

private:
	auto InitImGui() -> void;
	auto QuitImGui() -> void;
	auto DrawImGui() -> void;
	auto EnableDockSpace() -> void;

	auto DrawFractal() -> void;

	auto HandleEvents() -> void;

	auto ResizeFrameBuffer(const Vec2 &size) -> void;

	auto WorldToScreen(const Vec2f &worldCoord) -> Vec2;
	auto ScreenToWorld(const Vec2 &screenCoord) -> Vec2f;

private:
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;

	SDL_Texture *frameBuffer = nullptr;

	Vec2f camera;
	const Vec2 size;
	SDL_Rect fractalView;

	bool running;

	size_t maxIterations;
	double zoom;

	FractalId fractal;
	std::vector<SDL_Color> colorPalette;
};

}
