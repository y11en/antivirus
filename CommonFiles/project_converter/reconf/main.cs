using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;


// ListViewItem prepare_item(XmlNode conf)

namespace reconf {
  public partial class ProjConf : Form {
    private string m_file_name;
    private XmlDocument m_proj;
    private string m_proj_name;
    private string m_proj_ver;
    private bool m_dirty = false;

    public const int COL_WARN = 0;
    public const int COL_CTYPE = 1;
    public const int COL_PLATFORM = 2;
    public const int COL_CNAME = 3;
    public const int COL_RTL_NAME = 4;
		public const int COL_OUT_DIR = 5;
		public const int COL_NUM = 6;

    protected int column_sorted = -1;
    protected bool column_inverted = false;

    protected class RtlType {
      private int   m_id;
      public  bool  m_multi;
      public  bool  m_release;
      public  bool  m_dynamic;

      public int Code {
        get { return m_id; }
      }

      public RtlType( int id, bool multi, bool release, bool dynamic) {
        m_id = id;
        m_multi = multi;
        m_release = release;
        m_dynamic = dynamic;
      }
      public override string ToString() {
        string ret;
        if (m_multi)
          ret = "Multi-threaded";
        else
          ret = "Single-threaded";
        if (m_release)
          ret += " Release";
        else
          ret += " Debug";
        if (m_dynamic)
          ret += " Dll";
        else
          ret += " Static";
        return ret;
      }
      public static RtlType[] models = {
        //                multi   release dll
        new RtlType( 0, true,   true,   false),
        new RtlType( 1, true,   false,  false),
        new RtlType( 2, true,   true,   true),
        new RtlType( 3, true,   false,  true),
        new RtlType( 4, false,  true,   false),
        new RtlType( 5, false,  false,  false),
        new RtlType( 6, false,  true,   true),
        new RtlType( 7, false,  false,  true),
        //                multi   release dll
      };
      public static RtlType find( int key ) {
        for( int i=0; i<models.GetLength(0); ++i ) {
          if ( models[i].m_id == key )
            return models[i];
        }
        return null;
      }
      public static RtlType find(string key) {
        for (int i = 0; i < models.GetLength(0); ++i) {
          if (models[i].ToString() == key)
            return models[i];
        }
        return null;
      }
    };

    protected struct ConfType {
      public Type   id;
      public string name;
      public enum Type { unk=-1, make = 0, app = 1, dll = 2, lib = 4, util = 10 };
      public ConfType( Type pid, string pname ) {
        id = pid;
        name = pname;
      }
      private static ConfType[] ctypes = new ConfType[] {
        new ConfType( ConfType.Type.unk,  "unk" ),
        new ConfType( ConfType.Type.make, "Makefile" ),
        new ConfType( ConfType.Type.app,  "App(.exe)" ),
        new ConfType( ConfType.Type.dll,  "Dynamic(.dll)" ),
        new ConfType( ConfType.Type.lib,  "Static(.lib)" ),
        new ConfType( ConfType.Type.util, "Utility" ),
      };
      public static ConfType find(Type id) {
        for (int i = 0; i < ctypes.GetLength(0); ++i) {
          if (id == ctypes[i].id)
            return ctypes[i];
        }
        return ctypes[0];
      }
    };

    class ListWholeComparer : IComparer {
      private int cmp_col(object x, object y, int col ) {
        string xs = ((ListViewItem)x).SubItems[col].Text;
        string ys = ((ListViewItem)y).SubItems[col].Text;
        return String.Compare(xs, ys);
      }

      public int Compare(object x, object y) {
        int ret = cmp_col(x, y, COL_WARN);
        if (ret != 0)
          return ret * -1;
        ret = cmp_col(x, y, COL_CTYPE);
        if ( ret != 0 )
          return ret;
        ret = cmp_col(x, y, COL_PLATFORM);
        if (ret != 0)
          return ret;
        ret = cmp_col(x, y, COL_CNAME);
        if (ret != 0)
          return ret;
        return cmp_col(x, y, COL_RTL_NAME);
      }
    }

    class ListColumnComparer : IComparer {
      private int  m_col;
      private bool m_inv;
      public ListColumnComparer() {
        m_col = 0;
        m_inv = false;
      }
      public ListColumnComparer(int column, bool invert) {
        m_col = column;
        m_inv = invert;
      }
      public int Compare(object x, object y) {
        string xs = ((ListViewItem)x).SubItems[m_col].Text;
        string ys = ((ListViewItem)y).SubItems[m_col].Text;
        int ret = String.Compare(xs, ys);
        if (m_inv)
          ret *= -1;
        return ret;
      }
    }

