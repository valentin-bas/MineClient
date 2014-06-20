#include "stdafx.h"
#include "Vector2Int.h"

using namespace vox;

Vector2Int::Vector2Int(int x, int y)
{
	this->x = x;
	this->y = y;
}


Vector2Int::~Vector2Int(void)
{
}

bool
Vector2Int::operator<(const Vector2Int& oth) const
{
	int myHash = 23;
	myHash = myHash * 31 + x;
	myHash = myHash * 31 + y;
	int othHash = 23;
	othHash = othHash * 31 + oth.x;
	othHash = othHash * 31 + oth.y;
	if (myHash < othHash)
		return true;
	return false;
}