// This file has been generated automatically on Âò.29.11.05 17:03:12

// FILE: O:\Prague\Include\pr_serializable.h
DEF_IDENTIFIER( cSerString, 0x00010100 ) // 0x00010100 // DECLARE_IID(cSerString, cSerializable, PID_KERNEL, 1);

// FILE: O:\Prague\Include\structs\s_avp3info.h
DEF_IDENTIFIER( cAVP3FileInfo, 0x94810100 ) // 0x94810100 // DECLARE_IID(cAVP3FileInfo, cSerializable, PID_AVP3INFO, 1) ;
DEF_IDENTIFIER( cAVP3BasesInfo, 0x94810200 ) // 0x94810200 // DECLARE_IID(cAVP3BasesInfo, cSerializable, PID_AVP3INFO, 2) ;

// FILE: O:\Prague\Include\structs\s_ipresolver.h
DEF_IDENTIFIER( cIPResolveResult, 0xEE5B0100 ) // 0xEE5B0100 // DECLARE_IID(cIPResolveResult, cSerializable, PID_RESIP, 1);

// FILE: O:\PPP\Include\structs\s_ahfw.h
DEF_IDENTIFIER( cAHFWCommonStatistics, 0xEE5D6000 ) // 0xEE5D6000 // DECLARE_IID_STATISTICS(cAHFWCommonStatistics, cTaskStatistics, PID_AHFW);
DEF_IDENTIFIER( cAHFWStatistics, 0xEE5D0100 ) // 0xEE5D0100 // DECLARE_IID(cAHFWStatistics, cTaskStatistics, PID_AHFW, _eiid_fw_stat);
DEF_IDENTIFIER( cAHFWConnection, 0xEE5D0300 ) // 0xEE5D0300 // DECLARE_IID(cAHFWConnection, cSerializable, PID_AHFW, _eiid_connections);
DEF_IDENTIFIER( cAHFWNetwork, 0xEE5D1100 ) // 0xEE5D1100 // DECLARE_IID(cAHFWNetwork, cSerializable, PID_AHFW, _eiid_networks);
DEF_IDENTIFIER( cFwNets, 0xEE5D1A00 ) // 0xEE5D1A00 // DECLARE_IID(cFwNets, cSerializable, PID_AHFW, _eiid_cFwNets);
DEF_IDENTIFIER( cFwTransmitSize, 0xEE5D1800 ) // 0xEE5D1800 // DECLARE_IID(cFwTransmitSize, cSerializable, PID_AHFW, _eiid_cFwTransmitSize);
DEF_IDENTIFIER( cFwHostStat, 0xEE5D1900 ) // 0xEE5D1900 // DECLARE_IID(cFwHostStat, cSerializable, PID_AHFW, _eiid_cFwHostStat);
DEF_IDENTIFIER( cAHFWAppConnection, 0xEE5D0400 ) // 0xEE5D0400 // DECLARE_IID(cAHFWAppConnection, cSerializable, PID_AHFW, _eiid_applications);
DEF_IDENTIFIER( cAHFWStatisticsEx, 0xEE5D0200 ) // 0xEE5D0200 // DECLARE_IID(cAHFWStatisticsEx, cTaskStatistics, PID_AHFW, _eiid_fw_statex);
DEF_IDENTIFIER( cFWPort, 0xEE5D0800 ) // 0xEE5D0800 // DECLARE_IID(cFWPort, cSerializable, PID_AHFW, _eiid_port);
DEF_IDENTIFIER( cFWAddress, 0xEE5D0900 ) // 0xEE5D0900 // DECLARE_IID(cFWAddress, cSerializable, PID_AHFW, _eiid_address);
DEF_IDENTIFIER( cFwBaseRule, 0xEE5D1200 ) // 0xEE5D1200 // DECLARE_IID( cFwBaseRule, cSerializable, PID_AHFW, _eiid_base_rule );
DEF_IDENTIFIER( cFwPacketRule, 0xEE5D1300 ) // 0xEE5D1300 // DECLARE_IID( cFwPacketRule, cFwBaseRule, PID_AHFW, _eiid_packet_rule );
DEF_IDENTIFIER( cFwAppRule, 0xEE5D1400 ) // 0xEE5D1400 // DECLARE_IID( cFwAppRule, cSerializable, PID_AHFW, _eiid_app_rule );
DEF_IDENTIFIER( cFwAppRuleEx, 0xEE5D1500 ) // 0xEE5D1500 // DECLARE_IID( cFwAppRuleEx, cFwBaseRule, PID_AHFW, _eiid_app_rule_ex );
DEF_IDENTIFIER( cFwAppRulePreset, 0xEE5D1600 ) // 0xEE5D1600 // DECLARE_IID( cFwAppRulePreset, cSerializable, PID_AHFW, _eiid_app_rule_preset );
DEF_IDENTIFIER( cAHFWSettings, 0xEE5D5000 ) // 0xEE5D5000 // DECLARE_IID_SETTINGS(cAHFWSettings, cTaskSettings, PID_AHFW);
DEF_IDENTIFIER( cAHFWEventReportBase, 0xEE5D0E00 ) // 0xEE5D0E00 // DECLARE_IID(cAHFWEventReportBase, cTaskHeader, PID_AHFW, _eiid_event_report_base);
DEF_IDENTIFIER( cAHFWPacketEventReport, 0xEE5D0F00 ) // 0xEE5D0F00 // DECLARE_IID(cAHFWPacketEventReport, cAHFWEventReportBase, PID_AHFW, _eiid_packet_event_report);
DEF_IDENTIFIER( cAHFWAppEventReport, 0xEE5D1000 ) // 0xEE5D1000 // DECLARE_IID(cAHFWAppEventReport, cAHFWEventReportBase, PID_AHFW, _eiid_app_event_report);
DEF_IDENTIFIER( cAskActionFwLearning, 0xEE5D0B00 ) // 0xEE5D0B00 // DECLARE_IID( cAskActionFwLearning, cAskObjectAction, PID_AHFW,  _eiid_ask_appevent);
DEF_IDENTIFIER( cAHFWAppChanged, 0xEE5D0C00 ) // 0xEE5D0C00 // DECLARE_IID(cAHFWAppChanged, cTaskHeader, PID_AHFW, _eiid_appchanged);
DEF_IDENTIFIER( cAskActionFwAppChanged, 0xEE5D0D00 ) // 0xEE5D0D00 // DECLARE_IID( cAskActionFwAppChanged, cAskObjectAction, PID_AHFW,  _eiid_ask_appchanged);
DEF_IDENTIFIER( cAskActionFwNetChanged, 0xEE5D1700 ) // 0xEE5D1700 // DECLARE_IID(cAskActionFwNetChanged, cAskObjectAction, PID_AHFW, _eiid_network_changed);

// FILE: O:\PPP\Include\structs\s_ahstm.h
DEF_IDENTIFIER( cAHSTMSettings, 0xEE5F5000 ) // 0xEE5F5000 // DECLARE_IID_SETTINGS(cAHSTMSettings, cTaskSettings, PID_AHSTM);

// FILE: O:\PPP\Include\structs\s_antidial.h
DEF_IDENTIFIER( cAntiDialSettings, 0xE2C75000 ) // 0xE2C75000 // DECLARE_IID_SETTINGS(cAntiDialSettings, cTaskSettings, PID_ANTIDIAL);
DEF_IDENTIFIER( cAntiDialStatistics, 0xE2C76000 ) // 0xE2C76000 // DECLARE_IID_STATISTICS(cAntiDialStatistics, cTaskStatistics, PID_ANTIDIAL);
DEF_IDENTIFIER( cAntiDialCheckRequest, 0xE2C70100 ) // 0xE2C70100 // DECLARE_IID(cAntiDialCheckRequest, cSerializable, PID_ANTIDIAL, e_cAntiDialRequest);

