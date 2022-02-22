#include "app.hpp"

namespace app
{

app::app(const dim_t &size)
	: size(size)
{
	window = SDL_CreateWindow("zen",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size.x,
		size.y,
		SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	init_imgui();

	running = window && renderer;
	set_pixel_size({ 4, 4 });
	max_iterations = 100;
	zoom = 0.01;

	frame_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, vsize.x, vsize.y);

	// Set fractal to render
	fractal = FractalId_Mandelbrot;
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

		ImGui::Begin("Fractal View");
		{
			const auto view_size = ImGui::GetWindowSize();
			const auto scale = (view_size.y - 40.0f) / vsize.y;
			ImGui::Image((void *)frame_buffer, { vsize.x * scale, vsize.y * scale });
		}
		ImGui::End();

		ImGui::Begin("zen");
		{
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

	SDL_SetRenderTarget(renderer, frame_buffer);
	for (int y = 0; y < vsize.y; ++y)
	{
		for (int x = 0; x < vsize.x; ++x)
		{
			const auto start = zen::complex(x - vsize.x / 2.0, y - vsize.y / 2.0) * zoom;
			const size_t iterations = iter_fractal(start);
			if (iterations == max_iterations)
			{
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
			}

			SDL_RenderDrawPoint(renderer, x, y);
		}
	}
	SDL_SetRenderTarget(renderer, nullptr);
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

			default:
				break;
		}
	}
}

auto app::set_pixel_size(const dim_t &pixel) -> void
{
	this->pixel = pixel;
	vsize = { size.x / pixel.x, size.y / pixel.y };
}

}
