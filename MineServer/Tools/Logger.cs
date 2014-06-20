using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MineServer
{
	public class Logger
	{
		private static Logger instance;
		private Logger() { _value = ""; }

		public static Logger Instance
		{
		   get 
		   {
		      if (instance == null)
		      {
				  instance = new Logger();
		      }
		      return instance;
		   }
		}

		public event Action NotifyValueChanged;

		String _value;
		public String Value { get { return _value; } set { _value = value; } }

		public void Log(String str)
		{
			_value = DateTime.Now.ToString("<hh:mm:ss>") + str + "\n";
			if (NotifyValueChanged != null)
				NotifyValueChanged();
			else
				Console.WriteLine(_value);
		}
	}
}
