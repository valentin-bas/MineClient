#pragma once


namespace vox
{
	class Vector3Int
	{
	public:
		Vector3Int(int x = 0, int y = 0, int z = 0);
		~Vector3Int(void);

		bool		operator<(const Vector3Int& oth) const;
		Vector3Int&	operator=(const Vector3Int& oth);

		int x;
		int y;
		int z;

		friend std::ostream& operator<<(std::ostream& os, const Vector3Int& oth)
		{	
			os << oth.x << " " << oth.y << " " << oth.z << " ";
			return os;
		}
	};
}