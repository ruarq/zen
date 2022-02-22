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

namespace zen
{

template<typename Float>
class basic_complex;

template<typename T>
struct is_complex
{
	static auto constexpr value = false;
};

template<typename T>
struct is_complex<basic_complex<T>>
{
	static auto constexpr value = true;
};

template<typename T>
constexpr auto is_complex_v = is_complex<T>::value;

template<typename T>
concept complex_type = is_complex_v<T>;

template<complex_type Complex>
constexpr auto operator==(const Complex &lhs, const Complex &rhs)
{
	return lhs.real == rhs.real && lhs.imag == rhs.imag;
}

template<complex_type Complex>
constexpr auto operator!=(const Complex &lhs, const Complex &rhs)
{
	return !(lhs == rhs);
}

/**
 * @brief Complex number addition
 */
template<complex_type Complex>
constexpr auto add(const Complex &lhs, const Complex &rhs)
{
	return Complex(lhs.real + rhs.real, lhs.imag + rhs.imag);
}

template<complex_type Complex>
constexpr auto sub(const Complex &lhs, const Complex &rhs)
{
	return Complex(lhs.real - rhs.real, lhs.imag - rhs.imag);
}

/**
 * @brief Complex number multiplication.
 */
template<complex_type Complex>
constexpr auto mul(const Complex &lhs, const Complex &rhs)
{
	return Complex(lhs.real * rhs.real - lhs.imag * rhs.imag, lhs.real * rhs.imag + rhs.real * lhs.imag);
}

/**
 * @brief Scale a complex number
 */
template<complex_type Complex, typename Scalar>
constexpr auto mul(const Complex &complex, const Scalar scalar)
{
	return Complex(complex.real * scalar, complex.imag * scalar);
}

/**
 * @brief Alias for zen::add
 */
template<complex_type Complex>
constexpr auto operator+(const Complex &lhs, const Complex &rhs)
{
	return add<Complex>(lhs, rhs);
}

/**
 * @brief Alias for zen::sub
 */
template<complex_type Complex>
constexpr auto operator-(const Complex &lhs, const Complex &rhs)
{
	return sub<Complex>(lhs, rhs);
}

/**
 * @brief Alias for zen::mul
 */
template<complex_type Complex>
constexpr auto operator*(const Complex &lhs, const Complex &rhs)
{
	return mul<Complex>(lhs, rhs);
}

/**
 * @brief Alias for zen::mul
 */
template<complex_type Complex, typename Scalar>
constexpr auto operator*(const Complex &complex, const Scalar scalar)
{
	return mul<Complex, Scalar>(complex, scalar);
}

/**
 * @brief Get the absolute value squared of a complex number
 */
template<complex_type Complex>
constexpr auto abs_sq(const Complex &complex)
{
	return complex.real * complex.real + complex.imag * complex.imag;
}

/**
 * @brief Get the absolute value of a complex number
 */
template<complex_type Complex>
constexpr auto abs(const Complex &complex)
{
	return std::sqrt(abs_sq(complex));
}

template<typename Float>
struct basic_complex
{
public:
	using value_t = Float;
	using real_t = Float;
	using imag_t = Float;

public:
	/**
	 * @brief Default constructor, real=0, imag=0
	 */
	constexpr basic_complex()
		: real(static_cast<real_t>(0.0))
		, imag(static_cast<imag_t>(0.0))
	{
	}

	/**
	 * @brief Construct a complex number from real and imag
	 * @param real The real part of the complex number
	 * @param imag The imaginary part of the complex number
	 */
	constexpr basic_complex(const real_t real, const imag_t imag)
		: real(real)
		, imag(imag)
	{
	}

public:
	constexpr auto operator+=(const basic_complex<Float> &other)
	{
		*this = add(*this, other);
		return *this;
	}

	constexpr auto operator-=(const basic_complex<Float> &other)
	{
		*this = sub(*this, other);
		return *this;
	}

	constexpr auto operator*=(const basic_complex<Float> &other)
	{
		*this = mul(*this, other);
		return *this;
	}

public:
	real_t real;
	imag_t imag;
};

template<typename Float>
auto operator<<(std::ostream &out, const basic_complex<Float> &complex) -> std::ostream &
{
	out << "(" << complex.real << ", " << "" << complex.imag << "i)";
	return out;
}

using complex32 = basic_complex<float>;
using complex64 = basic_complex<double>;
using complex128 = basic_complex<__float128>;

#if ZEN_COMPLEX_HAS_GMP
using complex_mpz  = basic_complex<mpz_class>;
#endif

using complex = basic_complex<double>;
	
}
