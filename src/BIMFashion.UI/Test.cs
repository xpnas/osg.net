using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BIMFashion.UI
{
    public partial class Test : Form
    {
        public BF.NET.Viewer3D Viewer3D;

        public Test()
        {
            InitializeComponent();
            this.Load += Test_Load;
            this.Disposed += Test_Disposed;
        }

        private void Test_Disposed(object sender, EventArgs e)
        {
            Viewer3D.Dispose();
        }

        private void Test_Load(object sender, EventArgs e)
        {
            IntPtr ptr = this.splitContainer1.Panel2.Handle;
            Viewer3D = new BF.NET.Viewer3D(ptr);
            Viewer3D.OnProgress += Viewer3D_OnProgress;

        }

        private void Viewer3D_OnProgress(BF.NET.Viewer3D viewer3D, bool A_1)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Multiselect = false;//该值确定是否可以选择多个文件
            dialog.Title = "请选择文件夹";
            dialog.Filter = "所有文件(*.*)|*.*";
            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                string file = dialog.FileName;
                Viewer3D.Load(file);

            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Viewer3D.Start();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Viewer3D.Stop();
        }
    }
}
