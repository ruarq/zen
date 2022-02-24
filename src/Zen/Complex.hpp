#pragma once

#include <ostream>
#include <cmath>

// only include fmt if it exists
#if __has_include(<fmt/ostream.h>)
	#include <fmt/ostream.h>
#endif

#if __has_include(<gmpxx.h>)
	#define ZEN_COMPLEX_HAS_GMP 1

	#include <gmpxx.h>
#else
	#define ZEN_COMPLEX_HAS_GMP 0
#endif

namespace Zen
{

template<typename TFloat>
class BasicComplex;

template<typename T>
struct IsComplex_Value
{
	static auto constexpr value = false;
};

template<typename T>
struct IsComplex_Value<BasicComplex<T>>
{
	static auto constexpr value = true;
};

template<typename T>
constexpr auto IsComplex = IsComplex_Value<T>::value;

template<typename T>
concept ComplexType = IsComplex<T>;

template<ComplexType TComplex>
constexpr auto operator==(const TComplex &lhs, const TComplex &rhs)
{
	return lhs.real == rhs.real && lhs.imag == rhs.imag;
}

template<ComplexType TComplex>
constexpr auto operator!=(const TComplex &lhs, const TComplex &rhs)
{
	return !(lhs == rhs);
}

/**
 * @brief Complex number addition
 */
template<ComplexType TComplex>
constexpr auto Add(const TComplex &lhs, const TComplex &rhs)
{
	return TComplex(lhs.real + rhs.real, lhs.imag + rhs.imag);
}

template<ComplexType TComplex>
constexpr auto Sub(const TComplex &lhs, const TComplex &rhs)
{
	return TComplex(lhs.real - rhs.real, lhs.imag - rhs.imag);
}

/**
 * @brief Complex number multiplication.
 */
template<ComplexType TComplex>
constexpr auto Mul(const TComplex &lhs, const TComplex &rhs)
{
	return TComplex(lhs.real * rhs.real - lhs.imag * rhs.imag, lhs.real * rhs.imag + lhs.imag * rhs.real);
}

/**
 * @brief Scale a complex number
 */
template<ComplexType TComplex, typename TScalar>
constexpr auto Mul(const TComplex &complex, const TScalar scalar)
{
	return TComplex(complex.real * scalar, complex.imag * scalar);
}

/**
 * @brief Alias for zen::add
 */
template<ComplexType TComplex>
constexpr auto operator+(const TComplex &lhs, const TComplex &rhs)
{
	return Add<TComplex>(lhs, rhs);
}

/**
 * @brief Alias for zen::sub
 */
template<ComplexType TComplex>
constexpr auto operator-(const TComplex &lhs, const TComplex &rhs)
{
	return Sub<TComplex>(lhs, rhs);
}

/**
 * @brief Alias for zen::mul
 */
template<ComplexType TComplex>
constexpr auto operator*(const TComplex &lhs, const TComplex &rhs)
{
	return Mul<TComplex>(lhs, rhs);
}

/**
 * @brief Alias for zen::mul
 */
template<ComplexType TComplex, typename TScalar>
constexpr auto operator*(const TComplex &complex, const TScalar scalar)
{
	return Mul<TComplex, TScalar>(complex, scalar);
}

/**
 * @brief Get the absolute value squared of a complex number
 */
template<ComplexType TComplex>
constexpr auto AbsSq(const TComplex &complex)
{
	return complex.real * complex.real + complex.imag * complex.imag;
}

/**
 * @brief Get the absolute value of a complex number
 */
template<ComplexType TComplex>
constexpr auto Abs(const TComplex &complex)
{
	return std::sqrt(AbsSq(complex));
}

template<typename TFloat>
struct BasicComplex
{
public:
	using TValue = TFloat;
	using TReal = TFloat;
	using TImag = TFloat;

public:
	/**
	 * @brief Default constructor, real=0, imag=0
	 */
	constexpr BasicComplex()
		: real((TReal)0.0)
		, imag((TImag)0.0)
	{
	}

	/**
	 * @brief Construct a complex number from real and imag
	 * @param real The real part of the complex number
	 * @param imag The imaginary part of the complex number
	 */
	constexpr BasicComplex(const TReal real, const TImag imag)
		: real(real)
		, imag(imag)
	{
	}

public:
	constexpr auto operator+=(const BasicComplex<TFloat> &other)
	{
		*this = Add(*this, other);
		return *this;
	}

	constexpr auto operator-=(const BasicComplex<TFloat> &other)
	{
		*this = Sub(*this, other);
		return *this;
	}

	constexpr auto operator*=(const BasicComplex<TFloat> &other)
	{
		*this = Mul(*this, other);
		return *this;
	}

public:
	TReal real;
	TImag imag;
};

template<typename TFloat>
std::ostream& operator<<(std::ostream &out, const BasicComplex<TFloat> &complex)
{
	out << "(" << complex.real << ", " << "" << complex.imag << "i)";
	return out;
}

using Complex32 = BasicComplex<float>;
using Complex64 = BasicComplex<double>;
using Complex128 = BasicComplex<__float128>;

#if ZEN_COMPLEX_HAS_GMP
using ComplexMpz  = BasicComplex<mpz_class>; // TODO(ruarq): Fix segmentation fault issue when using to draw fractals
#endif

using Complex = BasicComplex<double>;
	
}
