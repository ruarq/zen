#pragma once

#include <stdexcept>
#include <string>

#include "complex.hpp"

namespace zen
{

/**
 * ABSTRACT SYNTAX TREE
 */
namespace ast
{
	/**
	 * @brief ast node base class
	 */
	template<complex_type Complex>
	struct node
	{
	public:
		virtual ~node() = default;

	public:
		virtual auto eval() const -> Complex = 0;
	};

	template<complex_type Complex>
	struct binary_op final : public node<Complex>
	{
	public:
		~binary_op()
		{
			if (left)
			{
				delete left;
			}

			if (right)
			{
				delete right;
			}
		}

	public:
		enum class type
		{
			add,
			sub,
			mul
		};

	public:
		auto eval() const -> Complex
		{
			switch (op)
			{
				case type::add: return add<Complex>(left->eval(), right->eval());
				case type::sub: return sub<Complex>(left->eval(), right->eval());
				case type::mul: return mul<Complex>(left->eval(), right->eval());
				default: throw std::runtime_error("zen::binary_op<>::eval() : unknown op");
			}
		}

	public:
		node<Complex> *left, *right;
		type op;
	};

	template<complex_type Complex>
	struct constant final : public node<Complex>
	{
	public:
		auto eval() const -> Complex
		{
			return value;
		}

	public:
		Complex value;
	};
}

/**
 * RUNTIME
 * TODO(ruarq): We can make this faster. Reparsing the expression every time is stupid and hella slow.
 * Problem: We have to somehow inject the value into the leaf nodes.
 * Solutions:
 * - pointers
 * - new symbol node that looks up the symbol in some kind of symbol table
 */
template<complex_type Complex>
struct basic_runtime
{
public:
	static constexpr auto lookup_table_size = 'z' - 'a' + 1;

public:
	auto eval(const std::string &src) -> Complex
	{
		const auto ast = parse(src);
		if (ast)
		{
			const auto result = ast->eval();
			delete ast;
			return result;
		}
		
		return {};
	}

	auto set_value(const char alpha, const Complex &value)
	{
		lookup_table.at(make_index(alpha)) = std::move(value);
	}

	auto parse(const std::string &src) -> ast::node<Complex> *
	{
		this->src = src;
		curr = 0;

		if (!eof())
		{
			return expr();
		}
		
		return nullptr;
	}

public:
	auto expr() -> ast::node<Complex> *
	{
		return add_sub_op();
	}

	auto add_sub_op() -> ast::node<Complex> *
	{
		auto left = mul_op();

		while (!eof() && (peek_token() == '+' || peek_token() == '-'))
		{
			const auto op = lex_token() == '+'
				? ast::binary_op<Complex>::type::add
				: ast::binary_op<Complex>::type::sub;

			auto binary_op = new ast::binary_op<Complex>();
			binary_op->left = left;
			binary_op->op = op;
			binary_op->right = mul_op();
			left = binary_op;
		}

		return left;
	}

	auto mul_op() -> ast::node<Complex> *
	{
		auto left = terminal();

		while (!eof() && peek_token() == '*')
		{
			lex_token(); // *

			auto binary_op = new ast::binary_op<Complex>();
			binary_op->left = left;
			binary_op->op = ast::binary_op<Complex>::type::mul;
			binary_op->right = terminal();
			left = binary_op;
		}

		return left;
	}

	auto terminal() -> ast::node<Complex> *
	{
		switch (peek_token())
		{
			case 'a' ... 'z': return leaf();
			case '(':
			{
				lex_token(); // (
				auto expr = add_sub_op();
				const auto rparen = lex_token(); // )
				if (rparen != ')')
				{
					throw std::runtime_error("expected ')'");
				}
				return expr;
			}

			default:
				fmt::print("{}\n", lex_token());
				throw std::runtime_error("unexpected token");
				return nullptr; // because the throw should be removed later
		}
	}

	auto leaf() -> ast::node<Complex> *
	{
		auto node = new ast::constant<Complex>();
		node->value = lookup(lex_token());
		return node;
	}

	inline auto lookup(const char alpha)
	{
		return lookup_table.at(make_index(alpha));
	}

	inline auto make_index(const char alpha)
	{
		return alpha - 'a';
	}

	auto skip_whitespace()
	{
		auto is_whitespace = [](const auto c) {
			return c == ' ' || c == '\n' || c == '\t';
		};

		while (!eof() && is_whitespace(src[curr]))
		{
			++curr;
		}
	}

	auto peek_token()
	{
		skip_whitespace();
		return src[curr];
	}

	auto lex_token()
	{
		skip_whitespace();
		return src[curr++];
	}

	inline auto eof()
	{
		return curr >= src.size();
	}

private:
	std::string src;
	std::string::size_type curr;

	std::array<Complex, lookup_table_size> lookup_table;
};

using runtime = basic_runtime<complex>;
using runtime32 = basic_runtime<complex32>;
using runtime64 = basic_runtime<complex64>;
using runtime128 = basic_runtime<complex128>;
using runtime_mpz = basic_runtime<complex_mpz>;

}