// FILE: O:\PPP\Include\structs\s_antiphishing.h
DEF_IDENTIFIER( cAntiPhishingSettings, 0xE2C85000 ) // 0xE2C85000 // DECLARE_IID_SETTINGS(cAntiPhishingSettings, cTaskSettings, PID_ANTIPHISHING);
DEF_IDENTIFIER( cAntiPhishingStatistics, 0xE2C86000 ) // 0xE2C86000 // DECLARE_IID_STATISTICS(cAntiPhishingStatistics, cTaskStatistics, PID_ANTIPHISHING);
DEF_IDENTIFIER( cAntiPhishingCheckRequest, 0xE2C80100 ) // 0xE2C80100 // DECLARE_IID(cAntiPhishingCheckRequest, cSerializable, PID_ANTIPHISHING, e_cAntiPhishingRequest);

// FILE: O:\PPP\Include\structs\s_antispam.h
DEF_IDENTIFIER( cAsBwListRule, 0x9C550900 ) // 0x9C550900 // DECLARE_IID( cAsBwListRule, cSerializable, PID_ANTISPAMTASK, asstruct_cAsBwListRule );
DEF_IDENTIFIER( cAsTrigger, 0x9C550D00 ) // 0x9C550D00 // DECLARE_IID( cAsTrigger, cSerializable, PID_ANTISPAMTASK, asstruct_cAsTrigger );
DEF_IDENTIFIER( cAsBwList, 0x9C550A00 ) // 0x9C550A00 // DECLARE_IID( cAsBwList, cTaskSettings, PID_ANTISPAMTASK, asstruct_cAsBwList );
DEF_IDENTIFIER( cAntispamSettings, 0x9C555000 ) // 0x9C555000 // DECLARE_IID_SETTINGS( cAntispamSettings, cTaskSettingsPerUser, PID_ANTISPAMTASK );
DEF_IDENTIFIER( cAntispamStatistics, 0x9C556000 ) // 0x9C556000 // DECLARE_IID_STATISTICS( cAntispamStatistics, cTaskStatistics, PID_ANTISPAMTASK );
DEF_IDENTIFIER( cAntispamStatisticsEx, 0x9C550500 ) // 0x9C550500 // DECLARE_IID( cAntispamStatisticsEx, cAntispamStatistics, PID_ANTISPAMTASK, asstruct_AS_STATISTICS_EX );
DEF_IDENTIFIER( cAntispamTrainParams, 0x9C550100 ) // 0x9C550100 // DECLARE_IID( cAntispamTrainParams, cSerializable, PID_ANTISPAMTASK, asstruct_AS_TRAIN_PARAMS );
DEF_IDENTIFIER( cAntispamTrainParamsEx, 0x9C550600 ) // 0x9C550600 // DECLARE_IID( cAntispamTrainParamsEx, cSerializable, PID_ANTISPAMTASK, asstruct_AS_TRAIN_PARAMS_EX );
DEF_IDENTIFIER( cAntispamPersistance, 0x9C550300 ) // 0x9C550300 // DECLARE_IID( cAntispamPersistance, cSerializable, PID_ANTISPAMTASK, asstruct_AS_PERSISTANCE );
DEF_IDENTIFIER( cAntispamAskTrain, 0x9C550400 ) // 0x9C550400 // DECLARE_IID( cAntispamAskTrain, cSerializable, PID_ANTISPAMTASK, asstruct_AS_ASK );
DEF_IDENTIFIER( cAntispamAskPlugin, 0x9C550C00 ) // 0x9C550C00 // DECLARE_IID( cAntispamAskPlugin, cSerializable, PID_ANTISPAMTASK, asstruct_AS_ASK_PLUGIN );

// FILE: O:\PPP\Include\structs\s_as.h
DEF_IDENTIFIER( cASSettings, 0x01A50100 ) // 0x01A50100 // DECLARE_IID( cASSettings, cTaskSettings, PID_AS, 1 );

// FILE: O:\PPP\Include\structs\s_as_trainsupport.h
DEF_IDENTIFIER( EntryID_t, 0x9C560E00 ) // 0x9C560E00 // DECLARE_IID(EntryID_t, cSerializable, PID_AS_TRAINSUPPORT, asts_EntryID);
DEF_IDENTIFIER( ObjectInfo_t, 0x9C560F00 ) // 0x9C560F00 // DECLARE_IID(ObjectInfo_t, cSerializable, PID_AS_TRAINSUPPORT, asts_ObjectInfo);
DEF_IDENTIFIER( FolderInfo_t, 0x9C561000 ) // 0x9C561000 // DECLARE_IID(FolderInfo_t, ObjectInfo_t, PID_AS_TRAINSUPPORT, asts_FolderInfo);
DEF_IDENTIFIER( TrainFolderInfo_t, 0x9C561100 ) // 0x9C561100 // DECLARE_IID(TrainFolderInfo_t, FolderInfo_t, PID_AS_TRAINSUPPORT, asts_TrainFolderInfo);
DEF_IDENTIFIER( TrainSettings_t, 0x9C561300 ) // 0x9C561300 // DECLARE_IID(TrainSettings_t, cSerializable, PID_AS_TRAINSUPPORT, asts_TrainSettings);
DEF_IDENTIFIER( TrainStatus_t, 0x9C561200 ) // 0x9C561200 // DECLARE_IID(TrainStatus_t, cSerializable, PID_AS_TRAINSUPPORT, asts_TrainStatus);

// FILE: O:\PPP\Include\structs\s_as_trainwizard.h
DEF_IDENTIFIER( cASWizardSettings, 0x9C570100 ) // 0x9C570100 // DECLARE_IID( cASWizardSettings, cSerializable, PID_AS_TRAINWIZARD, 1 );

