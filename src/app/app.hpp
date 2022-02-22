#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
#include "../imgui/imgui_impl_sdlrenderer.h"

#include "../zen/complex.hpp"
#include "../zen/runtime.hpp"
#include "../zen/fractals.hpp"

namespace app
{

enum FractalId : int
{
	FractalId_Mandelbrot,
	FractalId_Octopus,
	FractalId_Custom
};

template<typename T>
struct basic_vec2
{
	using value_t = T;

	T x = 0, y = 0;
};

using vec2 = basic_vec2<int>;
using vec2f = basic_vec2<double>;

struct app
{
public:
	app(const vec2 &size);
	~app();

public:
	auto run() -> void;

private:
	auto init_imgui() -> void;
	auto quit_imgui() -> void;
	auto draw_imgui() -> void;
	auto enable_dockspace() -> void;

	auto draw_fractal() -> void;

	auto handle_events() -> void;

	auto resize_frame_buffer(const vec2 &size) -> void;

	auto world_to_screen(const vec2f &world_coord) -> vec2;
	auto screen_to_world(const vec2 &screen_coord) -> vec2f;

private:
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;

	SDL_Texture *frame_buffer = nullptr;

	vec2f camera;
	const vec2 size;
	SDL_Rect fractal_view;

	bool running;

	size_t max_iterations;
	double zoom;

	FractalId fractal;
	std::vector<SDL_Color> color_palette;
};

}
