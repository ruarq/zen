#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
#include "../imgui/imgui_impl_sdlrenderer.h"

#include "Complex.hpp"
#include "Fractals.hpp"
#include "Canvas.hpp"

#define ZEN_UNUSED [[maybe_unused]]

namespace Zen
{

template<typename T>
struct BasicVec2
{
	using Value_t = T;

	T x = 0, y = 0;
};

using Vec2 = BasicVec2<int>;
using Vec2f = BasicVec2<double>;

class App
{
public:
	virtual ~App();

public:
	void Init(const Vec2 &size);
	void Run();

protected:
	virtual void OnInit() {}
	virtual void OnUpdate() {}
	virtual void OnEvent() {}

private:
	void InitImGui();
	void QuitImGui();
	void DrawImGui();
	void EnableDockSpace();

	void DrawFractal();

	void HandleEvents();

protected:
	std::string appName;
	Canvas *canvas;
	bool useDockSpace = true;
	bool running;

	bool leftMouseDown;
	bool rightMouseDown;
	Vec2 mousePos;
	Vec2f mouseDelta;
	int mouseWheel; // 1 = up, 0 = none, -1 = down

private:
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	Vec2f canvasTopLeft; // top left position of where the canvas starts in screen space
};

}