// FILE: O:\PPP\Include\structs\s_avs.h
DEF_IDENTIFIER( cObjectPathMask, 0x000F0700 ) // 0x000F0700 // DECLARE_IID( cObjectPathMask, cSerializable, PID_AVS, avsst_cObjectPathMask );
DEF_IDENTIFIER( cDetectExclude, 0x000F0F00 ) // 0x000F0F00 // DECLARE_IID( cDetectExclude, cSerializable, PID_AVS, avsst_cDetectExclude );
DEF_IDENTIFIER( cAVSSettings, 0x000F5000 ) // 0x000F5000 // DECLARE_IID_SETTINGS( cAVSSettings, cTaskSettings, PID_AVS );
DEF_IDENTIFIER( cAVSPerformance, 0x000F6100 ) // 0x000F6100 // DECLARE_IID_STATISTICS_EX( cAVSPerformance, cTaskStatistics, PID_AVS, 1 );
DEF_IDENTIFIER( cAVSStatistics, 0x000F6000 ) // 0x000F6000 // DECLARE_IID_STATISTICS( cAVSStatistics, cAVSPerformance, PID_AVS );
DEF_IDENTIFIER( cScanProcessInfo, 0x000F0100 ) // 0x000F0100 // DECLARE_IID( cScanProcessInfo, cSerializable, PID_AVS, avsst_cScanProcessInfo );
DEF_IDENTIFIER( cProcessCancelRequest, 0x000F1000 ) // 0x000F1000 // DECLARE_IID( cProcessCancelRequest, cTaskHeader, PID_AVS, avsst_cProcessCancelRequest );
DEF_IDENTIFIER( cProtectionStatistics, 0x000F6200 ) // 0x000F6200 // DECLARE_IID_STATISTICS_EX( cProtectionStatistics, cTaskStatistics, PID_AVS, 2 );
DEF_IDENTIFIER( cObjectInfo, 0x000F0400 ) // 0x000F0400 // DECLARE_IID( cObjectInfo, cTaskHeader, PID_AVS, avsst_cObjectInfo );
DEF_IDENTIFIER( cDetectObjectInfo, 0x000F0600 ) // 0x000F0600 // DECLARE_IID( cDetectObjectInfo, cObjectInfo, PID_AVS, avsst_cDetectObjectInfo );
DEF_IDENTIFIER( cInfectedObjectInfo, 0x000F0500 ) // 0x000F0500 // DECLARE_IID( cInfectedObjectInfo, cDetectObjectInfo, PID_AVS, avsst_cInfectedObjectInfo );
DEF_IDENTIFIER( cThreatsInfo, 0x000F0D00 ) // 0x000F0D00 // DECLARE_IID( cThreatsInfo, cSerializable, PID_AVS, avsst_cThreatsInfo );
DEF_IDENTIFIER( cAskProcessTreats, 0x000F0A00 ) // 0x000F0A00 // DECLARE_IID( cAskProcessTreats, cTaskHeader, PID_AVS, avsst_cAskProcessTreats );
DEF_IDENTIFIER( cScanTreatsInfo, 0x000F0E00 ) // 0x000F0E00 // DECLARE_IID( cScanTreatsInfo, cSerializable, PID_AVS, avsst_cScanTreatsInfo );
DEF_IDENTIFIER( cAskObjectAction, 0x000F0800 ) // 0x000F0800 // DECLARE_IID( cAskObjectAction, cInfectedObjectInfo, PID_AVS, avsst_cAskObjectAction );
DEF_IDENTIFIER( cAskObjectPassword, 0x000F0900 ) // 0x000F0900 // DECLARE_IID( cAskObjectPassword, cAskObjectAction, PID_AVS, avsst_cAskObjectPassword );
DEF_IDENTIFIER( cAskQBAction, 0x000F0C00 ) // 0x000F0C00 // DECLARE_IID( cAskQBAction, cAskObjectAction, PID_AVS, avsst_cAskQBAction );

// FILE: O:\PPP\Include\structs\s_bl.h
DEF_IDENTIFIER( cBLNotification, 0x00228800 ) // 0x00228800 // DECLARE_IID( cBLNotification, cSerializable, PID_BL, blst_cBLNotification );
DEF_IDENTIFIER( cBLLocalizedNotification, 0x00228900 ) // 0x00228900 // DECLARE_IID( cBLLocalizedNotification, cBLNotification, PID_BL, blst_cBLLocalizedNotification );
DEF_IDENTIFIER( cBLNotifySettings, 0x00228600 ) // 0x00228600 // DECLARE_IID( cBLNotifySettings, cSerializable, PID_BL, blst_cBLNotifySettings );
DEF_IDENTIFIER( cBLSettings, 0x00220100 ) // 0x00220100 // DECLARE_IID( cBLSettings, cTaskSettings, PID_BL, blst_cBLSettings );
DEF_IDENTIFIER( cBLStatistics, 0x00220200 ) // 0x00220200 // DECLARE_IID( cBLStatistics, cTaskStatistics, PID_BL, blst_cBLStatistics);
DEF_IDENTIFIER( cBLUpdateWhatsNew, 0x00228500 ) // 0x00228500 // DECLARE_IID( cBLUpdateWhatsNew, cSerializable, PID_BL, blst_cBLUpdateWhatsNew );
DEF_IDENTIFIER( cBLAppInfo, 0x00228700 ) // 0x00228700 // DECLARE_IID( cBLAppInfo, cSerializable, PID_BL, blst_cBLAppInfo );
DEF_IDENTIFIER( cUpdaterProductSpecific, 0x00228A00 ) // 0x00228A00 // DECLARE_IID( cUpdaterProductSpecific, cTaskBLSettings, PID_BL, blst_cUpdaterProductSpecific );

