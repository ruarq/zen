#include "App/App.hpp"

auto main() -> int
{
	App::App app({ 1280, 720 });
	app.Run();

	return 0;
}
