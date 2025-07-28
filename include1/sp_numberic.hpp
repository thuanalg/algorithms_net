#ifndef __sp_numberic__
#define __sp_numberic__
#include <type_traits> //C++11
#include <iostream> //C++98
#include <concepts> //C++20
class SP_TRACKING_MEMORY
{
      public:
	SP_TRACKING_MEMORY()
	{
#ifdef __DEBUG__
		std::cout << "SP_TRACKING_MEMORY(): 0x" << this << std::endl;
#endif
	}
	virtual ~SP_TRACKING_MEMORY()
	{
#ifdef __DEBUG__
		std::cout << "~SP_TRACKING_MEMORY(): 0x" << this << std::endl;
#endif
	}
};

typedef struct __C20TESTNBR__ {
	int b;
} C20TESTNBR;

template <typename T>
	requires std::totally_ordered<T> || std::same_as<T, double> ||
		 std::same_as<T, C20TESTNBR>
class sp_numberic : virtual public SP_TRACKING_MEMORY
{
private:
	T x1, x2;

protected:


      
public:
	sp_numberic() : x1(0), x2(0)
	{
		int a = 0;
		if (std::same_as<T, double>) {
			std::cout << "-----------------\n";
		}
	}
	sp_numberic(T a, T b) : x1(a), x2(b)
	{
		int bwew = 0;
		if (std::same_as<T, double>) {
			std::cout << "-----------------\n";
		} 
		// std::cout << "dsds\n";
	}
	virtual ~sp_numberic()
	{
		//std::cout << "~sp_numberic\n";
	};

	virtual bool operator==(const sp_numberic &other) const
	{
		return x1 == other.x1 && x2 == other.x2;
	}

	virtual bool operator!=(const sp_numberic &other) const
	{
		return !(*this == other);
	}

	virtual bool operator<(const sp_numberic &other) const
	{
		return (x1 < other.x1) || (x1 == other.x1 && x2 < other.x2);
	}
	virtual bool operator<=(const sp_numberic &other) const
	{
		return (x1 <= other.x1) || (x1 == other.x1 && x2 <= other.x2);
	}
	virtual bool operator>(const sp_numberic &other) const
	{
		return other < *this;
	}
	virtual bool operator>=(const sp_numberic &other) const
	{
		return (x1 >= other.x1) || (x1 == other.x1 && x2 >= other.x2);
	}
	virtual bool operator!() const
	{
		return !!x1  && !!x2;
	}
	sp_numberic operator+(const sp_numberic &other) const
		requires std::is_arithmetic_v<T>
	{
		return sp_numberic(x1 + other.x1, x2 + other.x2);
	}
	sp_numberic &operator+=(const sp_numberic &other)
		requires std::is_arithmetic_v<T>
	{
		x1 += other.x1;
		x2 += other.x2;
		return *this;
	}
	sp_numberic operator-(const sp_numberic &other) const
		requires std::is_arithmetic_v<T>
	{
		return sp_numberic(x1 - other.x1, x2 - other.x2);
	}

	sp_numberic &operator-=(const sp_numberic &other)
		requires std::is_arithmetic_v<T>
	{
		x1 -= other.x1;
		x2 -= other.x2;
		return *this;
	}

	sp_numberic operator*(const T &scalar) const
		requires std::is_arithmetic_v<T>
	{
		return sp_numberic(x1 * scalar, x2 * scalar);
	}

	sp_numberic &operator*=(const sp_numberic &other)
		requires std::is_arithmetic_v<T>
	{
		x1 *= other.x1;
		x2 *= other.x2;
		return *this;
	}

	sp_numberic operator/(const T &scalar) const
		requires std::is_arithmetic_v<T>
	{
		return sp_numberic(x1 / scalar, x2 / scalar);
	}
	sp_numberic &operator/=(const sp_numberic &other)
		requires std::is_arithmetic_v<T>
	{
		x1 /= other.x1;
		x2 /= other.x2;
		return *this;
	}
	sp_numberic &operator++()
		requires std::is_arithmetic_v<T>
	{
		++x1;
		++x2;
		return *this;
	}

	sp_numberic &operator--()
		requires std::is_arithmetic_v<T>
	{
		--x1;
		--x2;
		return *this;
	}

	sp_numberic operator++(int)
		requires std::is_arithmetic_v<T>
	{
		sp_numberic temp = *this;
		++(*this); //
		return temp;
	}

	sp_numberic operator--(int) //
		requires std::is_arithmetic_v<T>
	{
		sp_numberic temp = *this;
		--(*this); //
		return temp;
	}

	sp_numberic &operator=(const sp_numberic &other)
	{
		x1 = other.x1;
		x2 = other.x2;
		return *this;
	}
	friend std::ostream &operator<<(
	    std::ostream &os, const sp_numberic &obj)
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
#endif // __sp_numberic__