// FILE: O:\PPP\Include\structs\s_gui.h
DEF_IDENTIFIER( cAskGuiAction, 0x00130100 ) // 0x00130100 // DECLARE_IID( cAskGuiAction, cSerializable, PID_GUI, gst_AskGuiAction );
DEF_IDENTIFIER( cProfileAction, 0x00130200 ) // 0x00130200 // DECLARE_IID( cProfileAction, cTaskHeader, PID_GUI, gst_cProfileAction );
DEF_IDENTIFIER( cAskScanObjects, 0x00130300 ) // 0x00130300 // DECLARE_IID( cAskScanObjects, cTaskHeader, PID_GUI, gst_cAskScanObjects );
DEF_IDENTIFIER( cBrowseFile, 0x00130400 ) // 0x00130400 // DECLARE_IID( cBrowseFile, cSerializable, PID_GUI, gst_cBrowseFile );
DEF_IDENTIFIER( cGuiErrorInfo, 0x00130500 ) // 0x00130500 // DECLARE_IID( cGuiErrorInfo, cSerializable, PID_GUI, gst_cGuiErrorInfo );
DEF_IDENTIFIER( cLicErrorInfo, 0x00130600 ) // 0x00130600 // DECLARE_IID( cLicErrorInfo, cGuiErrorInfo, PID_GUI, gst_cLicErrorInfo );
DEF_IDENTIFIER( cLicExpirationInfo, 0x00131300 ) // 0x00131300 // DECLARE_IID( cLicExpirationInfo, cSerializable, PID_GUI, gst_cLicExpirationInfo );
DEF_IDENTIFIER( cQBErrorInfo, 0x00130700 ) // 0x00130700 // DECLARE_IID( cQBErrorInfo, cGuiErrorInfo, PID_GUI, gst_cQBErrorInfo );
DEF_IDENTIFIER( cFWAddressEx, 0x00130D00 ) // 0x00130D00 // DECLARE_IID( cFWAddressEx, cSerializable, PID_GUI, gst_cFWAddressEx );
DEF_IDENTIFIER( cFWLearnHlp, 0x00130E00 ) // 0x00130E00 // DECLARE_IID( cFWLearnHlp, cSerializable, PID_GUI, gst_cFWLearnHlp );
DEF_IDENTIFIER( cObjInfo, 0x00132B00 ) // 0x00132B00 // DECLARE_IID( cObjInfo, cSerializable, PID_GUI, gst_cObjInfo );
DEF_IDENTIFIER( cGuiReportExportInfo, 0x00130F00 ) // 0x00130F00 // DECLARE_IID( cGuiReportExportInfo, cSerializable, PID_GUI, gst_cGuiReportExportInfo );
DEF_IDENTIFIER( cGuiReportExportQuery, 0x00131600 ) // 0x00131600 // DECLARE_IID( cGuiReportExportQuery, cSerializable, PID_GUI, gst_cGuiReportExportQuery );
DEF_IDENTIFIER( cGuiExceptTaskInfo, 0x00131000 ) // 0x00131000 // DECLARE_IID( cGuiExceptTaskInfo, cSerializable, PID_GUI, gst_cGuiExceptTaskInfo );
DEF_IDENTIFIER( cGuiTaskStatistics, 0x00131100 ) // 0x00131100 // DECLARE_IID( cGuiTaskStatistics, cSerializable, PID_GUI, gst_cGuiTaskStatistics );
DEF_IDENTIFIER( cGuiGlobals, 0x00131200 ) // 0x00131200 // DECLARE_IID( cGuiGlobals, cSerializable, PID_GUI, gst_cGuiGlobals );
DEF_IDENTIFIER( cGuiMailwasherSettings, 0x00131B00 ) // 0x00131B00 // DECLARE_IID( cGuiMailwasherSettings, cSerializable, PID_GUI, gst_cGuiMailwasherSettings );
DEF_IDENTIFIER( cGuiMailwasher, 0x00131C00 ) // 0x00131C00 // DECLARE_IID( cGuiMailwasher, cGuiMailwasherSettings, PID_GUI, gst_cGuiMailwasher );
DEF_IDENTIFIER( cGuiVariables, 0x00131A00 ) // 0x00131A00 // DECLARE_IID( cGuiVariables, cSerializable, PID_GUI, gst_cGuiVariables );
DEF_IDENTIFIER( cGuiSettings, 0x00132400 ) // 0x00132400 // DECLARE_IID( cGuiSettings, cSerializable, PID_GUI, gst_cGuiSettings );
DEF_IDENTIFIER( cGuiPauseProfile, 0x00131400 ) // 0x00131400 // DECLARE_IID( cGuiPauseProfile, cSerializable, PID_GUI, gst_cGuiPauseProfile );
DEF_IDENTIFIER( cGuiOgMacros, 0x00131500 ) // 0x00131500 // DECLARE_IID( cGuiOgMacros, cSerializable, PID_GUI, gst_cGuiOgMacros );
DEF_IDENTIFIER( cResetItem, 0x00131700 ) // 0x00131700 // DECLARE_IID( cResetItem, cSerializable, PID_GUI, gst_cResetItem );
DEF_IDENTIFIER( cGuiResetArea, 0x00131800 ) // 0x00131800 // DECLARE_IID( cGuiResetArea, cSerializable, PID_GUI, gst_cGuiResetArea );
DEF_IDENTIFIER( cGuiAhNetActivity, 0x00131900 ) // 0x00131900 // DECLARE_IID( cGuiAhNetActivity, cAHFWConnection, PID_GUI, gst_cGuiAhNetActivity );
DEF_IDENTIFIER( cGuiPswrdProtection, 0x00131D00 ) // 0x00131D00 // DECLARE_IID( cGuiPswrdProtection, cSerializable, PID_GUI, gst_cGuiPswrdProtection );
DEF_IDENTIFIER( cRegKeyInfo, 0x00131E00 ) // 0x00131E00 // DECLARE_IID( cRegKeyInfo, cSerializable, PID_GUI, gst_cRegKeyInfo );
DEF_IDENTIFIER( cProgressInfo, 0x00131F00 ) // 0x00131F00 // DECLARE_IID( cProgressInfo, cSerializable, PID_GUI, gst_cProgressInfo );
DEF_IDENTIFIER( cGuiCurProfile, 0x00132000 ) // 0x00132000 // DECLARE_IID( cGuiCurProfile, cSerializable, PID_GUI, gst_cGuiCurProfile );
DEF_IDENTIFIER( cGuiEvent, 0x00132100 ) // 0x00132100 // DECLARE_IID( cGuiEvent, cSerializable, PID_GUI, gst_cGuiEvent );
DEF_IDENTIFIER( cDataFiles, 0x00132200 ) // 0x00132200 // DECLARE_IID( cDataFiles, cSerializable, PID_GUI, gst_cDataFiles );
DEF_IDENTIFIER( cDataFilesCleanup, 0x00132300 ) // 0x00132300 // DECLARE_IID( cDataFilesCleanup, cSerializable, PID_GUI, gst_cDataFilesCleanup );
DEF_IDENTIFIER( cGuiAskActionInfo, 0x00132C00 ) // 0x00132C00 // DECLARE_IID( cGuiAskActionInfo, cSerializable, PID_GUI, gst_cGuiAskActionInfo );
DEF_IDENTIFIER( cUpdateFilterListItem, 0x00132D00 ) // 0x00132D00 // DECLARE_IID( cUpdateFilterListItem, cSerializable, PID_GUI, gst_cUpdateFilterListItem );
DEF_IDENTIFIER( cConfigureProduct, 0x00132E00 ) // 0x00132E00 // DECLARE_IID( cConfigureProduct, cSerializable, PID_GUI, gst_cConfigureProduct );
DEF_IDENTIFIER( cMsgParams, 0x00133100 ) // 0x00133100 // DECLARE_IID( cMsgParams, cSerializable, PID_GUI, gst_cMsgParams );
DEF_IDENTIFIER( cProtectionNotify, 0x00133200 ) // 0x00133200 // DECLARE_IID( cProtectionNotify, cTaskHeader, PID_GUI, gst_cProtectionNotify );

// FILE: O:\PPP\Include\structs\s_httpprotocoller.h
DEF_IDENTIFIER( cHTTPSettings, 0x985D5000 ) // 0x985D5000 // DECLARE_IID_SETTINGS( cHTTPSettings, cSerializable, PID_HTTPPROTOCOLLER );

// FILE: O:\PPP\Include\structs\s_httpscan.h
DEF_IDENTIFIER( cHTTPScanSettings, 0xBF875000 ) // 0xBF875000 // DECLARE_IID_SETTINGS( cHTTPScanSettings, cProtectionSettings, PID_HTTPSCAN );
DEF_IDENTIFIER( cHTTPScanProcessParams, 0xBF870000 ) // 0xBF870000 // DECLARE_IID( cHTTPScanProcessParams, cSerializable, PID_HTTPSCAN, hss_PROCESS_PARAMS );
DEF_IDENTIFIER( cHTTPCancelSettings, 0xBF870100 ) // 0xBF870100 // DECLARE_IID( cHTTPCancelSettings, cSerializable, PID_HTTPSCAN, hss_CANCEL_PARAMS );
DEF_IDENTIFIER( cHTTPAskObjectAction, 0xBF870200 ) // 0xBF870200 // DECLARE_IID( cHTTPAskObjectAction, cAskObjectAction, PID_HTTPSCAN, hss_ASK_ACTION );

// FILE: O:\PPP\Include\structs\s_ids.h
DEF_IDENTIFIER( cIDSSettings, 0xEE4F5000 ) // 0xEE4F5000 // DECLARE_IID_SETTINGS(cIDSSettings, cTaskSettings, PID_IDSTASK);
DEF_IDENTIFIER( cIdsBannedHost, 0xEE4F0300 ) // 0xEE4F0300 // DECLARE_IID(cIdsBannedHost, cSerializable, PID_IDSTASK, _ids_cIdsBannedHost);
DEF_IDENTIFIER( cIDSStatistics, 0xEE4F6000 ) // 0xEE4F6000 // DECLARE_IID_STATISTICS(cIDSStatistics, cTaskStatistics, PID_IDSTASK);
DEF_IDENTIFIER( cIDSEventReport, 0xEE4F7000 ) // 0xEE4F7000 // DECLARE_IID(cIDSEventReport, cTaskHeader, PID_IDSTASK, 0x70);

// FILE: O:\PPP\Include\structs\s_licensing.h
DEF_IDENTIFIER( cKeySerialNumber, 0x947D0000 ) // 0x947D0000 // DECLARE_IID(cKeySerialNumber, cSerializable, PID_LICENSING60, 0) ;
DEF_IDENTIFIER( cLicensedObject, 0x947D0100 ) // 0x947D0100 // DECLARE_IID(cLicensedObject, cSerializable, PID_LICENSING60, 1) ;
DEF_IDENTIFIER( cComponentsLevel, 0x947D0200 ) // 0x947D0200 // DECLARE_IID(cComponentsLevel, cSerializable, PID_LICENSING60, 2) ;
DEF_IDENTIFIER( cKeyInfo, 0x947D0300 ) // 0x947D0300 // DECLARE_IID(cKeyInfo, cSerializable, PID_LICENSING60, 3) ;
DEF_IDENTIFIER( cCheckInfo, 0x947D0400 ) // 0x947D0400 // DECLARE_IID(cCheckInfo, cSerializable, PID_LICENSING60, 4) ;
DEF_IDENTIFIER( cCheckInfoList, 0x947D0500 ) // 0x947D0500 // DECLARE_IID(cCheckInfoList, cSerializable, PID_LICENSING60, 5) ;
DEF_IDENTIFIER( cUserData, 0x947D0600 ) // 0x947D0600 // DECLARE_IID(cUserData, cSerializable, PID_LICENSING60, 6) ;
DEF_IDENTIFIER( cAuthInfo, 0x947D0700 ) // 0x947D0700 // DECLARE_IID(cAuthInfo, cSerializable, PID_LICENSING60, 7) ;

