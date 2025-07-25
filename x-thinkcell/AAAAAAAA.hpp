#ifndef __AAAAAAAAAAA__
#define __AAAAAAAAAAA__
#include <type_traits>
#include <iostream>

template <typename T>
	requires std::is_arithmetic_v<T>
class AAAAAAAAAAA
{
      public:
	AAAAAAAAAAA() : x1(0), x2(0)
	{
		int a = 0;
		//std::cout << "ssssssssss";
	}
	AAAAAAAAAAA(T a, T b) : x1(a), x2(b)
	{
		int bwew = 0;
		//std::cout << "dsds\n";
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
		this->x1 += other.x1;
		this->x2 += other.x2;
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
		this->x1 -= other.x1;
		this->x2 -= other.x2;
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
		this->x1 *= other.x1;
		this->x2 *= other.x2;
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
		this->x1 /= other.x1;
		this->x2 /= other.x2;
		return *this;
	}
	AAAAAAAAAAA &operator++()
		requires std::is_arithmetic_v<T>
	{
		++this->x1;
		++this->x2;
		return *this;
	}

	AAAAAAAAAAA &operator--()
		requires std::is_arithmetic_v<T>
	{
		--this->x1;
		--this->x2;
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
		this->x1 = other.x1;
		this->x2 = other.x2;
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

      private:
	T x1, x2;
};
#endif // __AAAAAAAAAAA__
