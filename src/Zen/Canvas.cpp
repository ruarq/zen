#include "Canvas.hpp"

namespace Zen
{

Canvas::Canvas(SDL_Renderer *renderer)
	: texture(nullptr)
	, renderer(renderer)
{
}

Canvas::~Canvas()
{
	SDL_DestroyTexture(texture);
}

void Canvas::ResizeBuffer(const int width, const int height)
{
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, width, height);

	buffer.resize(width * height * sizeof(SDL_Color));

	this->width = width;
	this->height = height;
}

void Canvas::DrawPoint(const int x, const int y, const SDL_Color &color)
{
	const size_t index = y * width + x;
	buffer[index] = color;
}

void Canvas::Render()
{
	SDL_UpdateTexture(texture, nullptr, buffer.data(), width * sizeof(SDL_Color));
}

}
