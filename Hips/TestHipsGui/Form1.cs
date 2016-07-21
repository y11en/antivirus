using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace TestHipsGui
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            openFileDialog1.Filter = "Exe Files(*.EXE)|*.EXE|All files (*.*)|*.*";
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // TODO: This line of code loads data into the 'databaseDataSet.IPRanges' table. You can move, or remove it, as needed.
            this.iPRangesTableAdapter.Fill(this.databaseDataSet.IPRanges);
            // TODO: This line of code loads data into the 'databaseDataSet.ResTree' table. You can move, or remove it, as needed.
            this.resTreeTableAdapter.Fill(this.databaseDataSet.ResTree);
            // TODO: This line of code loads data into the 'databaseDataSet.ResTree' table. You can move, or remove it, as needed.
            this.resTreeTableAdapter.Fill(this.databaseDataSet.ResTree);
            // TODO: This line of code loads data into the 'databaseDataSet.WebServices' table. You can move, or remove it, as needed.
            this.webServicesTableAdapter.Fill(this.databaseDataSet.WebServices);
            // TODO: This line of code loads data into the 'databaseDataSet.WebDirection' table. You can move, or remove it, as needed.
            this.webDirectionTableAdapter.Fill(this.databaseDataSet.WebDirection);
            // TODO: This line of code loads data into the 'databaseDataSet.WebProtocol' table. You can move, or remove it, as needed.
            this.webProtocolTableAdapter.Fill(this.databaseDataSet.WebProtocol);
            CHipsBaseSerializer_dll.AutoInitializeConst();
            // TODO: This line of code loads data into the 'databaseDataSet.AppGrTable' table. You can move, or remove it, as needed.
            //this.appGrTableTableAdapter.Fill(this.databaseDataSet.AppGrTable);
            LoadSettings();
            // TODO: This line of code loads data into the 'databaseDataSet.RuleGrNameTable' table. You can move, or remove it, as needed.
            this.ruleGrNameTableTableAdapter.Fill(this.databaseDataSet.RuleGrNameTable);
            // TODO: This line of code loads data into the 'databaseDataSet.RulesTable' table. You can move, or remove it, as needed.
            this.rulesTableTableAdapter.Fill(this.databaseDataSet.RulesTable);
            // TODO: This line of code loads data into the 'databaseDataSet.ResGrTable' table. You can move, or remove it, as needed.
            //this.resGrTableTableAdapter.Fill(this.databaseDataSet.ResGrTable);
            // TODO: This line of code loads data into the 'databaseDataSet.AppGrNameTable' table. You can move, or remove it, as needed.
            this.appGrNameTableTableAdapter.Fill(this.databaseDataSet.AppGrNameTable);
            // TODO: This line of code loads data into the 'databaseDataSet.AppTable' table. You can move, or remove it, as needed.
            this.appTableTableAdapter.Fill(this.databaseDataSet.AppTable);
            // TODO: This line of code loads data into the 'databaseDataSet.ResTable' table. You can move, or remove it, as needed.
            this.resTableTableAdapter.Fill(this.databaseDataSet.ResTable);
            // TODO: This line of code loads data into the 'databaseDataSet.ResGrNameTable' table. You can move, or remove it, as needed.
            //this.resGrNameTableTableAdapter.Fill(this.databaseDataSet.ResGrNameTable);
            // TODO: This line of code loads data into the 'databaseDataSet.ResTypeTable' table. You can move, or remove it, as needed.
            this.resTypeTableTableAdapter.Fill(this.databaseDataSet.ResTypeTable);

            CHipsBaseSerializer_dll.Initialize();

        }

        private bool SaveChangesInDB()
        {
            try
            {

                Validate();

                resTableBindingSource.EndEdit();
                resTypeTableBindingSource.EndEdit();
                resGrNameTableBindingSource.EndEdit();
                //resGrTableBindingSource.EndEdit();
                rulesTableBindingSource.EndEdit();
                appTableBindingSource.EndEdit();
                appGrNameTableBindingSource.EndEdit();
                //appGrTableBindingSource.EndEdit();
                ruleGrNameTableBindingSource.EndEdit();

                webDirectionBindingSource.EndEdit();
                webProtocolBindingSource.EndEdit();
                webServicesBindingSource.EndEdit();

                resTreeBindingSource.EndEdit();

                iPRangesBindingSource.EndEdit();


                //resGrTableTableAdapter.Update(databaseDataSet.ResGrTable);
                resTableTableAdapter.Update(databaseDataSet.ResTable);
                resGrNameTableTableAdapter.Update(databaseDataSet.ResGrNameTable);
                resTypeTableTableAdapter.Update(databaseDataSet.ResTypeTable);
                appTableTableAdapter.Update(databaseDataSet.AppTable);
                appGrNameTableTableAdapter.Update(databaseDataSet.AppGrNameTable);
                //appGrTableTableAdapter.Update(databaseDataSet.AppGrTable);
                rulesTableTableAdapter.Update(databaseDataSet.RulesTable);
                ruleGrNameTableTableAdapter.Update(databaseDataSet.RuleGrNameTable);

                webDirectionTableAdapter.Update(databaseDataSet.WebDirection);
                webProtocolTableAdapter.Update(databaseDataSet.WebProtocol);
                webServicesTableAdapter.Update(databaseDataSet.WebServices);

                resTreeTableAdapter.Update(databaseDataSet.ResTree);

                iPRangesTableAdapter.Update(databaseDataSet.IPRanges);

                return true;
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("Update failed");
                return false;
            }

        }

        private bool makeTxtFile()
        {
            try
            {
        //private int ResTree_NumberColumnNumber = 0;
        //private int ResTree_StateColumnNumber = 1;
        //private int ResTree_ParentColumnNumber = 2;
        //private int ResTree_NameColumnNumber = 3;
        //private int ResTree_TypeColumnNumber = 4;
        //private int ResTree_ResRefColumnNumber = 5;
                //  fill resource tree (groups only)
                for (int i = 0; i < this.databaseDataSet.ResTree.Rows.Count; i++)
                {
                    DataRow data = this.databaseDataSet.ResTree.Rows[i];
                    if (0 == Convert.ToUInt32(data.ItemArray[ResTree_NumberColumnNumber]))
                        continue;
                    switch (Convert.ToUInt32(data.ItemArray[ResTree_TypeColumnNumber]))
                    {
                        case 0:
                        case 1:
                            {
                                //  group
                                CHipsBaseSerializer_dll.AddResourceGroup(
                                    Convert.ToUInt32(data.ItemArray[ResTree_NumberColumnNumber]),
                                    Convert.ToUInt32(data.ItemArray[ResTree_ParentColumnNumber]),
                                    Convert.ToString(data.ItemArray[ResTree_NameColumnNumber]),
                                    Convert.ToUInt32(data.ItemArray[ResTree_ExFlagsColumnNumber])
                                    );
                                break;
                            }
                    }
                }

                //  fill resource tree
                for (int i = 0; i < this.databaseDataSet.ResTree.Rows.Count; i++)
                {
                    DataRow data = this.databaseDataSet.ResTree.Rows[i];
                    //if (!IsValidRow(data))
                    //{
                    //    continue;
                    //}
                    if (0 == Convert.ToUInt32(data.ItemArray[ResTree_NumberColumnNumber]))
                        continue;
                    switch (Convert.ToUInt32(data.ItemArray[ResTree_TypeColumnNumber]))
                    {
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 8:
                        case 9:
                        case 10:
                            {
                                //  simple resource
                                string TempFilter = "ResId = " + Convert.ToString(data.ItemArray[ResTree_ResRefColumnNumber]);
                                DataRow[] tempData = databaseDataSet.ResTable.Select(TempFilter);
                                if (tempData.Length > 0)
                                {
                                    CHipsBaseSerializer_dll.AddSimpleResource(
                                        (data.ItemArray[ResTree_StateColumnNumber].ToString() == "") ?
                                            false : Convert.ToBoolean(data.ItemArray[ResTree_StateColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_ExFlagsColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_NumberColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_ParentColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_TypeColumnNumber]),
                                        Convert.ToString(data.ItemArray[ResTree_NameColumnNumber]),
                                        Convert.ToString(tempData[0].ItemArray[Res_Param1ColumnNumber]),
                                        Convert.ToString(tempData[0].ItemArray[Res_Param2ColumnNumber])
                                        );
                                }
                                break;
                            }
                        case 6:
                            {
                                //  WebService
                                string TempFilter = "WebServId = " + Convert.ToString(data.ItemArray[ResTree_ResRefColumnNumber]);
                                DataRow[] tempData = databaseDataSet.WebServices.Select(TempFilter);
                                if (tempData.Length > 0)
                                {
                                    CHipsBaseSerializer_dll.AddWebService(
                                        (data.ItemArray[ResTree_StateColumnNumber].ToString() == "") ?
                                            false : Convert.ToBoolean(data.ItemArray[ResTree_StateColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_NumberColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_ParentColumnNumber]),
                                        Convert.ToString(data.ItemArray[ResTree_NameColumnNumber]),
                                        (tempData[0].ItemArray[WebServ_UseProtocolColumnNumber].ToString() == "") ?
                                            false : Convert.ToBoolean(tempData[0].ItemArray[WebServ_UseProtocolColumnNumber]),
                                        Convert.ToUInt32(tempData[0].ItemArray[WebServ_ProtocolColumnNumber]),
                                        (tempData[0].ItemArray[WebServ_UseTypeColumnNumber].ToString() == "") ?
                                            false : Convert.ToBoolean(tempData[0].ItemArray[WebServ_UseTypeColumnNumber]),
                                        Convert.ToUInt32(tempData[0].ItemArray[WebServ_IcmpTypeColumnNumber]),
                                        (tempData[0].ItemArray[WebServ_UseCodeColumnNumber].ToString() == "") ?
                                            false : Convert.ToBoolean(tempData[0].ItemArray[WebServ_UseCodeColumnNumber]),
                                        Convert.ToUInt32(tempData[0].ItemArray[WebServ_IcmpCodeColumnNumber]),
                                        Convert.ToUInt32(tempData[0].ItemArray[WebServ_DirectionColumnNumber]),
                                        Convert.ToString(tempData[0].ItemArray[WebServ_RemPortsColumnNumber]),
                                        Convert.ToString(tempData[0].ItemArray[WebServ_LocPortsColumnNumber])
                                        );
                                }
                                break;
                            }
                        case 7:
                            {
                                //  IPRange
                                string TempFilter = "IPRangeID = " + Convert.ToString(data.ItemArray[ResTree_ResRefColumnNumber]);
                                DataRow[] tempData = databaseDataSet.IPRanges.Select(TempFilter);
                                if (tempData.Length > 0)
                                {
                                    CHipsBaseSerializer_dll.AddSimpleResource(
                                        (data.ItemArray[ResTree_StateColumnNumber].ToString() == "") ?
                                            false : Convert.ToBoolean(data.ItemArray[ResTree_StateColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_ExFlagsColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_NumberColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_ParentColumnNumber]),
                                        Convert.ToUInt32(data.ItemArray[ResTree_TypeColumnNumber]),
                                        Convert.ToString(data.ItemArray[ResTree_NameColumnNumber]),
                                        Convert.ToString(tempData[0].ItemArray[IPRange_LocalColumnNumber]),
                                        Convert.ToString(tempData[0].ItemArray[IPRange_RemoteColumnNumber])
                                        );
                                }
                                break;
                            }
                    }
                }

                ////  fill resource grops
                //for (int i = 0; i < this.databaseDataSet.ResGrNameTable.Rows.Count; i++)
                //{
                //    DataRow data = this.databaseDataSet.ResGrNameTable.Rows[i];
                //    if (!IsValidRow(data))
                //    {
                //        continue;
                //    }
                //    CHipsBaseSerializer_dll.AddResourceGroup(
                //        Convert.ToUInt32(data.ItemArray[0]),
                //        Convert.ToUInt32(data.ItemArray[2]),
                //        Convert.ToString(data.ItemArray[1]));
                //}

                ////  fill resources
                //for (int i = 0; i < this.databaseDataSet.ResTable.Rows.Count; i++)
                //{
                //    DataRow data = this.databaseDataSet.ResTable.Rows[i];
                //    if (!IsValidRow(data))
                //    {
                //        continue;
                //    }
                //    CHipsBaseSerializer_dll.AddSimpleResource(
                //        Convert.ToUInt32(data.ItemArray[Res_NumberColumnNumber]),
                //        Convert.ToUInt32(data.ItemArray[Res_GrIDColumnNumber]),
                //        Convert.ToUInt32(data.ItemArray[Res_TypeColumnNumber]),
                //        Convert.ToString(data.ItemArray[Res_DescColumnNumber]),
                //        Convert.ToString(data.ItemArray[Res_Param1ColumnNumber]),
                //        Convert.ToString(data.ItemArray[Res_Param2ColumnNumber])
                //        );
                //}

                

                //  fill application grops
                for (int i = 0; i < this.databaseDataSet.AppGrNameTable.Rows.Count; i++)
                {
                    DataRow data = this.databaseDataSet.AppGrNameTable.Rows[i];
                    if (!IsValidRow(data))
                    {
                        continue;
                    }
                    CHipsBaseSerializer_dll.AddAppGroup(
                        Convert.ToUInt32(data.ItemArray[0]),
                        Convert.ToUInt32(data.ItemArray[2]),
                        Convert.ToString(data.ItemArray[1]),
                        Convert.ToUInt32(data.ItemArray[3]),
                        Convert.ToString(data.ItemArray[4]));
                }

                //  fill applications
                for (int i = 0; i < this.databaseDataSet.AppTable.Rows.Count; i++)
                {
                    DataRow data = this.databaseDataSet.AppTable.Rows[i];
                    if (!IsValidRow(data))
                    {
                        continue;
                    }

                    CHipsBaseSerializer_dll.AddApp(
                        Convert.ToUInt32(data.ItemArray[App_NumberColumnNumber]),
                        Convert.ToUInt32(data.ItemArray[App_GrIdColumnNumber]),
                        Convert.ToString(data.ItemArray[App_NameColumnNumber]),
                        Convert.ToString(data.ItemArray[App_HashColumnNumber]),
                        Convert.ToUInt32(data.ItemArray[App_FlagsColumnNumber])
                        );
                }

                ////  fill web services
                //for (int i = 0; i < this.databaseDataSet.WebServices.Rows.Count; i++)
                //{
                //    DataRow data = this.databaseDataSet.WebServices.Rows[i];
                //    //if (!IsValidRow(data))
                //    //{
                //        //continue;
                //    //}
                //    CHipsBaseSerializer_dll.AddWebService(
                //        Convert.ToUInt32(data.ItemArray[WebServ_NumberColumnNumber]),
                //        Convert.ToUInt32(data.ItemArray[WebServ_ParentGrIdColumnNumber]),
                //        Convert.ToString(data.ItemArray[WebServ_DescColumnNumber]),
                //        Convert.ToUInt32(data.ItemArray[WebServ_ProtocolColumnNumber]),
                //        Convert.ToUInt32(data.ItemArray[WebServ_IcmpCodeColumnNumber]),
                //        Convert.ToUInt32(data.ItemArray[WebServ_DirectionColumnNumber]),
                //        Convert.ToString(data.ItemArray[WebServ_RemPortsColumnNumber]),
                //        Convert.ToString(data.ItemArray[WebServ_LocPortsColumnNumber])
                //        );
                //}

                //  fill rule list
                for (int i = 0; i < this.databaseDataSet.RulesTable.Rows.Count; i++)
                {
                    DataRow data = this.databaseDataSet.RulesTable.Rows[i];
                    if (!IsValidRow(data))
                    {
                        continue;
                    }
                    CHipsBaseSerializer_dll.AddRule(
                        Convert.ToUInt32(data.ItemArray[Rule_NumberColumnNumber]),
                        Convert.ToUInt32((data.ItemArray[Rule_EnableColumnNumber].ToString() == "") ?
                            0 : Convert.ToInt32(data.ItemArray[Rule_EnableColumnNumber])),
                        Convert.ToUInt32(data.ItemArray[Rule_RuleGrColumnNumber]),
                        Convert.ToUInt32(data.ItemArray[Rule_AppIDColumnNumber]),
                        Convert.ToUInt32(data.ItemArray[Rule_AppGrIDColumnNumber]),
                        Convert.ToUInt32(data.ItemArray[Rule_AppFlagsColumnNumber]),
                        Convert.ToUInt32(data.ItemArray[Rule_ResourceIDColumnNumber]),
                        Convert.ToUInt32(data.ItemArray[Rule_ResourceID2ColumnNumber]),
                        Convert.ToUInt32(data.ItemArray[Rule_AccessFlagColumnNumber])
                    );
                }
                
                CHipsBaseSerializer_dll.CompileBase(HipsDBFileName);
                return true;
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("Gen txt file fail");
                return false;
            }
        }
        private bool IsValidRow(DataRow Row)
        {
            for (int i = 0; i < Row.ItemArray.Length; i++)
            {
                if (Row.ItemArray[i].ToString() == "")
                {
                    if (Row.Table.ToString() == "ResTable" && i == Res_DescColumnNumber)
                    {
                    }
                    else if (Row.Table.ToString() == "AppTable" && i == App_HashColumnNumber)
                    {
                    }
                    else if (Row.Table.ToString() == "AppGrNameTable" && i == AppGr_ServStrNumber)
                    {
                    }

                    else
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        private void saveAndGenToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (SaveChangesInDB() && makeTxtFile())
            {
                MessageBox.Show("Ok", "TestHipsGui");
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (!ResListChB.Checked)
            {
                ResListChB.Checked = true;
                ResListChB_CheckedChanged(sender, e);
            }
            if (!AppListChB.Checked)
            {
                AppListChB.Checked = true;
                AppListChB_CheckedChanged(sender, e);
            }
            if (!ResGrChB.Checked)
            {
                ResGrChB.Checked = true;
                ResGrChb_CheckedChanged(sender, e);
            }
            SaveSettings();
            CHipsBaseSerializer_dll.UnInitialize();
        }

        private void LoadSettings()
        {
            if (File.Exists(SettingsFileName)) {
                string[] strArr = new string[11];
                strArr = File.ReadAllLines(SettingsFileName);
                if (strArr.Length == 11)
                {
                    this.SetDesktopBounds(
                                    Convert.ToInt32(strArr[0]),
                                    Convert.ToInt32(strArr[1]),
                                    Convert.ToInt32(strArr[2]),
                                    Convert.ToInt32(strArr[3])
                                    );

                    splitContainer1.SplitterDistance = Convert.ToInt32(strArr[4]);
                    splitContainer2.SplitterDistance = Convert.ToInt32(strArr[5]);
                    ResListSplit.SplitterDistance = Convert.ToInt32(strArr[6]);
                    AppListSplit.SplitterDistance = Convert.ToInt32(strArr[7]);
                    splitContainer5.SplitterDistance = Convert.ToInt32(strArr[8]);
                    splitContainer6.SplitterDistance = Convert.ToInt32(strArr[9]);
                    ResGrSplit.SplitterDistance = Convert.ToInt32(strArr[10]);
                }
            }
        }

        private void SaveSettings()
        {
            File.Delete(SettingsFileName);
            string str = string.Format("{0}\r\n{1}\r\n{2}\r\n{3}\r\n{4}\r\n{5}\r\n{6}\r\n{7}\r\n{8}\r\n{9}\r\n{10}\r\n",
                                this.DesktopBounds.Left,
                                this.DesktopBounds.Top,
                                this.DesktopBounds.Width,
                                this.DesktopBounds.Height,
                                splitContainer1.SplitterDistance,
                                splitContainer2.SplitterDistance,
                                ResListSplit.SplitterDistance,
                                AppListSplit.SplitterDistance,
                                splitContainer5.SplitterDistance,
                                splitContainer6.SplitterDistance,
                                ResGrSplit.SplitterDistance
                                );
            File.WriteAllText(SettingsFileName, str);
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void ResListChB_CheckedChanged(object sender, EventArgs e)
        {
            if (ResListChB.Checked)
            {
                ResListSplit.SplitterDistance = LastResListSplitDist;
                ResListPnl.Visible = ResListChB.Checked;
            }
            else 
            {
                ResListPnl.Visible = ResListChB.Checked;
                LastResListSplitDist = ResListSplit.SplitterDistance;
                ResListSplit.SplitterDistance = ResListCapPnl.Height;
            }
        }

        private void AppListChB_CheckedChanged(object sender, EventArgs e)
        {
            if (AppListChB.Checked)
            {
                AppListSplit.SplitterDistance = LastAppListSplitDist;
                AppListPnl.Visible = AppListChB.Checked;
            }
            else
            {
                AppListPnl.Visible = AppListChB.Checked;
                LastAppListSplitDist = AppListSplit.SplitterDistance;
                AppListSplit.SplitterDistance = AppListCapPnl.Height;
            }
        }

        private void ResGrChb_CheckedChanged(object sender, EventArgs e)
        {
            if (ResGrChB.Checked)
            {
                ResGrSplit.SplitterDistance = LastResGrSplitDist;
                ResGrPnl.Visible = ResGrChB.Checked;
            }
            else
            {
                ResGrPnl.Visible = ResGrChB.Checked;
                LastResGrSplitDist = ResGrSplit.SplitterDistance;
                ResGrSplit.SplitterDistance = ResGrCapPnl.Height;
            }
        }

        int ConvertFlagToItemNum(uint Value)
        {
            if (Value == CHipsBaseSerializer_dll.HIPS_FLAG_ALLOW)
                return 0;
            else if (Value == CHipsBaseSerializer_dll.HIPS_FLAG_ASK)
                return 1;
            else if (Value == CHipsBaseSerializer_dll.HIPS_FLAG_DENY)
                return 2;
            else
            {
                MessageBox.Show("Wrong flag value");
                return -1;
            }
        }
        uint ConvertItemNumToFlag(int Value)
        {
            switch (Value)
            {
                case 0:
                    return CHipsBaseSerializer_dll.HIPS_FLAG_ALLOW;
                case 1:
                    return CHipsBaseSerializer_dll.HIPS_FLAG_ASK;
                case 2:
                    return CHipsBaseSerializer_dll.HIPS_FLAG_DENY;
            }
            return CHipsBaseSerializer_dll.HIPS_FLAG_ALLOW;
        }

        private void dataGridView6_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == Rule_AccessFlagColumnNumber-1)
            {
                CurrentRow = e.RowIndex;

                string StrValue = RuleListDGW.Rows[CurrentRow].Cells[Rule_AccessFlagColumnNumber - 1].Value.ToString();

                uint Value = 0;
                if (StrValue != "")
                {
                    Value = Convert.ToUInt32(StrValue);
                }

                CMWriteFlag.SelectedIndex = ConvertFlagToItemNum(
                    CHipsBaseSerializer_dll.GetAM(CHipsBaseSerializer_dll.HIPS_POS_WRITE, Value));
                CMLogFlag1.SelectedIndex = Convert.ToInt32(
                    CHipsBaseSerializer_dll.GetLog(CHipsBaseSerializer_dll.HIPS_POS_WRITE, Value));
                CMInhWrite.SelectedIndex = Convert.ToInt32(
                    CHipsBaseSerializer_dll.GetInh(CHipsBaseSerializer_dll.HIPS_POS_WRITE, Value));
                //CMWriteFlag.Enabled = (CMInhWrite.SelectedIndex == 0) ? true : false;

                CMReadFlag.SelectedIndex = ConvertFlagToItemNum(
                    CHipsBaseSerializer_dll.GetAM(CHipsBaseSerializer_dll.HIPS_POS_READ, Value));
                CMLogFlag2.SelectedIndex = Convert.ToInt32(
                    CHipsBaseSerializer_dll.GetLog(CHipsBaseSerializer_dll.HIPS_POS_READ, Value));
                CMInhRead.SelectedIndex = Convert.ToInt32(
                    CHipsBaseSerializer_dll.GetInh(CHipsBaseSerializer_dll.HIPS_POS_READ, Value));
                //CMReadFlag.Enabled = (CMInhRead.SelectedIndex == 0) ? true : false;

                CMEnumFlag.SelectedIndex = ConvertFlagToItemNum(
                    CHipsBaseSerializer_dll.GetAM(CHipsBaseSerializer_dll.HIPS_POS_ENUM, Value));
                CMLogFlag3.SelectedIndex = Convert.ToInt32(
                    CHipsBaseSerializer_dll.GetLog(CHipsBaseSerializer_dll.HIPS_POS_ENUM, Value));
                CMInhEnum.SelectedIndex = Convert.ToInt32(
                    CHipsBaseSerializer_dll.GetInh(CHipsBaseSerializer_dll.HIPS_POS_ENUM, Value));
                //CMEnumFlag.Enabled = (CMInhEnum.SelectedIndex == 0) ? true : false;

                CMDeleteFlag.SelectedIndex = ConvertFlagToItemNum(
                    CHipsBaseSerializer_dll.GetAM(CHipsBaseSerializer_dll.HIPS_POS_DELETE, Value));
                CMLogFlag4.SelectedIndex = Convert.ToInt32(
                    CHipsBaseSerializer_dll.GetLog(CHipsBaseSerializer_dll.HIPS_POS_DELETE, Value));
                CMInhDelete.SelectedIndex = Convert.ToInt32(
                    CHipsBaseSerializer_dll.GetInh(CHipsBaseSerializer_dll.HIPS_POS_DELETE, Value));
                //CMDeleteFlag.Enabled = (CMInhDelete.SelectedIndex == 0) ? true : false;

                contextMenuStrip1.Show(Control.MousePosition);
            }
        }

        private void toolStripMenuItem1_Click_1(object sender, EventArgs e)
        {
            uint ii = 0;
            ii = CHipsBaseSerializer_dll.SetBlockPos(
                             ConvertItemNumToFlag(CMWriteFlag.SelectedIndex),
                             CMLogFlag1.SelectedIndex == 1,
                             CMInhWrite.SelectedIndex == 1,
                             CHipsBaseSerializer_dll.HIPS_POS_WRITE, ii);
            ii = CHipsBaseSerializer_dll.SetBlockPos(
                            ConvertItemNumToFlag(CMReadFlag.SelectedIndex),
                            CMLogFlag2.SelectedIndex == 1,
                            CMInhRead.SelectedIndex == 1,
                            CHipsBaseSerializer_dll.HIPS_POS_READ, ii);
            ii = CHipsBaseSerializer_dll.SetBlockPos(
                            ConvertItemNumToFlag(CMEnumFlag.SelectedIndex),
                            CMLogFlag3.SelectedIndex == 1,
                            CMInhEnum.SelectedIndex == 1,
                            CHipsBaseSerializer_dll.HIPS_POS_ENUM, ii);
            ii = CHipsBaseSerializer_dll.SetBlockPos(
                            ConvertItemNumToFlag(CMDeleteFlag.SelectedIndex),
                            CMLogFlag4.SelectedIndex == 1,
                            CMInhDelete.SelectedIndex == 1,
                            CHipsBaseSerializer_dll.HIPS_POS_DELETE, ii);
            RuleListDGW.Rows[CurrentRow].Cells[Rule_AccessFlagColumnNumber - 1].Value = ii;
        }

        private string SettingsFileName = "settings.dat";
        private string HipsSettingsFileName = "hips.dat";
        private string HipsDBFileName = "hipsDB.xml";

        private int LastResListSplitDist;
        private int LastAppListSplitDist;
        private int LastResGrSplitDist;
        private int CurrentRow;

        private int ResTree_NumberColumnNumber = 1;
        private int ResTree_StateColumnNumber = 2;
        private int ResTree_ParentColumnNumber = 3;
        private int ResTree_TypeColumnNumber = 4;
        private int ResTree_ResRefColumnNumber = 5;
        private int ResTree_NameColumnNumber = 6;
        private int ResTree_ExFlagsColumnNumber = 7;

        private int Res_NumberColumnNumber = 0;
        private int Res_Param1ColumnNumber = 1;
        private int Res_TypeColumnNumber = 2;
        private int Res_Param2ColumnNumber = 3;
        private int Res_DescColumnNumber = 4;
        private int Res_GrIDColumnNumber = 5;

        private int App_NumberColumnNumber = 0;
        private int App_NameColumnNumber = 1;
        private int App_HashColumnNumber = 2;
        private int App_FlagsColumnNumber = 3;
        private int App_GrIdColumnNumber = 4;

        private int WebServ_NumberColumnNumber      = 0;
        private int WebServ_DescColumnNumber        = 1;
        private int WebServ_UseProtocolColumnNumber = 2;
        private int WebServ_ProtocolColumnNumber    = 3;
        private int WebServ_UseTypeColumnNumber     = 4;
        private int WebServ_IcmpTypeColumnNumber    = 5;
        private int WebServ_UseCodeColumnNumber     = 6;
        private int WebServ_IcmpCodeColumnNumber    = 7;
        private int WebServ_DirectionColumnNumber   = 8;
        private int WebServ_RemPortsColumnNumber    = 9;
        private int WebServ_LocPortsColumnNumber    = 10;

        private int AppGr_ServStrNumber = 4;

        private int Rule_NumberColumnNumber = 0;
        private int Rule_EnableColumnNumber     = 1;
        private int Rule_RuleGrColumnNumber     = 2;
        private int Rule_AppIDColumnNumber      = 3;
        private int Rule_AppGrIDColumnNumber    = 4;
        private int Rule_AppFlagsColumnNumber   = 5;
        private int Rule_ResourceIDColumnNumber = 6;
        private int Rule_ResourceID2ColumnNumber = 7;
        private int Rule_AccessFlagColumnNumber = 8;

        private int IPRange_NumberColumnNumber = 0;
        private int IPRange_LocalColumnNumber = 1;
        private int IPRange_RemoteColumnNumber = 2;

        private void dataGridView4_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == 1 && openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                ((DataGridView)sender).Rows[e.RowIndex].Cells[0].Value = openFileDialog1.FileName;
                appTableBindingSource.EndEdit();
            }
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            return;
            if (((ComboBox)sender).SelectedValue == null)
                return;

            if (((ComboBox)sender).SelectedValue.ToString() == "1")
            {
                resTableBindingSource.Filter = "";
            }
            else
            {
                resTableBindingSource.Filter = "ResType = '" + ((ComboBox)sender).SelectedValue.ToString() + "'";
            }
            //rulesTableBindingSource.Filter. = "ResId NOT IN (SELECT ResId FROM ResTable)";// WHERE ResType = 3;

            //rulesTable
            //rulesTableBindingSource.Filter = "ResId NOT IN ('SELECT ResId FROM ResTable')";// WHERE ResType = 3;
            //if (((ComboBox)sender).SelectedValue != null)
            //{
            //    resTableBindingSource.Filter = "ResType = '" + ((ComboBox)sender).SelectedValue.ToString() + "'";
            //    rulesTableBindingSource.Filter = "ResId = '2'";
            //}
        }

        private void RuleListDGW_CellErrorTextNeeded(object sender, DataGridViewCellErrorTextNeededEventArgs e)
        {
        }

        private void ResListDGW_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
        }

        private void dataGridView5_DataError(object sender, DataGridViewDataErrorEventArgs e)
        {
            string str = string.Format("row = {0}, col = {1}, value = {2}",
                                e.RowIndex,
                                e.ColumnIndex,
                                ((DataGridView)sender).Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString());
            MessageBox.Show(str);
            ((DataGridView)sender).Rows[e.RowIndex].Cells[e.ColumnIndex].Value = 1;
        }

        private void RuleListDGW_DataError(object sender, DataGridViewDataErrorEventArgs e)
        {
            return;
            string str = string.Format("row = {0}, col = {1}, value = {2}",
                                e.RowIndex,
                                e.ColumnIndex,
                                ((DataGridView)sender).Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString());
            MessageBox.Show(str);
            ((DataGridView)sender).Rows[e.RowIndex].Cells[e.ColumnIndex].Value = 1;
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {

        }

        private void resTableBindingSource_CurrentChanged(object sender, EventArgs e)
        {

        }

        private void dataGridView2_DataError(object sender, DataGridViewDataErrorEventArgs e)
        {
        }

        private void CMInhWrite_SelectedIndexChanged(object sender, EventArgs e)
        {
            CMWriteFlag.Enabled = (((ToolStripComboBox)sender).SelectedIndex == 0) ? true : false;
        }

        private void CMInhDelete_SelectedIndexChanged(object sender, EventArgs e)
        {
            CMDeleteFlag.Enabled = (((ToolStripComboBox)sender).SelectedIndex == 0) ? true : false;
        }

        private void CMInhEnum_SelectedIndexChanged(object sender, EventArgs e)
        {
            CMEnumFlag.Enabled = (((ToolStripComboBox)sender).SelectedIndex == 0) ? true : false;
        }

        private void CMInhRead_SelectedIndexChanged(object sender, EventArgs e)
        {
            CMReadFlag.Enabled = (((ToolStripComboBox)sender).SelectedIndex == 0) ? true : false;
        }


        private void bsGroupsFromTree_CurrentChanged(object sender, EventArgs e)
        {

        }

    }
}


