#include "app/app.hpp"

auto main() -> int
{
	app::app app({ 1280, 720 });
	app.run();

	return 0;
}