// FILE: O:\PPP\Include\structs\s_licmgr.h
DEF_IDENTIFIER( cLicMgrSettings, 0xEE5C5000 ) // 0xEE5C5000 // DECLARE_IID_SETTINGS(cLicMgrSettings, cTaskSettings, PID_LICMGR);

// FILE: O:\PPP\Include\structs\s_mailwasher.h
DEF_IDENTIFIER( cPOP3Session, 0x9C5B0000 ) // 0x9C5B0000 // DECLARE_IID(cPOP3Session, cSerializable, PID_MAILWASHER, msid_Session);
DEF_IDENTIFIER( cPOP3MessageReguest, 0x9C5B0300 ) // 0x9C5B0300 // DECLARE_IID(cPOP3MessageReguest, cPOP3Session, PID_MAILWASHER, msid_MessageReguest);
DEF_IDENTIFIER( cPOP3Message, 0x9C5B0100 ) // 0x9C5B0100 // DECLARE_IID(cPOP3Message, cPOP3Session, PID_MAILWASHER, msid_Message);
DEF_IDENTIFIER( cPOP3MessageDecode, 0x9C5B0400 ) // 0x9C5B0400 // DECLARE_IID(cPOP3MessageDecode, cPOP3Message, PID_MAILWASHER, msid_MessageDecode);
DEF_IDENTIFIER( cPOP3MessageList, 0x9C5B0200 ) // 0x9C5B0200 // DECLARE_IID(cPOP3MessageList, cSerializable, PID_MAILWASHER, msid_MessageList);

// FILE: O:\PPP\Include\structs\s_mc.h
DEF_IDENTIFIER( cMCSettings, 0x9C445000 ) // 0x9C445000 // DECLARE_IID_SETTINGS( cMCSettings, cProtectionSettings, PID_MCTASK );
DEF_IDENTIFIER( cMCProcessParams, 0x9C440800 ) // 0x9C440800 // DECLARE_IID( cMCProcessParams, cSerializable, PID_MCTASK, mcstruct_MC_PROCESS_PARAMS );
DEF_IDENTIFIER( cMCPluginSettings, 0x9C440900 ) // 0x9C440900 // DECLARE_IID(cMCPluginSettings, cSerializable, PID_MCTASK, mcstruct_MC_PluginSettings);

// FILE: O:\PPP\Include\structs\s_mcou_antispam.h
DEF_IDENTIFIER( AntispamPluginRule_t, 0x9C550E00 ) // 0x9C550E00 // DECLARE_IID(AntispamPluginRule_t, cSerializable, PID_ANTISPAMTASK, apsid_PluginRule);
DEF_IDENTIFIER( Request_t, 0x9C551000 ) // 0x9C551000 // DECLARE_IID(Request_t, cSerializable, PID_ANTISPAMTASK, apsid_Request);
DEF_IDENTIFIER( AntispamPluginSettings_t, 0x9C550F00 ) // 0x9C550F00 // DECLARE_IID(AntispamPluginSettings_t, cSerializable, PID_ANTISPAMTASK, apsid_PluginSettings);
DEF_IDENTIFIER( cAntispamPluginStatus, 0x9C551100 ) // 0x9C551100 // DECLARE_IID(cAntispamPluginStatus, cSerializable, PID_ANTISPAMTASK, apsid_PluginStatus);

// FILE: O:\PPP\Include\structs\s_mc_oe.h
DEF_IDENTIFIER( cMCOESettings, 0x9C515000 ) // 0x9C515000 // DECLARE_IID_SETTINGS( cMCOESettings, cTaskSettings, PID_OUTLOOKPLUGINTASK );
DEF_IDENTIFIER( cMCOEStatistics, 0x9C516000 ) // 0x9C516000 // DECLARE_IID_STATISTICS( cMCOEStatistics, cTaskStatistics, PID_OUTLOOKPLUGINTASK );

// FILE: O:\PPP\Include\structs\s_mc_trafficmonitor.h
DEF_IDENTIFIER( cTmPort, 0x9C4C0000 ) // 0x9C4C0000 // DECLARE_IID( cTmPort, cSerializable, PID_TRAFFICMONITOR, tmst_cTmPort );
DEF_IDENTIFIER( cTmExclude, 0x9C4C0100 ) // 0x9C4C0100 // DECLARE_IID( cTmExclude, cSerializable, PID_TRAFFICMONITOR, tmst_cTmExclude );
DEF_IDENTIFIER( cTrafficMonitorSettings, 0x9C4C5000 ) // 0x9C4C5000 // DECLARE_IID_SETTINGS( cTrafficMonitorSettings, cTaskSettings, PID_TRAFFICMONITOR );
DEF_IDENTIFIER( cTrafficMonitorStatistics, 0x9C4C6000 ) // 0x9C4C6000 // DECLARE_IID_STATISTICS( cTrafficMonitorStatistics, cTaskStatistics, PID_TRAFFICMONITOR );
DEF_IDENTIFIER( cTrafficMonitorAskBeforeStop, 0x9C4C0200 ) // 0x9C4C0200 // DECLARE_IID( cTrafficMonitorAskBeforeStop, cTaskHeader, PID_TRAFFICMONITOR, tmst_cTrafficMonitorAskBeforeStop );
DEF_IDENTIFIER( cProtocollerAskBeforeStop, 0x9C4C0300 ) // 0x9C4C0300 // DECLARE_IID( cProtocollerAskBeforeStop, cTaskHeader, PID_TRAFFICMONITOR, tmst_cProtocollerAskBeforeStop );

// FILE: O:\PPP\Include\structs\s_offguard.h
DEF_IDENTIFIER( cOffGuardRequestData, 0xE2BD0100 ) // 0xE2BD0100 // DECLARE_IID( cOffGuardRequestData, cSerializable, PID_OFFGUARD, 1 );
DEF_IDENTIFIER( cOffGuardSettings, 0xE2BD5000 ) // 0xE2BD5000 // DECLARE_IID_SETTINGS( cOffGuardSettings, cTaskSettings, PID_OFFGUARD);
DEF_IDENTIFIER( cOffGuardStatistics, 0xE2BD6000 ) // 0xE2BD6000 // DECLARE_IID_STATISTICS( cOffGuardStatistics, cTaskStatistics, PID_OFFGUARD );

