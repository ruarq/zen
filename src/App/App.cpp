#include "App.hpp"

namespace App
{

App::App(const Vec2 &size)
	: size(size)
{
	window = SDL_CreateWindow("zen",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size.x,
		size.y,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	InitImGui();

	running = window && renderer;
	maxIterations = 64;
	zoom = 100.0;

	ResizeFrameBuffer({ 1280, 720 });
	camera.x = (-fractalView.w / 2.0f) / zoom;
	camera.y = (-fractalView.h / 2.0f) / zoom;

	// Set fractal to render
	fractal = FractalId_Mandelbrot;

	for (int i = 0; i < 244; ++i)
	{
		const float factor = i / 243.0f;
		colorPalette.push_back({ (uint8_t)(255 * factor), (uint8_t)(155 * factor), (uint8_t)(55 * factor), 255 });
	}
	colorPalette.push_back({ 0, 0, 0, 255 });
}

App::~App()
{
	SDL_DestroyTexture(frameBuffer);

	QuitImGui();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

auto App::InitImGui() -> void
{
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	// Enable docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

auto App::QuitImGui() -> void
{
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

auto App::Run() -> void
{
	while (running)
	{
		SDL_RenderClear(renderer);

		DrawFractal();
		DrawImGui();

		
		SDL_RenderPresent(renderer);
		HandleEvents();
	}
}

auto App::DrawImGui() -> void
{
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	{
		EnableDockSpace();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Fractal View");
		{
			const auto content_region_min = ImGui::GetWindowContentRegionMin();
			const auto window_pos = ImGui::GetWindowPos();
			fractalView.x = window_pos.x + content_region_min.x;
			fractalView.y = window_pos.y + content_region_min.y;

			const auto view_size = ImGui::GetContentRegionAvail();
			if (view_size.x != fractalView.w || view_size.y != fractalView.h)
			{
				ResizeFrameBuffer({ (int)view_size.x, (int)view_size.y });
			}

			ImGui::Image((void *)frameBuffer, view_size);
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

			ImGui::SliderInt("Iterations", (int *)&maxIterations, 1, 1 << 11);
			ImGui::Text("Average %.3f ms/frame (%.0f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
}

auto App::EnableDockSpace() -> void
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

auto App::DrawFractal() -> void
{
	auto iter_fractal = [&](const auto &complex) {
		switch (fractal)
		{
			case FractalId_Mandelbrot: return Zen::Fractals::mandelbrot::iter(complex, maxIterations);
			case FractalId_Octopus: return Zen::Fractals::octopus::iter(complex, maxIterations);
			case FractalId_Custom: return 0ul;
			default: return Zen::Fractals::mandelbrot::iter(complex, maxIterations);
		}
	};

	static std::vector<uint8_t> pixels(fractalView.w * fractalView.h * 4);
	for (int y = 0; y < fractalView.h; ++y)
	{
		for (int x = 0; x < fractalView.w; ++x)
		{
			const auto pos = ScreenToWorld({ x, y });
			const auto start = Zen::Complex(pos.x, pos.y);

			const auto iterations = iter_fractal(start);

			const auto color = colorPalette[(iterations / (float)maxIterations) * (colorPalette.size() - 1)];

			const size_t index = (y * fractalView.w + x) * 4;
			pixels[index + 0] = color.r;
			pixels[index + 1] = color.g;
			pixels[index + 2] = color.b;
			pixels[index + 3] = color.a;
		}
	}

	SDL_UpdateTexture(frameBuffer, nullptr, pixels.data(), fractalView.w * 4);
}

auto App::HandleEvents() -> void
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
				Vec2 mouse_pos;
				SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

				// only zoom if mouse is in viewport
				if (mouse_pos.x >= fractalView.x && mouse_pos.y >= fractalView.y
					&& mouse_pos.x <= fractalView.x + fractalView.w
					&& mouse_pos.y <= fractalView.y + fractalView.h)
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
				Vec2 mouse_pos;
				SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

				// only zoom if mouse is in viewport
				if (mouse_pos.x >= fractalView.x && mouse_pos.y >= fractalView.y
					&& mouse_pos.x <= fractalView.x + fractalView.w
					&& mouse_pos.y <= fractalView.y + fractalView.h)
				{
					mouse_pos.x -= fractalView.x;
					mouse_pos.y -= fractalView.y;

					const auto mouse_before_zoom = ScreenToWorld(mouse_pos);
					if (event.wheel.y > 0)
					{
						zoom *= 1.1f;
					}
					else
					{
						zoom *= 0.9f;
					}

					const auto mouse_after_zoom = ScreenToWorld(mouse_pos);
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

auto App::ResizeFrameBuffer(const Vec2 &size) -> void
{
	SDL_DestroyTexture(frameBuffer);
	frameBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
	fractalView.w = size.x;
	fractalView.h = size.y;
}

auto App::WorldToScreen(const Vec2f &world_coord) -> Vec2
{
	return Vec2 {
		(Vec2::Value_t)((world_coord.x - camera.x) * zoom),
		(Vec2::Value_t)((world_coord.y - camera.y) * zoom)
	};
}

auto App::ScreenToWorld(const Vec2 &screen_coord) -> Vec2f
{
	return Vec2f {
		screen_coord.x / zoom + camera.x,
		screen_coord.y / zoom + camera.y
	};
}

}