#include "Zen/FractalApp.hpp"

auto main() -> int
{
	Zen::App *app = new FractalApp();
	
	app->Init({ 1280, 720 });
	app->Run();

	delete app;

	return 0;
}
