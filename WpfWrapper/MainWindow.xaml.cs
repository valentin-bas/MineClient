using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WpfWrapper
{
	/// <summary>
	/// Logique d'interaction pour MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		private MineServer.Server						_serv;
		public MineServer.Server						server { get { return _serv; } }
		private MineServer.Core							_core;
		private Logger									_console;
		public Logger									console { get { return _console; } }
		private Thread									_th;
		public ObservableCollection<Player>				players;
		private MineServer.World.EWorldType				worldType;
		public string									_port;
		public string	Port
		{
			get { return _port; }
			set { _port = value; }
		}

		public MainWindow()
		{
			InitializeComponent();
			Port = "1337";
			DataContext = this;
			this.Loaded += MainWindow_Loaded;
			this.Closed += MainWindow_Closed;
		}

		void MainWindow_Closed(object sender, System.EventArgs e)
		{
			_serv.Stop();
			_th.Join();
		}

		void MainWindow_Loaded(object sender, RoutedEventArgs e)
		{
			worldType = MineServer.World.EWorldType.CREATIVE;
			Radio_Creative.IsChecked = true;
			StartServer();	
		}

		void StartServer()
		{
			_serv = new MineServer.Server(Convert.ToInt32(Port));
			_core = new MineServer.Core(_serv, worldType);
			_console = new Logger(_core.Log, ScrollView, this.Dispatcher);
			Consoler.DataContext = console;
			_th = new Thread(new ThreadStart(_serv.Run));
			_th.Start();
			players = new ObservableCollection<Player>();
			_serv.world.NotifyPlayerRemove += worldNotifyRemove;
			_serv.world.NotifyPlayerAdd += worldNotifyAdd;
			listPlayer.ItemsSource = players;
		}

		void worldNotifyRemove(MineServer.Player obj)
		{
			this.Dispatcher.BeginInvoke(new Action(() => {
				foreach (Player player in players)
				{
					if (player.Name == obj.Name)
					{
						players.Remove(player);
						return;
					}
				}}), null);
		}

		void worldNotifyAdd(MineServer.Player obj)
		{
			this.Dispatcher.BeginInvoke(new Action(() => {
				Player tmp = new Player(obj);
				players.Add(tmp);
			}), null);
		}

		private void Kick(object sender, RoutedEventArgs e)
		{
			Button button = sender as Button;
			int tag = (int)button.Tag;

			_serv.KickClient((uint)tag);
		}

		private void RestartServ(object sender, RoutedEventArgs e)
		{
			Button button = sender as Button;
			_serv.Stop();
			_th.Join();
			StartServer();
		}

		private void Exit(object sender, RoutedEventArgs e)
		{
			Button button = sender as Button;

			this.Close();
		}

		private void SelectGameType(object sender, RoutedEventArgs e)
		{
			MenuItem item = sender as MenuItem;

			if (Radio_Survival != item)
				Radio_Survival.IsChecked = false;
			else
				worldType = MineServer.World.EWorldType.SURVIVAL;
			if (Radio_Creative != item)
				Radio_Creative.IsChecked = false;
			else
				worldType = MineServer.World.EWorldType.CREATIVE;
			if (Radio_Adventure != item)
				Radio_Adventure.IsChecked = false;
			else
				worldType = MineServer.World.EWorldType.ADVENTURE;
		}
	}
}