    public ProjConf() {
      InitializeComponent();
    }

    private string get_node_name(XmlNode node) {
      return get_node_attr(node, "Name");
    }

    private bool set_node_name(XmlNode node, string new_name) {
      if (node == null)
        return false;
      string old_name = get_node_name(node);
      if (old_name == new_name)
        return false;
      XmlAttributeCollection attrs = node.Attributes;
      if (attrs == null)
        return false;
      XmlAttribute attr = attrs["Name"];
      if (attr == null)
        return false;
      attr.Value = new_name;
      return true;
    }

    private XmlAttribute get_attr(XmlNode node, string attr_name) {
      if (node == null)
        return null;
      XmlAttributeCollection attrs = node.Attributes;
      if (attrs == null)
        return null;
      if (attr_name == null)
        attr_name = "Name";
      return attrs[attr_name];
    }

    private string get_node_attr(XmlNode node, string attr_name)
    {
      XmlAttribute attr = get_attr(node, attr_name);
      if ( attr != null )
        return attr.Value;
      return "unknown";
    }

    private ConfType get_node_conf_type(XmlNode node) {
      ConfType.Type type = ConfType.Type.unk;
      XmlAttribute attr = get_attr(node, "ConfigurationType");
      if ( attr != null ) {
        try {
          type = (ConfType.Type)XmlConvert.ToInt32(attr.Value);
        }
        catch {
        }
      }
      return ConfType.find( type );
    }

		private string get_node_out_dir(XmlNode node) {
			XmlAttribute attr = get_attr(node, "OutputDirectory");
			if ( attr != null )
				return attr.Value;
			return "";
		}


    private string get_node_conf_type_name(XmlNode node) {
      ConfType cf = get_node_conf_type(node);
      return cf.name;
    }

    private XmlNode get_conf_node_by_name(string name) {
      XmlNodeList confs = m_proj.GetElementsByTagName("Configuration");
      for ( int i = 0; i < confs.Count; ++i ) {
        XmlNode conf = confs[i];
        string cname = get_node_name(conf);
        if (cname == name)
          return conf;
      }
      return null;
    }

    private int get_rtl_code(XmlNode node) {
      if (node == null)
        return -1;
      XmlNode child = node.FirstChild;
      while (child != null) {
        string name = get_node_name(child);
        if ( name == "VCCLCompilerTool" ) {
          string val = get_node_attr(child, "RuntimeLibrary");
          try {
            return XmlConvert.ToInt32(val);
          }
          catch {
            return -1;
          }
        }
        child = child.NextSibling;
      }
      return -1;
    }

    private bool set_rtl_code(XmlNode node, int code) {
      if (code == -1)
        return false;
      if (node == null)
        return false;
      XmlNode child = node.FirstChild;
      while (child != null)
      {
        string name = get_node_name(child);
        if (name == "VCCLCompilerTool")
        {
          int old_code;
          XmlAttribute attr = get_attr(child, "RuntimeLibrary");
          if ( attr == null )
            return false;
          string val = attr.Value;
          try {
            old_code = XmlConvert.ToInt32(val);
          }
          catch {
            old_code = -1;
          }
          if (old_code == code)
            return false;
          attr.Value = code.ToString();
          return true;
        }
        child = child.NextSibling;
      }
      return false;
    }

    private bool save_project(ref string name, bool save_as)
    {
      if ( (name == null) || (name == "") || (true == save_as) ) {
        SaveFileDialog dlg = new SaveFileDialog();
        dlg.Title = "Save " + m_proj_name + " project as";
        dlg.Filter = "Projects(*.vcproj)|*.vcproj|All(*.*)|*.*";
        dlg.FilterIndex = 0;
        dlg.AddExtension = true;
        dlg.DefaultExt = "vcproj";
        //dlg.CheckFileExists = true;
        dlg.CheckPathExists = true;
        dlg.OverwritePrompt = true;
        dlg.FileName = name;
        DialogResult result = dlg.ShowDialog();
        if (result != DialogResult.OK)
          return false;
        name = dlg.FileName;
      }
      try {
        m_proj.Save(name);
        return true;
      }
      catch {
        return false;
      }
    }

