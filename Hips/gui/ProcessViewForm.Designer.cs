namespace SystemWatcherGui
{
    partial class ProcessView
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
            System.Windows.Forms.Label label1;
            System.Windows.Forms.Label label2;
            System.Windows.Forms.Label label4;
            System.Windows.Forms.Label label5;
            System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
            System.Windows.Forms.Label label3;
            System.Windows.Forms.Label label6;
            System.Windows.Forms.Label label8;
            System.Windows.Forms.Label label7;
            this.m_panelProcessInfo = new System.Windows.Forms.Panel();
            this.m_parentPID = new System.Windows.Forms.LinkLabel();
            this.m_finishEvent = new System.Windows.Forms.MaskedTextBox();
            this.m_ownPID = new System.Windows.Forms.MaskedTextBox();
            this.m_finishTime = new System.Windows.Forms.MaskedTextBox();
            this.m_startEvent = new System.Windows.Forms.MaskedTextBox();
            this.m_startTime = new System.Windows.Forms.MaskedTextBox();
            this.m_processImageName = new System.Windows.Forms.MaskedTextBox();
            this.m_events = new System.Windows.Forms.DataGridView();
            this.m_colTime = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_colType = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_colAction = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_colObjectID = new System.Windows.Forms.DataGridViewLinkColumn();
            this.m_colProcess = new System.Windows.Forms.DataGridViewLinkColumn();
            this.m_colChildProcess = new System.Windows.Forms.DataGridViewLinkColumn();
            this.m_colDescription = new System.Windows.Forms.DataGridViewTextBoxColumn();
            label1 = new System.Windows.Forms.Label();
            label2 = new System.Windows.Forms.Label();
            label4 = new System.Windows.Forms.Label();
            label5 = new System.Windows.Forms.Label();
            tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            label3 = new System.Windows.Forms.Label();
            label6 = new System.Windows.Forms.Label();
            label8 = new System.Windows.Forms.Label();
            label7 = new System.Windows.Forms.Label();
            tableLayoutPanel1.SuspendLayout();
            this.m_panelProcessInfo.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_events)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(3, 5);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(48, 13);
            label1.TabIndex = 1;
            label1.Text = "Process:";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new System.Drawing.Point(3, 31);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(54, 13);
            label2.TabIndex = 1;
            label2.Text = "Start time:";
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new System.Drawing.Point(477, 31);
            label4.Name = "label4";
            label4.Size = new System.Drawing.Size(53, 13);
            label4.TabIndex = 1;
            label4.Text = "Own PID:";
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new System.Drawing.Point(235, 31);
            label5.Name = "label5";
            label5.Size = new System.Drawing.Size(59, 13);
            label5.TabIndex = 1;
            label5.Text = "Finish time:";
            // 
            // tableLayoutPanel1
            // 
            tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            tableLayoutPanel1.ColumnCount = 1;
            tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            tableLayoutPanel1.Controls.Add(this.m_panelProcessInfo, 0, 0);
            tableLayoutPanel1.Controls.Add(this.m_events, 0, 1);
            tableLayoutPanel1.Location = new System.Drawing.Point(12, 12);
            tableLayoutPanel1.Name = "tableLayoutPanel1";
            tableLayoutPanel1.RowCount = 2;
            tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            tableLayoutPanel1.Size = new System.Drawing.Size(747, 363);
            tableLayoutPanel1.TabIndex = 6;
            // 
            // m_panelProcessInfo
            // 
            this.m_panelProcessInfo.Controls.Add(label3);
            this.m_panelProcessInfo.Controls.Add(label2);
            this.m_panelProcessInfo.Controls.Add(this.m_parentPID);
            this.m_panelProcessInfo.Controls.Add(label1);
            this.m_panelProcessInfo.Controls.Add(this.m_finishEvent);
            this.m_panelProcessInfo.Controls.Add(this.m_ownPID);
            this.m_panelProcessInfo.Controls.Add(this.m_finishTime);
            this.m_panelProcessInfo.Controls.Add(label6);
            this.m_panelProcessInfo.Controls.Add(label5);
            this.m_panelProcessInfo.Controls.Add(this.m_startEvent);
            this.m_panelProcessInfo.Controls.Add(this.m_startTime);
            this.m_panelProcessInfo.Controls.Add(label8);
            this.m_panelProcessInfo.Controls.Add(label7);
            this.m_panelProcessInfo.Controls.Add(label4);
            this.m_panelProcessInfo.Controls.Add(this.m_processImageName);
            this.m_panelProcessInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.m_panelProcessInfo.Location = new System.Drawing.Point(3, 3);
            this.m_panelProcessInfo.Name = "m_panelProcessInfo";
            this.m_panelProcessInfo.Size = new System.Drawing.Size(741, 90);
            this.m_panelProcessInfo.TabIndex = 4;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new System.Drawing.Point(3, 50);
            label3.Name = "label3";
            label3.Size = new System.Drawing.Size(62, 13);
            label3.TabIndex = 1;
            label3.Text = "Start event:";
            // 
            // m_parentPID
            // 
            this.m_parentPID.AutoSize = true;
            this.m_parentPID.Location = new System.Drawing.Point(545, 50);
            this.m_parentPID.Name = "m_parentPID";
            this.m_parentPID.Size = new System.Drawing.Size(55, 13);
            this.m_parentPID.TabIndex = 5;
            this.m_parentPID.TabStop = true;
            this.m_parentPID.Text = "parentPID";
            this.m_parentPID.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.m_parent_LinkClicked);
            // 
            // m_finishEvent
            // 
            this.m_finishEvent.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_finishEvent.Location = new System.Drawing.Point(308, 50);
            this.m_finishEvent.Name = "m_finishEvent";
            this.m_finishEvent.ReadOnly = true;
            this.m_finishEvent.Size = new System.Drawing.Size(100, 13);
            this.m_finishEvent.TabIndex = 4;
            // 
            // m_ownPID
            // 
            this.m_ownPID.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_ownPID.Location = new System.Drawing.Point(548, 31);
            this.m_ownPID.Name = "m_ownPID";
            this.m_ownPID.ReadOnly = true;
            this.m_ownPID.Size = new System.Drawing.Size(100, 13);
            this.m_ownPID.TabIndex = 4;
            // 
            // m_finishTime
            // 
            this.m_finishTime.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_finishTime.Location = new System.Drawing.Point(308, 31);
            this.m_finishTime.Name = "m_finishTime";
            this.m_finishTime.ReadOnly = true;
            this.m_finishTime.Size = new System.Drawing.Size(100, 13);
            this.m_finishTime.TabIndex = 4;
            // 
            // label6
            // 
            label6.AutoSize = true;
            label6.Location = new System.Drawing.Point(235, 50);
            label6.Name = "label6";
            label6.Size = new System.Drawing.Size(67, 13);
            label6.TabIndex = 1;
            label6.Text = "Finish event:";
            // 
            // m_startEvent
            // 
            this.m_startEvent.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_startEvent.Location = new System.Drawing.Point(71, 50);
            this.m_startEvent.Name = "m_startEvent";
            this.m_startEvent.ReadOnly = true;
            this.m_startEvent.Size = new System.Drawing.Size(100, 13);
            this.m_startEvent.TabIndex = 4;
            // 
            // m_startTime
            // 
            this.m_startTime.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_startTime.Location = new System.Drawing.Point(71, 31);
            this.m_startTime.Name = "m_startTime";
            this.m_startTime.ReadOnly = true;
            this.m_startTime.Size = new System.Drawing.Size(100, 13);
            this.m_startTime.TabIndex = 4;
            // 
            // label8
            // 
            label8.AutoSize = true;
            label8.Location = new System.Drawing.Point(477, 50);
            label8.Name = "label8";
            label8.Size = new System.Drawing.Size(62, 13);
            label8.TabIndex = 1;
            label8.Text = "Parent PID:";
            // 
            // label7
            // 
            label7.AutoSize = true;
            label7.Location = new System.Drawing.Point(477, 50);
            label7.Name = "label7";
            label7.Size = new System.Drawing.Size(55, 13);
            label7.TabIndex = 1;
            label7.Text = "Parent ID:";
            // 
            // m_processImageName
            // 
            this.m_processImageName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_processImageName.Location = new System.Drawing.Point(57, 2);
            this.m_processImageName.Name = "m_processImageName";
            this.m_processImageName.ReadOnly = true;
            this.m_processImageName.Size = new System.Drawing.Size(681, 20);
            this.m_processImageName.TabIndex = 2;
            // 
            // m_events
            // 
            this.m_events.AllowUserToAddRows = false;
            this.m_events.AllowUserToDeleteRows = false;
            this.m_events.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_events.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_events.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.m_colTime,
            this.m_colType,
            this.m_colAction,
            this.m_colObjectID,
            this.m_colProcess,
            this.m_colChildProcess,
            this.m_colDescription});
            this.m_events.Location = new System.Drawing.Point(3, 99);
            this.m_events.Name = "m_events";
            this.m_events.ReadOnly = true;
            this.m_events.Size = new System.Drawing.Size(741, 261);
            this.m_events.TabIndex = 3;
            this.m_events.CellFormatting += new System.Windows.Forms.DataGridViewCellFormattingEventHandler(this.m_events_CellFormatting);
            this.m_events.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_events_CellContentClick);
            // 
            // m_colTime
            // 
            this.m_colTime.DataPropertyName = "time";
            this.m_colTime.HeaderText = "Time";
            this.m_colTime.Name = "m_colTime";
            this.m_colTime.ReadOnly = true;
            this.m_colTime.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // m_colType
            // 
            this.m_colType.DataPropertyName = "type";
            this.m_colType.HeaderText = "Type";
            this.m_colType.Name = "m_colType";
            this.m_colType.ReadOnly = true;
            this.m_colType.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // m_colAction
            // 
            this.m_colAction.DataPropertyName = "action";
            this.m_colAction.HeaderText = "Action";
            this.m_colAction.Name = "m_colAction";
            this.m_colAction.ReadOnly = true;
            this.m_colAction.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // m_colObjectID
            // 
            this.m_colObjectID.DataPropertyName = "objectID";
            this.m_colObjectID.HeaderText = "Object ID";
            this.m_colObjectID.Name = "m_colObjectID";
            this.m_colObjectID.ReadOnly = true;
            this.m_colObjectID.VisitedLinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(0)))), ((int)(((byte)(255)))));
            this.m_colObjectID.Width = 107;
            // 
            // m_colProcess
            // 
            this.m_colProcess.DataPropertyName = "process";
            this.m_colProcess.HeaderText = "Process";
            this.m_colProcess.Name = "m_colProcess";
            this.m_colProcess.ReadOnly = true;
            this.m_colProcess.VisitedLinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(0)))), ((int)(((byte)(255)))));
            this.m_colProcess.Width = 59;
            // 
            // m_colChildProcess
            // 
            this.m_colChildProcess.DataPropertyName = "childProcess";
            this.m_colChildProcess.HeaderText = "Child Process";
            this.m_colChildProcess.Name = "m_colChildProcess";
            this.m_colChildProcess.ReadOnly = true;
            this.m_colChildProcess.VisitedLinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(0)))), ((int)(((byte)(255)))));
            this.m_colChildProcess.Width = 77;
            // 
            // m_colDescription
            // 
            this.m_colDescription.DataPropertyName = "description";
            this.m_colDescription.FillWeight = 500F;
            this.m_colDescription.HeaderText = "Description";
            this.m_colDescription.Name = "m_colDescription";
            this.m_colDescription.ReadOnly = true;
            this.m_colDescription.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            this.m_colDescription.Width = 500;
            // 
            // ProcessView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(771, 387);
            this.Controls.Add(tableLayoutPanel1);
            this.MinimumSize = new System.Drawing.Size(600, 300);
            this.Name = "ProcessView";
            this.Text = "System Watcher";
            tableLayoutPanel1.ResumeLayout(false);
            this.m_panelProcessInfo.ResumeLayout(false);
            this.m_panelProcessInfo.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_events)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.MaskedTextBox m_processImageName;
        private System.Windows.Forms.MaskedTextBox m_startTime;
        private System.Windows.Forms.MaskedTextBox m_finishTime;
        private System.Windows.Forms.Panel m_panelProcessInfo;
        private System.Windows.Forms.MaskedTextBox m_startEvent;
        private System.Windows.Forms.MaskedTextBox m_finishEvent;
        private System.Windows.Forms.LinkLabel m_parentPID;
        private System.Windows.Forms.MaskedTextBox m_ownPID;
        private System.Windows.Forms.DataGridView m_events;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_colTime;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_colType;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_colAction;
        private System.Windows.Forms.DataGridViewLinkColumn m_colObjectID;
        private System.Windows.Forms.DataGridViewLinkColumn m_colProcess;
        private System.Windows.Forms.DataGridViewLinkColumn m_colChildProcess;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_colDescription;
    }
}