// FILE: O:\PPP\Include\structs\s_pdm.h
DEF_IDENTIFIER( cPdmAppMonitoring_Data, 0xE2BE0800 ) // 0xE2BE0800 // DECLARE_IID(cPdmAppMonitoring_Data, cSerializable, PID_PDM, e_cPdmAppMonitoring_Data);
DEF_IDENTIFIER( cPdmAppMonitoring_Item, 0xE2BE0700 ) // 0xE2BE0700 // DECLARE_IID(cPdmAppMonitoring_Item, cSerializable, PID_PDM, e_cPdmAppMonitoring_Item);
DEF_IDENTIFIER( cPDMSettings_Item, 0xE2BE0D00 ) // 0xE2BE0D00 // DECLARE_IID(cPDMSettings_Item, cSerializable, PID_PDM, e_cPdmSettingsItem);
DEF_IDENTIFIER( cPDMSettings, 0xE2BE5000 ) // 0xE2BE5000 // DECLARE_IID_SETTINGS(cPDMSettings, cTaskSettings, PID_PDM);
DEF_IDENTIFIER( cPDMStatistics, 0xE2BE6000 ) // 0xE2BE6000 // DECLARE_IID_STATISTICS(cPDMStatistics, cTaskStatistics, PID_PDM);
DEF_IDENTIFIER( cPdmHistoryEntry, 0xE2BE0300 ) // 0xE2BE0300 // DECLARE_IID(cPdmHistoryEntry, cSerializable, PID_PDM, e_cPdmHistoryEntry);
DEF_IDENTIFIER( cPdmHistory, 0xE2BE0400 ) // 0xE2BE0400 // DECLARE_IID(cPdmHistory, cSerializable, PID_PDM, e_cPdmHistory);
DEF_IDENTIFIER( cPdmReportInfo, 0xE2BE0500 ) // 0xE2BE0500 // DECLARE_IID(cPdmReportInfo, cSerializable, PID_PDM, e_cPdmReportInfo);
DEF_IDENTIFIER( cPdmProcList, 0xE2BE0900 ) // 0xE2BE0900 // DECLARE_IID(cPdmProcList, cSerializable, PID_PDM, e_cPdmProcList);
DEF_IDENTIFIER( cAskObjectAction_PDM_BASE, 0xE2BE0200 ) // 0xE2BE0200 // DECLARE_IID(cAskObjectAction_PDM_BASE, cAskObjectAction, PID_PDM, e_cAskObjectAction_PDM_BASE);
DEF_IDENTIFIER( cAskObjectAction_PDM_A, 0xE2BE0B00 ) // 0xE2BE0B00 // DECLARE_IID(cAskObjectAction_PDM_A, cAskObjectAction_PDM_BASE, PID_PDM, e_cAskObjectAction_PDM_A);
DEF_IDENTIFIER( cAskObjectAction_REG, 0xE2BE0C00 ) // 0xE2BE0C00 // DECLARE_IID(cAskObjectAction_REG, cAskObjectAction_PDM_BASE, PID_PDM, e_cAskObjectAction_REG);

// FILE: O:\PPP\Include\structs\s_popupchk.h
DEF_IDENTIFIER( cPopupCheckSettings, 0xE2C65000 ) // 0xE2C65000 // DECLARE_IID_SETTINGS(cPopupCheckSettings, cTaskSettings, PID_POPUPCHK);
DEF_IDENTIFIER( cPopupCheckStatistics, 0xE2C66000 ) // 0xE2C66000 // DECLARE_IID_STATISTICS(cPopupCheckStatistics, cTaskStatistics, PID_POPUPCHK);
DEF_IDENTIFIER( cPopupCheckRequest, 0xE2C60100 ) // 0xE2C60100 // DECLARE_IID(cPopupCheckRequest, cInfectedObjectInfo, PID_POPUPCHK, e_cPopupCheckRequest);

// FILE: O:\PPP\Include\structs\s_procmon.h
DEF_IDENTIFIER( cBLTrustedApp, 0xBF8B0000 ) // 0xBF8B0000 // DECLARE_IID( cBLTrustedApp, cSerializable, PID_PROCESSMONITOR, pmst_cBLTrustedApp );
DEF_IDENTIFIER( cBLTrustedApps, 0xBF8B5000 ) // 0xBF8B5000 // DECLARE_IID_SETTINGS( cBLTrustedApps, cSerializable, PID_PROCESSMONITOR);
DEF_IDENTIFIER( cModuleInfo, 0xBF8B0200 ) // 0xBF8B0200 // DECLARE_IID( cModuleInfo, cSerializable, PID_PROCESSMONITOR, pmst_cModuleInfo );
DEF_IDENTIFIER( cModuleInfoHash, 0xBF8B0300 ) // 0xBF8B0300 // DECLARE_IID( cModuleInfoHash, cModuleInfo, PID_PROCESSMONITOR, pmst_cModuleInfoHash );

// FILE: O:\PPP\Include\structs\s_qb.h
DEF_IDENTIFIER( cQBObject, 0x00120100 ) // 0x00120100 // DECLARE_IID( cQBObject, cDetectObjectInfo, PID_QB, 1 );

// FILE: O:\PPP\Include\structs\s_scaner.h
DEF_IDENTIFIER( cScanObject, 0xBF7E0200 ) // 0xBF7E0200 // DECLARE_IID( cScanObject, cSerializable, PID_ODS, 2 );
DEF_IDENTIFIER( cScanObjects, 0xBF7F0300 ) // 0xBF7F0300 // DECLARE_IID( cScanObjects, cProtectionSettings, PID_OAS, 3 );
DEF_IDENTIFIER( cOASSettings, 0xBF7F5000 ) // 0xBF7F5000 // DECLARE_IID_SETTINGS( cOASSettings, cScanObjects, PID_OAS );
DEF_IDENTIFIER( cODSSettings, 0xBF7E5000 ) // 0xBF7E5000 // DECLARE_IID_SETTINGS( cODSSettings, cScanObjects, PID_ODS );
DEF_IDENTIFIER( cODSStatistics, 0xBF7E0500 ) // 0xBF7E0500 // DECLARE_IID( cODSStatistics, cProtectionStatistics, PID_ODS, 5 );
DEF_IDENTIFIER( cODSObjectStatistics, 0xBF7E0600 ) // 0xBF7E0600 // DECLARE_IID( cODSObjectStatistics, cProtectionStatistics, PID_ODS, 6 );
DEF_IDENTIFIER( cODSStatisticsEx, 0xBF7E6000 ) // 0xBF7E6000 // DECLARE_IID_STATISTICS( cODSStatisticsEx, cODSStatistics, PID_ODS );
DEF_IDENTIFIER( cODSRuntimeParams, 0xBF7E0700 ) // 0xBF7E0700 // DECLARE_IID( cODSRuntimeParams, cSerializable, PID_ODS, 7 );

// FILE: O:\PPP\Include\structs\s_scheduler.h
DEF_IDENTIFIER( cScheduleSettings, 0xE2B80100 ) // 0xE2B80100 // DECLARE_IID( cScheduleSettings, cSerializable, PID_SCHEDULER, 1 );
DEF_IDENTIFIER( cTaskSchedule, 0xE2B80200 ) // 0xE2B80200 // DECLARE_IID( cTaskSchedule, cSerializable, PID_SCHEDULER, 2 );

// FILE: O:\PPP\Include\structs\s_settings.h
DEF_IDENTIFIER( cTaskSettings, 0x00000200 ) // 0x00000200 // DECLARE_IID( cTaskSettings, cSerializable, PID_ANY, setst_cTaskSettings );
DEF_IDENTIFIER( cTaskBLSettings, 0x00000100 ) // 0x00000100 // DECLARE_IID( cTaskBLSettings, cTaskSettings, PID_ANY, setst_cTaskBLSettings );
DEF_IDENTIFIER( cEnabledStrItem, 0x00000300 ) // 0x00000300 // DECLARE_IID( cEnabledStrItem, cSerializable, PID_ANY, setst_cEnabledStrItem );
DEF_IDENTIFIER( cTaskSettingsPerUserItem, 0x00000400 ) // 0x00000400 // DECLARE_IID( cTaskSettingsPerUserItem, cSerializable, PID_ANY, setst_cTaskSettingsPerUserItem );
DEF_IDENTIFIER( cTaskSettingsPerUser, 0x00000500 ) // 0x00000500 // DECLARE_IID( cTaskSettingsPerUser, cTaskSettings, PID_ANY, setst_cTaskSettingsPerUser );