    private void check_dirty() {
      if ((m_dirty == true) && (m_proj != null)) {
        string prompt = "Project '" + m_proj_name + "' is not saved. Do you want to save it?";
        DialogResult result = MessageBox.Show(prompt, m_file_name, MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
        if (result == DialogResult.Yes)
          save_project(ref m_file_name, false);
      }
    }

    private bool open_project(string name) {
      check_dirty();
      if ( name == null || name =="" ) {
        OpenFileDialog dlg = new OpenFileDialog();
        dlg.Title = "Open project";
        dlg.Filter = "Projects(*.vcproj)|*.vcproj|All(*.*)|*.*";
        dlg.FilterIndex = 0;
        dlg.AddExtension = true;
        dlg.DefaultExt = "vcproj";
        dlg.CheckFileExists = true;
        dlg.CheckPathExists = true;
        dlg.FileName = name;
        DialogResult result = dlg.ShowDialog();
        if (result != DialogResult.OK)
          return false;
        name = dlg.FileName;
      }

      m_proj = new XmlDocument();
      try {
        m_proj.Load( name );
        XmlNodeList list = m_proj.GetElementsByTagName("VisualStudioProject");
        if (list != null)
          m_proj_name = get_node_name(list[0]);
        else
          m_proj_name = "unk";
        XmlAttribute ver = get_attr(list[0], "Version");
        if (ver == null)
          m_proj_ver = "unk";
        else
          m_proj_ver = ver.Value;
        m_file_name = name;
        m_dirty = false;
        init_dlg();
        return true;
      }
      catch {
        return false;
      }
    }

    private ListViewItem prepare_item(XmlNode conf) {
      string cname = get_node_name(conf);
      string pname = "unk";
      int ind = cname.IndexOf( '|' );
      if ( ind != -1 ) {
        pname = cname.Substring(ind+1);
        cname = cname.Substring(0,ind);
      }

      bool correct = true;
      string rtl_name;
      int rtl_code = get_rtl_code( conf );
      if (rtl_code == -1)
        rtl_name = "unk";
      else {
        RtlType mt = RtlType.find(rtl_code);
        if (mt == null)
          rtl_name = "unk";
        else {
          rtl_name = mt.ToString();
          string curr = cname.ToLower();
          ConfType cf = get_node_conf_type( conf );
					if ((cf.id == ConfType.Type.app) || (cf.id == ConfType.Type.make) || (cf.id == ConfType.Type.util) || (cf.id == ConfType.Type.unk))
						;
          else if ( !mt.m_multi ) {
            correct = false;
          }
          else {
            int len;
            int pos = -1;
            if ( mt.m_release ) {
							pos = curr.IndexOf("release");
              len = 7;
            }
            else {
              pos = curr.IndexOf("debug");
              len = 5;
            }
            if ( pos == -1 )
              correct = false;
						else if (-1 != curr.IndexOf("dll"))
							correct = false;
						else {
              string rest_of_name = curr.Substring(pos).Trim().ToLower();
              char[] separator = new char[] { ' ' };
              string[] parts = rest_of_name.Split(separator);
              string rest = parts[0];
							rest = rest.Substring(len).Trim();
							ind = rest.IndexOf( 's' );
							if (!mt.m_dynamic && (ind == -1))
								correct = false;
							else if (mt.m_dynamic && (ind != -1))
								correct = false;
            }
          }
        }
      }

      string ctype = get_node_conf_type_name(conf);

      string[] vals = new string[COL_NUM];
      vals[COL_WARN] = correct ? "" : "X";
      vals[COL_CTYPE] = ctype;
      vals[COL_CNAME] = cname;
      vals[COL_PLATFORM] = pname;
      vals[COL_RTL_NAME] = rtl_name;
			vals[COL_OUT_DIR] = get_node_out_dir(conf);
      return new ListViewItem(vals);
    }

    private void init_dlg() {
      ListView.ListViewItemCollection items = lv_confs.Items;
      items.Clear();

      if (m_proj == null)
        Text = "Project configuration";
      else {
        Text = "Project '" + m_proj_name + "' (" + m_proj_ver + ") configuration";
        XmlNodeList confs = m_proj.GetElementsByTagName("Configuration");
        for ( int i = 0; i < confs.Count; ++i ) {
          XmlNode conf = confs[i];
          ListViewItem item = prepare_item( conf );
          items.Add(item);
        }
        if (confs.Count > 0)
          lv_confs.Items[0].Selected = true;
      }
    }

    private void ProjConf_Load(object sender, EventArgs e) {
      lv_confs.FullRowSelect = true;
      c_rtl.DropDownStyle = ComboBoxStyle.DropDownList;
      ComboBox.ObjectCollection items = c_rtl.Items;
      for ( int i = 0; i < RtlType.models.GetLength(0); ++i )
        items.Add( RtlType.models[i].ToString() );

      ListView.ColumnHeaderCollection headers = lv_confs.Columns;
      int width = lv_confs.Width;
      headers[COL_WARN].Width     =  3 * width / 100;
      headers[COL_CTYPE].Width    = 10 * width / 100;
      headers[COL_PLATFORM].Width =  6 * width / 100;
      headers[COL_CNAME].Width    = 15 * width / 100;
      headers[COL_RTL_NAME].Width = 25 * width / 100;
			headers[COL_OUT_DIR].Width  = 39 * width / 100;

      lv_confs.ListViewItemSorter = new ListWholeComparer();
      lv_confs.ColumnClick += new ColumnClickEventHandler(ColumnClick);

      if (Program.g_args.Length > 0) {
        string name = Program.g_args[0];
        open_project(name);
      }
    }

    private void ColumnClick(object o, ColumnClickEventArgs e) {
      // Set the ListViewItemSorter property to a new ListViewItemComparer 
      // object. Setting this property immediately sorts the 
      // ListView using the ListViewItemComparer object.
      if (e.Column == column_sorted)
        column_inverted = !column_inverted;
      else {
        column_sorted = e.Column;
        column_inverted = false;
      }

      lv_confs.ListViewItemSorter = new ListColumnComparer(column_sorted,column_inverted);
    }

    private void lv_confs_SelectedIndexChanged(object sender, EventArgs e)
    {
      ListView.SelectedListViewItemCollection items = lv_confs.SelectedItems;
      if ( items.Count == 0 ) {
        e_conf_name.Text = "";
        c_rtl.SelectedItem = null;
      }
      else {
        ListViewItem item = items[0];
        e_conf_name.Text = item.SubItems[COL_CNAME].Text;
        string key = item.SubItems[COL_RTL_NAME].Text;
        c_rtl.SelectedIndex = c_rtl.FindString( key );
      }
    }

    private void bt_update_Click(object sender, EventArgs e) {
      ListView.SelectedListViewItemCollection items = lv_confs.SelectedItems;
      if ( items.Count == 0 )
        return;
      
      ListViewItem item = items[0];
      int ind = item.Index;
      if ( ind == -1 )
        return;
      
      string pname = item.SubItems[COL_PLATFORM].Text.Trim();
      string oname = item.SubItems[COL_CNAME].Text.Trim() + '|' + pname;

      XmlNode conf = get_conf_node_by_name(oname);
      if (conf == null)
        return;

			bool dirty_name = false;
			string cname = e_conf_name.Text.Trim() + '|' + pname;
			if ( cname != oname ) {
				ListView.ListViewItemCollection all = lv_confs.Items;
				for ( ind = 0; ind < all.Count; ++ind ) {
					ListViewItem citem = all[ind];
					string conf_name = citem.SubItems[COL_CNAME].Text + '|' + citem.SubItems[COL_PLATFORM].Text;
					if ( conf_name == cname ){
						MessageBox.Show(this, "Configuration \"" + cname + "\" already exists in the project", Text);
						return;
					}
				}
				dirty_name = set_node_name(conf, cname);
			}

			bool dirty_code = false;
			ind = c_rtl.SelectedIndex;
      if ( (ind != -1) && (ind < RtlType.models.GetLength(0)) ) {
        RtlType rtl = RtlType.models[ind];
        dirty_code = set_rtl_code(conf, rtl.Code);
      }

      if ( dirty_name || dirty_code ) {
        m_dirty = true;
        int pos = item.Index;
        item = prepare_item(conf);
        lv_confs.Items[pos] = item;
        item.Selected = true;
      }
      else
        lv_confs_SelectedIndexChanged(null, null);
      lv_confs.Select();
    }

    private void bt_open_Click(object sender, EventArgs e)
    {
      open_project(null);
      lv_confs.Select();
    }

    private void bt_save_Click(object sender, EventArgs e)
    {
      string new_name = m_file_name;
      if (save_project(ref new_name,false))
        m_dirty = false;
      lv_confs.Select();
    }

    private void bt_save_as_Click(object sender, EventArgs e)
    {
      string new_name = m_file_name;
      if (save_project(ref new_name, true)) {
        m_dirty = false;
        open_project(new_name);
      }
      lv_confs.Select();
    }
  
    private void bt_done_Click(object sender, EventArgs e)
    {
      check_dirty();
      Close();
    }

  }
}