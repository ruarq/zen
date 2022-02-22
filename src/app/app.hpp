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

struct dim_t
{
	int x, y;
};

struct app
{
public:
	app(const dim_t &size);
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

	auto set_pixel_size(const dim_t &pixel) -> void;

private:
	SDL_Window *window;
	SDL_Renderer *renderer;

	SDL_Texture *frame_buffer;

	const dim_t size;
	dim_t pixel;
	dim_t vsize; // virtual size
	bool running;

	size_t max_iterations;
	double zoom;

	FractalId fractal;
	static constexpr size_t fractal_input_buf_size = 255;
	char fractal_input_buf[fractal_input_buf_size] = { 0 };
};

}
