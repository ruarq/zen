#include "App.hpp"

using namespace std::string_literals;

namespace Zen
{

App::~App()
{
	delete canvas;

	QuitImGui();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void App::Init(const Vec2 &size)
{
	window = SDL_CreateWindow(("Zen: Unitialized"s).c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		size.x,
		size.y,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	InitImGui();
	running = window && renderer;

	canvas = new Canvas(renderer);
	canvas->ResizeBuffer(size.x, size.y);

	OnInit();

	SDL_SetWindowTitle(window, ("Zen: "s + (!appName.empty() ? appName : "An application powered by Zen"s)).c_str());
}

void App::InitImGui()
{
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	// Enable docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void App::QuitImGui()
{
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void App::Run()
{
	while (running)
	{
		// Clear window buffer
		SDL_RenderClear(renderer);

		// Start ImGui frame
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// enable dockspace
		if (useDockSpace)
		{
			EnableDockSpace();
		}

		// User defined update/render
		OnUpdate();
		canvas->Render();

		// Draw UI provided by the engine
		DrawImGui();

		// End ImGui frame
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		ImGui::UpdatePlatformWindows();

		// Update window
		SDL_RenderPresent(renderer);
		HandleEvents();
	}
}

void App::DrawImGui()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin(!appName.empty() ? appName.c_str() : "Canvas");
	{
		const auto contentRegionMin = ImGui::GetWindowContentRegionMin();
		const auto windowPos = ImGui::GetWindowPos();

		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		mousePos.x = (float)mouseX - contentRegionMin.x - windowPos.x;
		mousePos.y = (float)mouseY - contentRegionMin.y - windowPos.y;

		const auto viewSize = ImGui::GetContentRegionAvail();
		if (viewSize.x != canvas->width || viewSize.y != canvas->height)
		{
			canvas->ResizeBuffer((int)viewSize.x, (int)viewSize.y);
		}

		ImGui::Image((void *)canvas->texture, { (float)canvas->width, (float)canvas->height });
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void App::EnableDockSpace()
{
	const ImGuiViewport *viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar
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

void App::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		mouseDelta = Vec2f();
		mouseWheel = 0;

		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type)
		{
			case SDL_QUIT:
				running = false;
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					leftMouseDown = true;
				}
				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					rightMouseDown = true;
				}
				OnEvent();
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					leftMouseDown = false;
				}
				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					rightMouseDown = false;
				}
				OnEvent();
				break;

			case SDL_MOUSEMOTION:
				mouseDelta = { (Vec2f::Value_t)event.motion.xrel, (Vec2f::Value_t)event.motion.yrel };
				OnEvent();
				break;

			case SDL_MOUSEWHEEL:
				mouseWheel = event.wheel.y;
				OnEvent();
				break;

			default:
				break;
		}
	}
}

}
