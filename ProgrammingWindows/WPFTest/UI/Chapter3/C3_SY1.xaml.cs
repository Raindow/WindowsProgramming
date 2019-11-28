
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using dll_csharp;

namespace WPFTest.UI.Chapter3
{
    /// <summary>
    /// C2_SY1.xaml 的交互逻辑
    /// </summary>
    public partial class C3_SY1 : ChildPage
    {
        public C3_SY1()
        {
            InitializeComponent();

        }

        private void btn1_Click_1(object sender, RoutedEventArgs e)
        {
            string strText1 = textBox1.Text.Trim();
            string strText2 = textBox2.Text.Trim();
            string ret = ComTest.add("9F6E38B0-2657-443D-BCC1-1C3246221828", "Simulation Transaction", int.Parse(strText1), int.Parse(strText2));
            textBox3.Text = String.Concat(ret);
        }

        private void btn2_Click_1(object sender, RoutedEventArgs e)
        {
            string strText1 = textBox5.Text.Trim();
            string strText2 = textBox6.Text.Trim();
            string ret = ComTest.multi("A0FD122E-58B0-4BC8-9CB9-31673DCC7DB8", "User Transaction", int.Parse(strText1), int.Parse(strText2));
            textBox7.Text = String.Concat(ret);
        }
    }
}
