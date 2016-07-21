namespace reconf
{
  partial class ProjConf
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
			this.e_conf_name = new System.Windows.Forms.TextBox();
			this.c_rtl = new System.Windows.Forms.ComboBox();
			this.bt_update = new System.Windows.Forms.Button();
			this.bt_open = new System.Windows.Forms.Button();
			this.bt_save = new System.Windows.Forms.Button();
			this.bt_done = new System.Windows.Forms.Button();
			this.bt_save_as = new System.Windows.Forms.Button();
			this.lv_confs = new System.Windows.Forms.ListView();
			this.h_warn = new System.Windows.Forms.ColumnHeader();
			this.h_type = new System.Windows.Forms.ColumnHeader();
			this.h_platform = new System.Windows.Forms.ColumnHeader();
			this.h_config = new System.Windows.Forms.ColumnHeader();
			this.h_rtl = new System.Windows.Forms.ColumnHeader();
			this.h_outdir = new System.Windows.Forms.ColumnHeader();
			this.SuspendLayout();
			// 
			// e_conf_name
			// 
			this.e_conf_name.Location = new System.Drawing.Point(10, 302);
			this.e_conf_name.Name = "e_conf_name";
			this.e_conf_name.Size = new System.Drawing.Size(210, 20);
			this.e_conf_name.TabIndex = 2;
			// 
			// c_rtl
			// 
			this.c_rtl.FormattingEnabled = true;
			this.c_rtl.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.c_rtl.Location = new System.Drawing.Point(226, 302);
			this.c_rtl.Name = "c_rtl";
			this.c_rtl.Size = new System.Drawing.Size(191, 21);
			this.c_rtl.TabIndex = 3;
			// 
			// bt_update
			// 
			this.bt_update.Location = new System.Drawing.Point(423, 302);
			this.bt_update.Name = "bt_update";
			this.bt_update.Size = new System.Drawing.Size(63, 23);
			this.bt_update.TabIndex = 4;
			this.bt_update.Text = "&reconf";
			this.bt_update.UseVisualStyleBackColor = true;
			this.bt_update.Click += new System.EventHandler(this.bt_update_Click);
			// 
			// bt_open
			// 
			this.bt_open.Location = new System.Drawing.Point(492, 302);
			this.bt_open.Name = "bt_open";
			this.bt_open.Size = new System.Drawing.Size(63, 23);
			this.bt_open.TabIndex = 5;
			this.bt_open.Text = "&open";
			this.bt_open.UseVisualStyleBackColor = true;
			this.bt_open.Click += new System.EventHandler(this.bt_open_Click);
			// 
			// bt_save
			// 
			this.bt_save.Location = new System.Drawing.Point(561, 302);
			this.bt_save.Name = "bt_save";
			this.bt_save.Size = new System.Drawing.Size(63, 23);
			this.bt_save.TabIndex = 6;
			this.bt_save.Text = "&save";
			this.bt_save.UseVisualStyleBackColor = true;
			this.bt_save.Click += new System.EventHandler(this.bt_save_Click);
			// 
			// bt_done
			// 
			this.bt_done.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.bt_done.Location = new System.Drawing.Point(699, 302);
			this.bt_done.Name = "bt_done";
			this.bt_done.Size = new System.Drawing.Size(63, 23);
			this.bt_done.TabIndex = 8;
			this.bt_done.Text = "&done";
			this.bt_done.UseVisualStyleBackColor = true;
			this.bt_done.Click += new System.EventHandler(this.bt_done_Click);
			// 
			// bt_save_as
			// 
			this.bt_save_as.Location = new System.Drawing.Point(630, 302);
			this.bt_save_as.Name = "bt_save_as";
			this.bt_save_as.Size = new System.Drawing.Size(63, 23);
			this.bt_save_as.TabIndex = 7;
			this.bt_save_as.Text = "save &as";
			this.bt_save_as.UseVisualStyleBackColor = true;
			this.bt_save_as.Click += new System.EventHandler(this.bt_save_as_Click);
			// 
			// lv_confs
			// 
			this.lv_confs.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.h_warn,
            this.h_type,
            this.h_platform,
            this.h_config,
            this.h_rtl,
            this.h_outdir});
			this.lv_confs.FullRowSelect = true;
			this.lv_confs.Location = new System.Drawing.Point(10, 13);
			this.lv_confs.MultiSelect = false;
			this.lv_confs.Name = "lv_confs";
			this.lv_confs.Size = new System.Drawing.Size(749, 281);
			this.lv_confs.TabIndex = 1;
			this.lv_confs.UseCompatibleStateImageBehavior = false;
			this.lv_confs.View = System.Windows.Forms.View.Details;
			this.lv_confs.SelectedIndexChanged += new System.EventHandler(this.lv_confs_SelectedIndexChanged);
			// 
			// h_warn
			// 
			this.h_warn.Text = "*";
			this.h_warn.Width = 19;
			// 
			// h_type
			// 
			this.h_type.Text = "Type";
			this.h_type.Width = 69;
			// 
			// h_platform
			// 
			this.h_platform.Text = "Platform";
			this.h_platform.Width = 77;
			// 
			// h_config
			// 
			this.h_config.Text = "Configuration Name";
			this.h_config.Width = 168;
			// 
			// h_rtl
			// 
			this.h_rtl.Text = "Runtime Library";
			this.h_rtl.Width = 198;
			// 
			// h_outdir
			// 
			this.h_outdir.Text = "OutDir";
			this.h_outdir.Width = 244;
			// 
			// ProjConf
			// 
			this.AcceptButton = this.bt_done;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSize = true;
			this.CancelButton = this.bt_done;
			this.ClientSize = new System.Drawing.Size(769, 334);
			this.Controls.Add(this.lv_confs);
			this.Controls.Add(this.bt_save_as);
			this.Controls.Add(this.bt_done);
			this.Controls.Add(this.bt_save);
			this.Controls.Add(this.bt_open);
			this.Controls.Add(this.bt_update);
			this.Controls.Add(this.c_rtl);
			this.Controls.Add(this.e_conf_name);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.KeyPreview = true;
			this.Name = "ProjConf";
			this.Padding = new System.Windows.Forms.Padding(7);
			this.Text = "Project configurations";
			this.Load += new System.EventHandler(this.ProjConf_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.TextBox e_conf_name;
    private System.Windows.Forms.ComboBox c_rtl;
    private System.Windows.Forms.Button bt_update;
    private System.Windows.Forms.Button bt_open;
    private System.Windows.Forms.Button bt_save;
    private System.Windows.Forms.Button bt_done;
    private System.Windows.Forms.Button bt_save_as;
    private System.Windows.Forms.ListView lv_confs;
    private System.Windows.Forms.ColumnHeader h_config;
    private System.Windows.Forms.ColumnHeader h_platform;
    private System.Windows.Forms.ColumnHeader h_rtl;
    private System.Windows.Forms.ColumnHeader h_type;
		private System.Windows.Forms.ColumnHeader h_warn;
		private System.Windows.Forms.ColumnHeader h_outdir;
  }
}