// FILE: O:\PPP\Include\structs\s_spamtest.h
DEF_IDENTIFIER( cSpamtestSettings, 0x9C5E5000 ) // 0x9C5E5000 // DECLARE_IID_SETTINGS( cSpamtestSettings, cTaskSettings, PID_SPAMTEST );
DEF_IDENTIFIER( cSpamtestParams, 0x9C5E0100 ) // 0x9C5E0100 // DECLARE_IID( cSpamtestParams, cSerializable, PID_SPAMTEST, 1 );

// FILE: O:\PPP\Include\structs\s_taskmanager.h
DEF_IDENTIFIER( cTaskInfo, 0x00160000 ) // 0x00160000 // DECLARE_IID( cTaskInfo, cSerializable, PID_TM, tms_cTaskInfo );
DEF_IDENTIFIER( cTaskHeader, 0x00161600 ) // 0x00161600 // DECLARE_IID( cTaskHeader, cSerializable, PID_TM, tms_cTaskHeader );
DEF_IDENTIFIER( cTaskParams, 0x00161200 ) // 0x00161200 // DECLARE_IID( cTaskParams, cSerializable, PID_TM, tms_cTaskParams );
DEF_IDENTIFIER( cTaskStatistics, 0x00160200 ) // 0x00160200 // DECLARE_IID( cTaskStatistics, cSerializable, PID_TM, tms_cTaskStatistics );
DEF_IDENTIFIER( cTaskStatisticsListItem, 0x00160400 ) // 0x00160400 // DECLARE_IID( cTaskStatisticsListItem, cSerializable, PID_TM, tms_cTaskStatisticsListItem );
DEF_IDENTIFIER( cTaskStatisticsList, 0x00160300 ) // 0x00160300 // DECLARE_IID( cTaskStatisticsList, cSerializable, PID_TM, tms_cTaskStatisticsList );
DEF_IDENTIFIER( cTaskRunAs, 0x00161400 ) // 0x00161400 // DECLARE_IID( cTaskRunAs, cSerializable, PID_TM, tms_cTaskRunAs );
DEF_IDENTIFIER( cCfg, 0x00160A00 ) // 0x00160A00 // DECLARE_IID( cCfg, cTaskSettings, PID_TM, tms_cCfg );
DEF_IDENTIFIER( cCfgEx, 0x00160B00 ) // 0x00160B00 // DECLARE_IID( cCfgEx, cCfg, PID_TM, tms_cCfgEx );
DEF_IDENTIFIER( cTaskReportDsc, 0x00160600 ) // 0x00160600 // DECLARE_IID( cTaskReportDsc, cSerializable, PID_TM, tms_cTaskReportDsc );
DEF_IDENTIFIER( cProfileBase, 0x00160C00 ) // 0x00160C00 // DECLARE_IID( cProfileBase, cSerializable, PID_TM, tms_cProfileBase );
DEF_IDENTIFIER( cProfile, 0x00160D00 ) // 0x00160D00 // DECLARE_IID( cProfile, cProfileBase, PID_TM, tms_cProfile );
DEF_IDENTIFIER( cProfileExList, 0x00161000 ) // 0x00161000 // DECLARE_IID( cProfileExList, cSerializable, PID_TM, tms_cProfileExList );
DEF_IDENTIFIER( cProfileEx, 0x00160E00 ) // 0x00160E00 // DECLARE_IID( cProfileEx, cProfile, PID_TM, tms_cProfileEx );
DEF_IDENTIFIER( cReportInfo, 0x00160700 ) // 0x00160700 // DECLARE_IID( cReportInfo, cSerializable, PID_TM, tms_cReportInfo );
DEF_IDENTIFIER( cCloseRequest, 0x00161500 ) // 0x00161500 // DECLARE_IID( cCloseRequest, cSerializable, PID_TM, tms_cCloseRequest );

// FILE: O:\PPP\Include\structs\s_updater2005.h
DEF_IDENTIFIER( cUpdaterStatistics, 0x94806000 ) // 0x94806000 // DECLARE_IID_STATISTICS(cUpdater2005Statistics, cTaskStatistics, PID_UPDATER2005);
DEF_IDENTIFIER( cUpdaterSource, 0x94800100 ) // 0x94800100 // DECLARE_IID(cUpdaterSource, cSerializable, PID_UPDATER2005, 1);
DEF_IDENTIFIER( cUpdaterEvent, 0x94800200 ) // 0x94800200 // DECLARE_IID(cUpdaterEvent, cSerializable, PID_UPDATER2005, 2);

// FILE: O:\PPP\Include\structs\s_updater2005settings.h
DEF_IDENTIFIER( cUpdaterSettings, 0x94805000 ) // 0x94805000 // DECLARE_IID_SETTINGS(cUpdaterSettings, cTaskSettings, PID_UPDATER2005);
DEF_IDENTIFIER( cProxyCredentialsRequest, 0x94800500 ) // 0x94800500 // DECLARE_IID(cProxyCredentialsRequest, cTaskHeader, PID_UPDATER2005, 5) ;

// FILE: O:\PPP\MailCommon\Interceptors\TrafficMonitor2\TrafficProtocoller.h
DEF_IDENTIFIER( session_t, 0x9C4C6400 ) // 0x9C4C6400 // DECLARE_IID(session_t, cSerializable, PID_TRAFFICMONITOR, 100) ;
DEF_IDENTIFIER( detect_t, 0x9C4C6500 ) // 0x9C4C6500 // DECLARE_IID(detect_t, session_t, PID_TRAFFICMONITOR, 101) ;
DEF_IDENTIFIER( detect_ex_t, 0x9C4C6600 ) // 0x9C4C6600 // DECLARE_IID(detect_ex_t, detect_t, PID_TRAFFICMONITOR, 102) ;
DEF_IDENTIFIER( process_t, 0x9C4C6700 ) // 0x9C4C6700 // DECLARE_IID(process_t, session_t, PID_TRAFFICMONITOR, 103) ;
DEF_IDENTIFIER( process_msg_t, 0x9C4C6800 ) // 0x9C4C6800 // DECLARE_IID(process_msg_t, session_t, PID_TRAFFICMONITOR, 104) ;
DEF_IDENTIFIER( pseudo_stop_t, 0x9C4C6900 ) // 0x9C4C6900 // DECLARE_IID(pseudo_stop_t, session_t, PID_TRAFFICMONITOR, 105) ;
DEF_IDENTIFIER( anybody_here_t, 0x9C4C6A00 ) // 0x9C4C6A00 // DECLARE_IID(anybody_here_t, session_t, PID_TRAFFICMONITOR, 106) ;

// FILE: O:\PPP\TestProjects\WinSockTracer2\TrafficProtocoller.h
DEF_IDENTIFIER( process_t, 0x9C4C6600 ) // 0x9C4C6600 // DECLARE_IID(process_t, session_t, PID_TRAFFICMONITOR, 102) ;
DEF_IDENTIFIER( process_msg_t, 0x9C4C6700 ) // 0x9C4C6700 // DECLARE_IID(process_msg_t, session_t, PID_TRAFFICMONITOR, 103) ;

// FILE: O:\PPP\tm\tm_task.h
DEF_IDENTIFIER( cTmTask, 0x00160400 ) // 0x00160400 // DECLARE_IID( cTmTask, cTmProfileBase, PID_TM, tmsi_cTmTask );

