#pragma once

namespace vox
{
	class Vector2Int
	{
	public:
		Vector2Int(int x = 0, int y = 0);
		~Vector2Int(void);

		bool	operator<(const Vector2Int& oth) const;

		int x;
		int y;
	};
}