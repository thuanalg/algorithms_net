#ifndef __AAAAAAAAAAA__
#define __AAAAAAAAAAA__
#include <type_traits> //C++11
#include <iostream> //C++98
#include <concepts> //C++20
class AAAAAAAAAAA_abstract_log
{
      public:
	AAAAAAAAAAA_abstract_log()
	{
		//std::cout << "AAAAAAAAAAA_abstract-----------------\n";
	}
	virtual ~AAAAAAAAAAA_abstract_log()
	{
		//std::cout << "~AAAAAAAAAAA_abstract-----------------\n";
	}
};

typedef struct __TTTTESSSS__ {
	int b;
} __TTTTESSSS__;
template <typename T>
	requires std::totally_ordered<T> || std::same_as<T, double> ||
		 std::same_as<T, __TTTTESSSS__>
class AAAAAAAAAAA : virtual public AAAAAAAAAAA_abstract_log
{
private:
	T x1, x2;

protected:


      
public:
	AAAAAAAAAAA() : x1(0), x2(0)
	{
		int a = 0;
		if (std::same_as<T, double>) {
			std::cout << "-----------------\n";
		}
	}
	AAAAAAAAAAA(T a, T b) : x1(a), x2(b)
	{
		int bwew = 0;
		if (std::same_as<T, double>) {
			std::cout << "-----------------\n";
		} 
		// std::cout << "dsds\n";
	}
	virtual ~AAAAAAAAAAA()
	{
		//std::cout << "~AAAAAAAAAAA\n";
	};

	virtual bool operator==(const AAAAAAAAAAA &other) const
	{
		return x1 == other.x1 && x2 == other.x2;
	}

	virtual bool operator!=(const AAAAAAAAAAA &other) const
	{
		return !(*this == other);
	}

	virtual bool operator<(const AAAAAAAAAAA &other) const
	{
		return (x1 < other.x1) || (x1 == other.x1 && x2 < other.x2);
	}
	virtual bool operator<=(const AAAAAAAAAAA &other) const
	{
		return (x1 <= other.x1) || (x1 == other.x1 && x2 <= other.x2);
	}
	virtual bool operator>(const AAAAAAAAAAA &other) const
	{
		return other < *this;
	}
	virtual bool operator>=(const AAAAAAAAAAA &other) const
	{
		return (x1 >= other.x1) || (x1 == other.x1 && x2 >= other.x2);
	}
	virtual bool operator!() const
	{
		return !!x1  && !!x2;
	}
	AAAAAAAAAAA operator+(const AAAAAAAAAAA &other) const
		requires std::is_arithmetic_v<T>
	{
		return AAAAAAAAAAA(x1 + other.x1, x2 + other.x2);
	}
	AAAAAAAAAAA &operator+=(const AAAAAAAAAAA &other)
		requires std::is_arithmetic_v<T>
	{
		x1 += other.x1;
		x2 += other.x2;
		return *this;
	}
	AAAAAAAAAAA operator-(const AAAAAAAAAAA &other) const
		requires std::is_arithmetic_v<T>
	{
		return AAAAAAAAAAA(x1 - other.x1, x2 - other.x2);
	}

	AAAAAAAAAAA &operator-=(const AAAAAAAAAAA &other)
		requires std::is_arithmetic_v<T>
	{
		x1 -= other.x1;
		x2 -= other.x2;
		return *this;
	}

	AAAAAAAAAAA operator*(const T &scalar) const
		requires std::is_arithmetic_v<T>
	{
		return AAAAAAAAAAA(x1 * scalar, x2 * scalar);
	}

	AAAAAAAAAAA &operator*=(const AAAAAAAAAAA &other)
		requires std::is_arithmetic_v<T>
	{
		x1 *= other.x1;
		x2 *= other.x2;
		return *this;
	}

	AAAAAAAAAAA operator/(const T &scalar) const
		requires std::is_arithmetic_v<T>
	{
		return AAAAAAAAAAA(x1 / scalar, x2 / scalar);
	}
	AAAAAAAAAAA &operator/=(const AAAAAAAAAAA &other)
		requires std::is_arithmetic_v<T>
	{
		x1 /= other.x1;
		x2 /= other.x2;
		return *this;
	}
	AAAAAAAAAAA &operator++()
		requires std::is_arithmetic_v<T>
	{
		++x1;
		++x2;
		return *this;
	}

	AAAAAAAAAAA &operator--()
		requires std::is_arithmetic_v<T>
	{
		--x1;
		--x2;
		return *this;
	}

	AAAAAAAAAAA operator++(int)
		requires std::is_arithmetic_v<T>
	{
		AAAAAAAAAAA temp = *this;
		++(*this); //
		return temp;
	}

	AAAAAAAAAAA operator--(int) //
		requires std::is_arithmetic_v<T>
	{
		AAAAAAAAAAA temp = *this;
		--(*this); //
		return temp;
	}

	AAAAAAAAAAA &operator=(const AAAAAAAAAAA &other)
	{
		x1 = other.x1;
		x2 = other.x2;
		return *this;
	}
	friend std::ostream &operator<<(
	    std::ostream &os, const AAAAAAAAAAA &obj)
	{
		os << "(" << obj.x1 << ", " << obj.x2 << ")";
		return os;
	}
	virtual void print() const
	{
		std::cout << "\n (" << x1 << ", " << x2 << ")\n";
	}

      


protected:

};
#endif // __AAAAAAAAAAA__
