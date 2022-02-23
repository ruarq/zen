#pragma once

#include <vector>

#include <SDL2/SDL.h>

namespace Zen
{

struct Canvas
{
public:
	Canvas(SDL_Renderer *renderer);
	~Canvas();

public:
	void ResizeBuffer(const int width, const int height);
	void DrawPoint(const int x, const int y, const SDL_Color &color = { 255, 255, 255, 255 });
	void Render();

public:
	SDL_Texture *texture;
	int width, height;

private:
	SDL_Renderer *renderer;
	std::vector<SDL_Color> buffer;
};

}
