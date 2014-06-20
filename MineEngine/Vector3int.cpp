#include "stdafx.h"
#include "Vector3Int.h"

using namespace vox;

Vector3Int::Vector3Int(int x, int y, int z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}


Vector3Int::~Vector3Int(void)
{
}

bool
Vector3Int::operator<(const Vector3Int& oth) const
{
	int myHash = 23;
	myHash = myHash * 31 + x;
	myHash = myHash * 31 + y;
	myHash = myHash * 31 + z;
	int othHash = 23;
	othHash = othHash * 31 + oth.x;
	othHash = othHash * 31 + oth.y;
	othHash = othHash * 31 + oth.z;
	if (myHash < othHash)
		return true;
	return false;
}

Vector3Int&
Vector3Int::operator=(const Vector3Int& oth)
{
	this->x = oth.x;
	this->y = oth.y;
	this->z = oth.z;
	return (*this);
}
/*
std::ostream&
operator<<(std::ostream& os, const Vector3Int& oth)
{

}*/