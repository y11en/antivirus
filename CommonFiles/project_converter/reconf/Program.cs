using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace reconf
{

  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    public static string[] g_args;

    [STAThread]
    static void Main(string[] args)
    {
      g_args = args;
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new ProjConf());
    }
  }
}