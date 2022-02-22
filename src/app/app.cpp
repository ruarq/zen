#include "app.hpp"

namespace app
{

app::app(const vec2 &size)
	: size(size)
{
	window = SDL_CreateWindow("zen",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size.x,
		size.y,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	init_imgui();

	running = window && renderer;
	max_iterations = 64;
	zoom = 100.0;

	resize_frame_buffer({ 1280, 720 });
	camera.x = (-fractal_view.w / 2.0f) / zoom;
	camera.y = (-fractal_view.h / 2.0f) / zoom;

	// Set fractal to render
	fractal = FractalId_Mandelbrot;

	for (int i = 0; i < 244; ++i)
	{
		const float factor = i / 243.0f;
		color_palette.push_back({ (uint8_t)(255 * factor), (uint8_t)(155 * factor), (uint8_t)(55 * factor), 255 });
	}
	color_palette.push_back({ 0, 0, 0, 255 });
}

app::~app()
{
	SDL_DestroyTexture(frame_buffer);

	quit_imgui();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

auto app::init_imgui() -> void
{
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	// Enable docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

auto app::quit_imgui() -> void
{
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

auto app::run() -> void
{
	while (running)
	{
		SDL_RenderClear(renderer);

		draw_fractal();
		draw_imgui();

		
		SDL_RenderPresent(renderer);
		handle_events();
	}
}

auto app::draw_imgui() -> void
{
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	{
		enable_dockspace();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Fractal View");
		{
			const auto content_region_min = ImGui::GetWindowContentRegionMin();
			const auto window_pos = ImGui::GetWindowPos();
			fractal_view.x = window_pos.x + content_region_min.x;
			fractal_view.y = window_pos.y + content_region_min.y;

			const auto view_size = ImGui::GetContentRegionAvail();
			if (view_size.x != fractal_view.w || view_size.y != fractal_view.h)
			{
				resize_frame_buffer({ (int)view_size.x, (int)view_size.y });
			}

			ImGui::Image((void *)frame_buffer, view_size);
		}
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("zen");
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

			ImGui::SliderInt("Iterations", (int *)&max_iterations, 1, 1 << 11);
			ImGui::Text("Average %.3f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
}

auto app::enable_dockspace() -> void
{
	const ImGuiViewport *viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	const auto window_flags = ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoBringToFrontOnFocus
				| ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar(2);

	ImGui::DockSpace(ImGui::GetID("MyDockSpace"));
	ImGui::End();
}

auto app::draw_fractal() -> void
{
	auto iter_fractal = [&](const auto &complex) {
		switch (fractal)
		{
			case FractalId_Mandelbrot: return zen::fractals::mandelbrot::iter(complex, max_iterations);
			case FractalId_Octopus: return zen::fractals::octopus::iter(complex, max_iterations);
			case FractalId_Custom: return 0ul;
			default: return zen::fractals::mandelbrot::iter(complex, max_iterations);
		}
	};

	static std::vector<uint8_t> pixels(fractal_view.w * fractal_view.h * 4);
	for (int y = 0; y < fractal_view.h; ++y)
	{
		for (int x = 0; x < fractal_view.w; ++x)
		{
			const auto pos = screen_to_world({ x, y });
			const auto start = zen::complex(pos.x, pos.y);

			const auto iterations = iter_fractal(start);

			const auto color = color_palette[(iterations / (float)max_iterations) * (color_palette.size() - 1)];

			const size_t index = (y * fractal_view.w + x) * 4;
			pixels[index + 0] = color.r;
			pixels[index + 1] = color.g;
			pixels[index + 2] = color.b;
			pixels[index + 3] = color.a;
		}
	}

	SDL_UpdateTexture(frame_buffer, nullptr, pixels.data(), fractal_view.w * 4);
}

auto app::handle_events() -> void
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type)
		{
			case SDL_QUIT:
				running = false;
				break;

			// pan and zoom
			case SDL_MOUSEMOTION:
			{
				vec2 mouse_pos;
				SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

				// only zoom if mouse is in viewport
				if (mouse_pos.x >= fractal_view.x && mouse_pos.y >= fractal_view.y
					&& mouse_pos.x <= fractal_view.x + fractal_view.w
					&& mouse_pos.y <= fractal_view.y + fractal_view.h)
				{
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						camera.x -= (float)event.motion.xrel / zoom;
						camera.y -= (float)event.motion.yrel / zoom;
					}
				}
			}
			break;

			case SDL_MOUSEWHEEL:
			{
				vec2 mouse_pos;
				SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

				// only zoom if mouse is in viewport
				if (mouse_pos.x >= fractal_view.x && mouse_pos.y >= fractal_view.y
					&& mouse_pos.x <= fractal_view.x + fractal_view.w
					&& mouse_pos.y <= fractal_view.y + fractal_view.h)
				{
					mouse_pos.x -= fractal_view.x;
					mouse_pos.y -= fractal_view.y;

					const auto mouse_before_zoom = screen_to_world(mouse_pos);
					if (event.wheel.y > 0)
					{
						zoom *= 1.1f;
					}
					else
					{
						zoom *= 0.9f;
					}

					const auto mouse_after_zoom = screen_to_world(mouse_pos);
					camera.x += (mouse_before_zoom.x - mouse_after_zoom.x);
					camera.y += (mouse_before_zoom.y - mouse_after_zoom.y);
				}
			}
			break;

			default:
				break;
		}
	}
}

auto app::resize_frame_buffer(const vec2 &size) -> void
{
	SDL_DestroyTexture(frame_buffer);
	frame_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
	fractal_view.w = size.x;
	fractal_view.h = size.y;
}

auto app::world_to_screen(const vec2f &world_coord) -> vec2
{
	return vec2 {
		(vec2::value_t)((world_coord.x - camera.x) * zoom),
		(vec2::value_t)((world_coord.y - camera.y) * zoom)
	};
}

auto app::screen_to_world(const vec2 &screen_coord) -> vec2f
{
	return vec2f {
		screen_coord.x / zoom + camera.x,
		screen_coord.y / zoom + camera.y
	};
}

}
