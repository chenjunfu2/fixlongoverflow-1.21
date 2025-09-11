#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <bit>

class Double
{
public:
	static constexpr int64_t doubleToRawLongBits(double d)
	{
		static_assert(sizeof(d) == sizeof(int64_t), "error");
		return std::bit_cast<int64_t>(d);
	}
};

class Math
{
public:
	static constexpr int64_t negativeZeroDoubleBits = Double::doubleToRawLongBits(-0.0f);

	static int32_t floor(double value)
	{
		int32_t i = (int32_t)value;
		return value < i ? i - 1 : i;
	}

	static double min(double a, double b)
	{
		if (a != a)
			return a;   // a is NaN
		if ((a == 0.0f) &&
			(b == 0.0f) &&
			(Double::doubleToRawLongBits(b) == negativeZeroDoubleBits))
		{
			// Raw conversion ok since NaN can't map to -0.0.
			return b;
		}
		return (a <= b) ? a : b;
	}

	static double max(double a, double b)
	{
		if (a != a)
			return a;   // a is NaN
		if ((a == 0.0f) &&
			(b == 0.0f) &&
			(Double::doubleToRawLongBits(a) == negativeZeroDoubleBits))
		{
			// Raw conversion ok since NaN can't map to -0.0.
			return b;
		}
		return (a >= b) ? a : b;
	}
};


class Box
{
public:
	double minX;
	double minY;
	double minZ;
	double maxX;
	double maxY;
	double maxZ;

public:
	Box(double x1, double y1, double z1, double x2, double y2, double z2)
	{
		minX = Math::min(x1, x2);
		minY = Math::min(y1, y2);
		minZ = Math::min(z1, z2);
		maxX = Math::max(x1, x2);
		maxY = Math::max(y1, y2);
		maxZ = Math::max(z1, z2);
	}

	~Box(void) = default;
};
		

class ChunkSectionPos
{
public:
	static int32_t getSectionCoord(double coord)
	{
		return getSectionCoord(Math::floor(coord));
	}

	static int32_t getSectionCoord(int32_t coord)
	{
		return coord / 16;//Java是算术位移所以可以coord >> 4，而cpp不能保证，必须强制使用除法，然后触发编译器优化使得编译器自行使用算术位移
	}

	static int64_t asLong(int32_t x, int32_t y, int32_t z)
	{
		int64_t l = 0LL;
		l |= ((int64_t)x & 4194303LL) << 42;
		l |= ((int64_t)y & 1048575LL) << 0;
		return l | ((int64_t)z & 4194303LL) << 20;
	}
};


void subSet(int64_t fromElement, int64_t toElement)
{
	if (fromElement > toElement)
	{
		printf("IllegalArgumentException: Start element (%lld) is larger than end element (%lld)\n", fromElement, toElement);
		exit(-1);
	}
}


void Test(const Box &box)
{
	//int32_t i = 2;
	int32_t j = ChunkSectionPos::getSectionCoord(box.minX - 2.0);
	//int32_t k = ChunkSectionPos::getSectionCoord(box.minY - 4.0);
	//int32_t l = ChunkSectionPos::getSectionCoord(box.minZ - 2.0);
	int32_t m = ChunkSectionPos::getSectionCoord(box.maxX + 2.0);
	//int32_t n = ChunkSectionPos::getSectionCoord(box.maxY + 0.0);
	//int32_t o = ChunkSectionPos::getSectionCoord(box.maxZ + 2.0);

	for (int32_t p = j; p <= m; p++)
	{
		int64_t q = ChunkSectionPos::asLong(p, 0, 0);
		int64_t r = ChunkSectionPos::asLong(p, -1, -1);

		subSet(q, r + 1LL);
	}
}

int main(void)
{
	double testval = 33554418.0;//33554418.0 ~ 33554433.9之间的取值都会崩
	Test(Box{ testval,0,0,testval,0,0 });

	return 0;
}