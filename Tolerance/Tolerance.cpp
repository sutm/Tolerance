// Tolerance.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <assert.h>
#include <type_traits>

using namespace std;

template<typename Derived>
struct NotEqualMixin {};

template<typename Derived>
bool operator!=(const NotEqualMixin<Derived>& lhs,
	const NotEqualMixin<Derived>& rhs)
{
	static_assert(is_base_of<NotEqualMixin<Derived>, Derived>::value,
		"Detected misuse of NotEqualMixin");
	auto derivedLhs = static_cast<const Derived&>(lhs);
	auto derivedRhs = static_cast<const Derived&>(rhs);
	return !(derivedLhs == derivedRhs);  // call Derived's operator==
}

template<typename Derived>
struct MinTol {};

template<typename Derived>
struct MaxTol {};

template<typename Derived>
struct MinMaxTol {};


template< template<typename> class... Mixins>
struct Gadget : public Mixins<Gadget<Mixins...>>...
{
	Gadget(int i) : x(i) {};

	bool operator==(const Gadget& rhs) const
	{
		return x == rhs.x;
	}
	int x;
};

using GadgetEx = Gadget<>;
using GadgetWithNotEqual = Gadget<NotEqualMixin, MinTol>;

int main()
{
	GadgetEx x{ 1 };
	GadgetEx y{ 2 };
	//assert(x != y); // does not compile

	GadgetWithNotEqual a{ 1 };
	GadgetWithNotEqual b{ 2 };
	assert(a != b);

	return 0;
}