using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SystemWatcher;

namespace SystemWatcherGui
{
    public partial class ProcessView : Form
    {

        public class EventFormatter : ICustomFormatter, IFormatProvider
        {
            public object GetFormat(Type argType)
            {
                return this;
            }

            public string Format(string formatString, object argToBeFormatted, IFormatProvider provider)
            {
                if( argToBeFormatted.GetType() != typeof(UInt32) )
                    return "unsupported data type";

                if( formatString == "objectType" )
                    return ObjectType2String((UInt32)argToBeFormatted);
                
                if( formatString == "eventType" )
                    return EventType2String((UInt32)argToBeFormatted);

                return "unsupported format";
            }

            static public string ObjectType2String(UInt32 type)
            {
                switch (type)
                {
                    case 1: return "File";
                    case 2: return "Folder";
                    case 3: return "Registry value";
                    case 4: return "Registry key";
                    case 5: return "Shared folder";
                    case 6: return "User Account";
                    case 7: return "Url";
                    case 8: return "IP-address";
                }
                return "unknown";
            }

            static public string EventType2String(UInt32 type)
            {
                switch (type)
                {
                    case 1: return "Created";
                    case 2: return "Modified";
                    case 3: return "Deleted";
                    case 4: return "Renamed";
                    case 5: return "Process started";
                    case 6: return "Process finished";
                    case 7: return "Library loaded";
                    case 8: return "Library unloaded";
                }
                return "unknown";
            }
        };

        protected DataSource m_ds = new DataSource();

        public ProcessView()                                  { Initialize(0, 0); }
        public ProcessView(UInt32 uniquePid, UInt64 objectId) { Initialize(uniquePid, objectId); }

        private void Initialize(UInt32 uniquePid, UInt64 objectId)
        {
            InitializeComponent();
            m_panelProcessInfo.Visible = uniquePid != 0;
            
            EventFormatter eventFormatter = new EventFormatter();
            
            m_colTime.DefaultCellStyle.Format = "f";
            m_colObjectID.DefaultCellStyle.Format = "x";
            m_colProcess.DefaultCellStyle.Format = "x";
            m_colChildProcess.DefaultCellStyle.Format = "x";
            m_colType.DefaultCellStyle.Format = "objectType";
            m_colType.DefaultCellStyle.FormatProvider = eventFormatter;
            m_colAction.DefaultCellStyle.Format = "eventType";
            m_colAction.DefaultCellStyle.FormatProvider = eventFormatter;

            m_ds.FetchInfo(uniquePid, objectId);
            m_events.DataSource = m_ds;
            m_events.DataMember = "eventsList";

            if( uniquePid != 0 )
            {
                m_processImageName.DataBindings.Add("Text", m_ds, "processInfo.imagePath");
                m_startTime.DataBindings.Add("Text", m_ds, "processInfo.startTime");
                m_finishTime.DataBindings.Add("Text", m_ds, "processInfo.finishTime");
                m_startEvent.DataBindings.Add("Text", m_ds, "processInfo.startEvent");
                m_finishEvent.DataBindings.Add("Text", m_ds, "processInfo.finishEvent");
                m_ownPID.DataBindings.Add("Text", m_ds, "processInfo.unique", true, DataSourceUpdateMode.OnPropertyChanged, "unknown", "x");
                m_parentPID.DataBindings.Add("Text", m_ds, "processInfo.parentUnique", true, DataSourceUpdateMode.OnPropertyChanged, "unknown", "x");
            }
        }

        private void m_parent_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            UInt32 parentUnique = (UInt32)m_ds.Tables["processInfo"].Rows[0]["parentUnique"];
            ProcessView parentProcessView = new ProcessView(parentUnique, 0);
            parentProcessView.ShowDialog(this);
        }

        private void m_events_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            DataGridView grid = (DataGridView)sender;
            if( e.RowIndex == -1 || e.ColumnIndex == -1 )
                return;
            DataGridViewColumn column = grid.Columns[e.ColumnIndex];
            if( column.GetType() != typeof(DataGridViewLinkColumn) )
                return;

            ProcessView processView = null;

            DataGridViewCell cell = grid.Rows[e.RowIndex].Cells[e.ColumnIndex];
            if( column.Name == m_colProcess.Name )
                processView = new ProcessView(System.Convert.ToUInt32(cell.Value), 0);
            else
            if (column.Name == m_colChildProcess.Name)
                processView = new ProcessView(System.Convert.ToUInt32(cell.Value), 0);
            else
            if( column.Name == m_colObjectID.Name )
                processView = new ProcessView(0, System.Convert.ToUInt64(cell.Value));
            
            if( processView != null )
                processView.ShowDialog(this);
        }

        private void m_events_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            DataGridViewColumn col = m_events.Columns[e.ColumnIndex];
            if( col.DefaultCellStyle.FormatProvider is ICustomFormatter )
            {
                ICustomFormatter formatter = (ICustomFormatter)col.DefaultCellStyle.FormatProvider;
                e.Value = formatter.Format(col.DefaultCellStyle.Format, e.Value, col.DefaultCellStyle.FormatProvider);
            }
        }
    }
}