#pragma once

#include <cstdint>

#include "complex.hpp"
#include "runtime.hpp"

namespace zen::fractals
{

namespace runtime
{
	template<complex_type Complex>
	auto iter(const std::string &expr, const Complex &start, const size_t max_iter) -> size_t
	{
		static zen::basic_runtime<Complex> static_runtime;

		auto z = start;
		static_runtime.set_value('c', start);

		for (size_t i = 0; i < max_iter; ++i)
		{
			static_runtime.set_value('z', z);
			z = static_runtime.eval(expr);

			if (abs_sq(z) > 4.0)
			{
				return i;
			}
		}

		return max_iter;
	}
}

#define CREATE_SET_BY_EXPR(name, expr_) \
	namespace name { \
		static constexpr auto expr = #expr_; \
		template<complex_type Complex> \
		auto iter(const Complex &start, const size_t max_iter) -> size_t \
		{ \
			const auto c = start; \
			auto z = start; \
			for (size_t i = 0; i < max_iter; ++i) \
			{ \
				z = expr_; \
				if (abs_sq(z) > 4.0) \
				{ \
					return i; \
				} \
			} \
			return max_iter; \
		} \
	}

CREATE_SET_BY_EXPR(mandelbrot, z * z + c);

// Some sets I found by myself (they obviously probably have already been found, but I gave them my own names)

CREATE_SET_BY_EXPR(octopus, (c + z) * z + z * z * z + c * z * z + z);
CREATE_SET_BY_EXPR(a, z * z * z * z + c);

}
