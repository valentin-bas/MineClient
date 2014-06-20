#include "stdafx.h"
#include "Cube.h"

vox::Cube::Cube(void) //:
{
	active = true;
	type = 0;
}

vox::Cube::Cube(vox::Cube const& right) //:
{
	(void)right;
}

vox::Cube::~Cube(void)
{
	this->__destroy();
}

bool
vox::Cube::__destroy(void)
{
	return true;
}

vox::Cube&
vox::Cube::operator=(const vox::Cube& right)
{
	return *this;
	(void)right;
}

bool
vox::Cube::isActive(void)
{
	return active;
}

void
vox::Cube::setIsActive(bool ac)
{
	active = ac;
}