// FILE: O:\PPP\tm0\tm_profile.h
DEF_IDENTIFIER( cTmProfile, 0x00160000 ) // 0x00160000 // DECLARE_IID( cTmProfile, cTmProfileBase, PID_TM, tmsi_cTmProfile );

// FILE: O:\PPP\tm0\tm_service.h
DEF_IDENTIFIER( cTmTaskType, 0x00160100 ) // 0x00160100 // DECLARE_IID( cTmTaskType, cSerializable, PID_TM, tmsi_cTmTaskType );
DEF_IDENTIFIER( cTmTypeList, 0x00160200 ) // 0x00160200 // DECLARE_IID( cTmTypeList, cSerializable, PID_TM, tmsi_cTmTypeList );

// FILE: O:\PPP\AdminGUI\TaskProfileAdm.h
DEF_IDENTIFIER( CProfileAdm, 0x00132700 ) // 0x00132700 // DECLARE_IID( CProfileAdm, CProfile, PID_GUI, gst_CTaskProfileAdm );

// FILE: O:\PPP\gui60\avpgui.h
DEF_IDENTIFIER( cGuiRequest, 0x00136800 ) // 0x00136800 // DECLARE_IID( cGuiRequest, cSerializable, PID_GUI, 104 );

// FILE: O:\PPP\gui60\ListsView.h
DEF_IDENTIFIER( CDialogCtx, 0x00132F00 ) // 0x00132F00 // DECLARE_IID( CDialogCtx, cSerializable, PID_GUI, gst_CDialogCtx );
DEF_IDENTIFIER( CProfileCtx, 0x00133000 ) // 0x00133000 // DECLARE_IID( CProfileCtx, cSerializable, PID_GUI, gst_CProfileCtx );

// FILE: O:\PPP\gui60\TaskProfile.h
DEF_IDENTIFIER( CProfile, 0x00132500 ) // 0x00132500 // DECLARE_IID( CProfile, cProfileEx, PID_GUI, gst_CProfile );
DEF_IDENTIFIER( CTaskProfile, 0x00132600 ) // 0x00132600 // DECLARE_IID( CTaskProfile, CProfile, PID_GUI, gst_CTaskProfile );

// FILE: O:\PPP\gui60\TaskProfileProduct.h
DEF_IDENTIFIER( CProduct, 0x00132800 ) // 0x00132800 // DECLARE_IID( CProduct, CProfile, PID_GUI, gst_CProduct );
DEF_IDENTIFIER( CGuiSubProfile, 0x00132A00 ) // 0x00132A00 // DECLARE_IID( CGuiSubProfile, CProfile, PID_GUI, gst_CGuiSubProfile );
DEF_IDENTIFIER( CGuiProfile, 0x00132900 ) // 0x00132900 // DECLARE_IID( CGuiProfile, CProfile, PID_GUI, gst_CGuiProfile );
// ???: DEF_IDENTIFIER( cScanSettings, DECLARE_IID( cScanSettings, cTaskSettings, PID_AVS, SER_SETTINGS_ID+2 ); ) // ERR: Cannot resolve SER_SETTINGS_ID+2
// ???: DEF_IDENTIFIER( cProtectionSettings, DECLARE_IID( cProtectionSettings, cScanSettings, PID_AVS, SER_SETTINGS_ID+1 ); ) // ERR: Cannot resolve SER_SETTINGS_ID+1
// ???: DEF_IDENTIFIER( cPdmRegGroupApp_Item, DECLARE_IID(cPdmRegGroupApp_Item, cSerializable, PID_PDM, e_cPdmRegGroupApp_Item); ) // ERR: Cannot resolve e_cPdmRegGroupApp_Item
// ???: DEF_IDENTIFIER( cPdmRegGroupKey_Item, DECLARE_IID(cPdmRegGroupKey_Item, cSerializable, PID_PDM, e_cPdmRegGroupKey_Item); ) // ERR: Cannot resolve e_cPdmRegGroupKey_Item
// ???: DEF_IDENTIFIER( cPdmRegGroup_Item, DECLARE_IID(cPdmRegGroup_Item, cSerializable, PID_PDM, e_cPdmRegGroup_Item); ) // ERR: Cannot resolve e_cPdmRegGroup_Item

// FILE: O:\PPP\Include\structs\s_updater.h
// ???: DEF_IDENTIFIER( cUpdaterStatistics, DECLARE_IID_STATISTICS( cUpdaterStatistics, cProtectionStatistics, PID_UPDATER ); ) // ERR: Cannot resolve PID_UPDATER value
// ???: DEF_IDENTIFIER( cUpdaterStatisticsEx, DECLARE_IID_STATISTICS_EX( cUpdaterStatisticsEx, cUpdaterStatistics , PID_UPDATER, 1 ); ) // ERR: Cannot resolve PID_UPDATER value
// ???: DEF_IDENTIFIER( cUpdaterSettings, DECLARE_IID_SETTINGS( cUpdaterSettings, cCommonSettings, PID_UPDATER ); ) // ERR: Cannot resolve PID_UPDATER value
// ???: DEF_IDENTIFIER( cUpdaterReportData, DECLARE_IID_REPORT( cUpdaterReportData, cSerializable, PID_UPDATER ); ) // ERR: Cannot resolve PID_UPDATER value, Cannot resolve DECLARE_IID_REPORT( cUpdaterReportData, cSerializable, PID_UPDATER );
// ???: DEF_IDENTIFIER( cObjectInfo, DECLARE_IID( cObjectInfo, cSerializable, PID_UPDATERNEW, 1 ); ) // ERR: Cannot resolve PID_UPDATERNEW value
// ???: DEF_IDENTIFIER( cUpdaterNewSettings, DECLARE_IID_SETTINGS( cUpdaterNewSettings, cCommonSettings, PID_UPDATERNEW ); ) // ERR: Cannot resolve PID_UPDATERNEW value
// ???: DEF_IDENTIFIER( cUpdaterNewStatistics, DECLARE_IID_STATISTICS( cUpdaterNewStatistics, cProtectionStatistics , PID_UPDATERNEW ) ; ) // ERR: Cannot resolve PID_UPDATERNEW value
// ???: DEF_IDENTIFIER( cFileInfo, DECLARE_IID(cFileInfo, cSerializable, PID_UPDATERNEW, 4) ; ) // ERR: Cannot resolve PID_UPDATERNEW value
// ???: DEF_IDENTIFIER( cUpdaterNewStatisticsEx, DECLARE_IID_STATISTICS_EX(cUpdaterNewStatisticsEx, cUpdaterNewStatistics, PID_UPDATERNEW, 1) ; ) // ERR: Cannot resolve PID_UPDATERNEW value

// FILE: O:\PPP\tm\tm_profile.h
// ???: DEF_IDENTIFIER( cTmProfileBase, DECLARE_IID( cTmProfileBase, cProfileEx, PID_TM, tmsi_cTmProfileBase ); ) // ERR: Cannot resolve tmsi_cTmProfileBase

// FILE: O:\PPP\tm2\tm_profile.h
// ???: DEF_IDENTIFIER( cTmProfile, DECLARE_IID( cTmProfile, cProfileEx, PID_TM, tms_LAST+1 ); ) // ERR: Cannot resolve tms_LAST+1
// ???: DEF_IDENTIFIER( cTmTaskType, DECLARE_IID( cTmTaskType, cSerializable, PID_TM, tms_LAST+2 ); ) // ERR: Cannot resolve tms_LAST+2
// ???: DEF_IDENTIFIER( cScheduleParams, DECLARE_IID( cScheduleParams, cSerializable, PID_TM, tms_LAST+3 ); ) // ERR: Cannot resolve tms_LAST+3