public class CHipsBaseSerializer_dll
{
    public static uint HIPS_FLAG_ALLOW;
    public static uint HIPS_FLAG_ASK;
    public static uint HIPS_FLAG_DENY;

    public static uint HIPS_FLAG_INHERIT;
    //----------------------------------------------------
    public static uint HIPS_POS_WRITE;
    public static uint HIPS_POS_PERMIS;
    public static uint HIPS_POS_READ;
    public static uint HIPS_POS_ENUM;
    public static uint HIPS_POS_DELETE;
    //----------------------------------------------------
    public static uint HIPS_LOG_OFF;
    public static uint HIPS_LOG_ON;

    public static void AutoInitializeConst()
    {
        InitializeConst(
            out HIPS_FLAG_ALLOW,
            out HIPS_FLAG_ASK,
            out HIPS_FLAG_DENY,
            out HIPS_FLAG_INHERIT,
            out HIPS_POS_WRITE,
            out HIPS_POS_PERMIS,
            out HIPS_POS_READ,
            out HIPS_POS_ENUM,
            out HIPS_POS_DELETE,
            out HIPS_LOG_OFF,
            out HIPS_LOG_ON
            );
        return;
    }
    
    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool InitializeConst(
            out uint HIPS_FLAG_ALLOW,
            out uint HIPS_FLAG_ASK,
            out uint HIPS_FLAG_DENY,
            out uint HIPS_FLAG_INHERIT,
            out uint HIPS_POS_WRITE,
            out uint HIPS_POS_PERMIS,
            out uint HIPS_POS_READ,
            out uint HIPS_POS_ENUM,
            out uint HIPS_POS_DELETE,
            out uint HIPS_LOG_OFF,
            out uint HIPS_LOG_ON
        );

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool Initialize();

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool UnInitialize();

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool AddResourceGroup(uint l_GrID, uint l_ParentGrID, string l_ResGrName, uint l_ExFlags);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool AddAppGroup(uint l_GrID, uint l_ParentGrID, string l_AppGrName, uint l_ServStrType, string l_StrType);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool AddSimpleResource(bool l_IsEnabled, uint l_ExFlags, uint l_ResID, uint l_ResGrID, uint l_ResType, string l_ResDescr, string l_Param1, string l_Param2);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool AddApp(uint l_AppID, uint l_AppGrID, string l_AppName, string l_AppHash, uint l_AppFlags);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool AddWebService(
        bool l_IsEnabled, uint WebServID, uint GrID, string Desc,
        bool l_UsePort, uint Prot,
        bool l_UseType, uint l_IType,
        bool l_UseCode, uint ICode,
        uint Dir, string RemPorts, string LocPorts);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool AddRule(uint l_RuleID, uint l_RuleState, uint l_RuleType, uint l_AppId, uint l_AppGrId, uint l_AppFlag, uint l_ResId, uint l_Res2Id, uint l_AccessFlag);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern bool CompileBase(string l_pBaseFileName);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern uint SetBlockPos(uint am, bool is_log, bool is_inh, uint pos, uint val);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern uint GetInh(uint pos, uint val);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern uint GetLog(uint pos, uint val);

    [DllImport("hips_base_serializer.dll", CharSet = CharSet.Auto)]
    public static extern uint GetAM(uint pos, uint val);
}