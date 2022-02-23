#pragma once

#include <cstdint>

#include "Complex.hpp"

namespace Zen::Fractals
{

#define CREATE_SET_BY_EXPR(name, expr_) \
	namespace name { \
		static constexpr auto expr = #expr_; \
		template<ComplexType TComplex> \
		auto Iter(const TComplex &start, const size_t max_iter) -> size_t \
		{ \
			const auto c = start; \
			auto z = start; \
			for (size_t i = 0; i < max_iter; ++i) \
			{ \
				z = expr_; \
				if (AbsSq(z) > 4.0) \
				{ \
					return i; \
				} \
			} \
			return max_iter; \
		} \
	}

CREATE_SET_BY_EXPR(Mandelbrot, z * z + c);

// Some sets I found by myself (they obviously probably have already been found, but I gave them my own names)

CREATE_SET_BY_EXPR(Octopus, (c + z) * z + z * z * z + c * z * z + z);

}
