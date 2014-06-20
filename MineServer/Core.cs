using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;

namespace MineServer
{
	public class Core
	{
		private World	_world;
		private Logger _log;
		public Logger Log { get { return _log; } }
		private Timer	_timer;
		private Server	_serv;
		private DateTime _lastTime;

		public Core(Server serv, World.EWorldType type)
		{
			_log = Logger.Instance;
			_world = new World(type);
			_timer = new Timer(50);
			_timer.Elapsed += callbackTimer;
			_timer.Enabled = true;
			_serv = serv;
			_serv.world = _world;
			_lastTime = DateTime.Now;
		}

		private void callbackTimer(object source, ElapsedEventArgs e)
		{
			TimeSpan delta = e.SignalTime - _lastTime;
			_lastTime = e.SignalTime;
			_world.Update(delta.Milliseconds / 1000.0f);
		}
	}
}
