using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Threading;

namespace WpfWrapper
{
	public class Logger : INotifyPropertyChanged
	{
		private string _text;
		private string _str;
		private MineServer.Logger _Log;
		private ScrollViewer _view;
		private Dispatcher _dispatcher;

		public string Text
		{
			get
			{
				return this._text;
			}
			private set
			{
				this._text += value;
				OnPropertyChanged("Text");
				if (_view != null)
					_view.ScrollToBottom();
			}
		}

		public Logger(MineServer.Logger ori, ScrollViewer view, Dispatcher dispatcher)
		{
			_Log = ori;
			this._text = "";
			ori.NotifyValueChanged += ValueChangedHandler;
			this.Text = "";
			_view = view;
			_dispatcher = dispatcher;
		}

		private void ValueChangedHandler()
		{
			_str += _Log.Value;
			_dispatcher.BeginInvoke(new Action(() =>
			{
				if (_str != "")
				{
					this.Text = _str;
					_str = "";
				}
			}), null);
		}

		private void OnPropertyChanged(string info)
		{
			PropertyChangedEventHandler handler = PropertyChanged;
			if (handler != null)
				handler(this, new PropertyChangedEventArgs(info));
		}

		public event PropertyChangedEventHandler PropertyChanged;
	}
}
