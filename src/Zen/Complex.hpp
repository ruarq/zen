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

template<typename Float>
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
constexpr auto operator==(const Complex &lhs, const Complex &rhs)
{
	return lhs.real == rhs.real && lhs.imag == rhs.imag;
}

template<ComplexType TComplex>
constexpr auto operator!=(const Complex &lhs, const Complex &rhs)
{
	return !(lhs == rhs);
}

/**
 * @brief Complex number addition
 */
template<ComplexType TComplex>
constexpr auto Add(const Complex &lhs, const Complex &rhs)
{
	return Complex(lhs.real + rhs.real, lhs.imag + rhs.imag);
}

template<ComplexType TComplex>
constexpr auto Sub(const Complex &lhs, const Complex &rhs)
{
	return Complex(lhs.real - rhs.real, lhs.imag - rhs.imag);
}

/**
 * @brief Complex number multiplication.
 */
template<ComplexType TComplex>
constexpr auto Mul(const Complex &lhs, const Complex &rhs)
{
	return Complex(lhs.real * rhs.real - lhs.imag * rhs.imag, lhs.real * rhs.imag + lhs.imag * rhs.real);
}

/**
 * @brief Scale a complex number
 */
template<ComplexType TComplex, typename Scalar>
constexpr auto Mul(const Complex &complex, const Scalar scalar)
{
	return Complex(complex.real * scalar, complex.imag * scalar);
}

/**
 * @brief Alias for zen::add
 */
template<ComplexType TComplex>
constexpr auto operator+(const Complex &lhs, const Complex &rhs)
{
	return Add<Complex>(lhs, rhs);
}

/**
 * @brief Alias for zen::sub
 */
template<ComplexType TComplex>
constexpr auto operator-(const Complex &lhs, const Complex &rhs)
{
	return Sub<Complex>(lhs, rhs);
}

/**
 * @brief Alias for zen::mul
 */
template<ComplexType TComplex>
constexpr auto operator*(const Complex &lhs, const Complex &rhs)
{
	return Mul<Complex>(lhs, rhs);
}

/**
 * @brief Alias for zen::mul
 */
template<ComplexType TComplex, typename Scalar>
constexpr auto operator*(const Complex &complex, const Scalar scalar)
{
	return Mul<Complex, Scalar>(complex, scalar);
}

/**
 * @brief Get the absolute value squared of a complex number
 */
template<ComplexType TComplex>
constexpr auto abs_sq(const Complex &complex)
{
	return complex.real * complex.real + complex.imag * complex.imag;
}

/**
 * @brief Get the absolute value of a complex number
 */
template<ComplexType TComplex>
constexpr auto abs(const Complex &complex)
{
	return std::sqrt(abs_sq(complex));
}

template<typename Float>
struct BasicComplex
{
public:
	using Value_t = Float;
	using Real_t = Float;
	using Imag_t = Float;

public:
	/**
	 * @brief Default constructor, real=0, imag=0
	 */
	constexpr BasicComplex()
		: real((Real_t)0.0)
		, imag((Imag_t)0.0)
	{
	}

	/**
	 * @brief Construct a complex number from real and imag
	 * @param real The real part of the complex number
	 * @param imag The imaginary part of the complex number
	 */
	constexpr BasicComplex(const Real_t real, const Imag_t imag)
		: real(real)
		, imag(imag)
	{
	}

public:
	constexpr auto operator+=(const BasicComplex<Float> &other)
	{
		*this = Add(*this, other);
		return *this;
	}

	constexpr auto operator-=(const BasicComplex<Float> &other)
	{
		*this = Sub(*this, other);
		return *this;
	}

	constexpr auto operator*=(const BasicComplex<Float> &other)
	{
		*this = Mul(*this, other);
		return *this;
	}

public:
	Real_t real;
	Imag_t imag;
};

template<typename Float>
auto operator<<(std::ostream &out, const BasicComplex<Float> &complex) -> std::ostream &
{
	out << "(" << complex.real << ", " << "" << complex.imag << "i)";
	return out;
}

using Complex32 = BasicComplex<float>;
using Complex64 = BasicComplex<double>;
using Complex128 = BasicComplex<__float128>;

#if ZEN_COMPLEX_HAS_GMP
using complex_mpz  = BasicComplex<mpz_class>;
#endif

using Complex = BasicComplex<double>;
	
}
