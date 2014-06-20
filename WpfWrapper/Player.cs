using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WpfWrapper
{
	public class Player : INotifyPropertyChanged
	{
		public Double PosX { get; set; }
		public Double PosY { get; set; }
		public Double PosZ { get; set; }
		public string Name { get; set; }
		public int ClientID { get; set; }

		public Player(MineServer.Player ori)
		{
			PosX = ori.PosX;
			ori.NotifyXChange += ori_NotifyXChange;

			PosY = ori.PosYFeet;
			ori.NotifyYChange += ori_NotifyYChange;

			PosZ = ori.PosZ;
			ori.NotifyZChange += ori_NotifyZChange;

			Name = ori.Name;
			ClientID = (int)ori.GetClientID();
		}

		private void ori_NotifyZChange(double obj)
		{
			PosZ = obj;
			OnPropertyChanged("PosZ");
		}

		private void ori_NotifyYChange(double obj)
		{
			PosY = obj;
			OnPropertyChanged("PosY");
		}

		void ori_NotifyXChange(double obj)
		{
			PosX = obj;

			OnPropertyChanged("PosX");
		}

		public event PropertyChangedEventHandler PropertyChanged;

		protected void OnPropertyChanged(string property)
		{
			PropertyChangedEventHandler handler = PropertyChanged;
			if (handler != null)
				handler(this, new PropertyChangedEventArgs(property));
		}
	}
}
