using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	class MineServer
	{
		static void Main(string[] args)
		{
			Server serv = new Server();
			Core core = new Core(serv, World.EWorldType.CREATIVE);

			serv.Run();
		}
	}
}
