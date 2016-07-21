'<
'**********************************************************
' SCRIPT:			TD_Import.vbs
' AUTHOR:           V. Samarin
' VERSION:			1.1
'
' LAST MODIFIED:	17/05/2006
' MODIFIED BY:      V. Samarin
'
' Descripion:       
'					Imports test results from specified test log to 
'					Mercury TestDirector (TD), updates statuses of:
'					- Test Run
'					- Test in TestSet
'					- Test
'					- Requirement
'
' Requirements:
'			1. OTAClient80.dll library registered (classes to iteract with TD)
'			2. Proper TD connection settings: 
'				- see constants at the beginning of the script
'			3. Test log specification:
'				TEST	TD_user	OS	AUT_Build	Host	Parent_folder	Test_name	Test_set	Test_date	Test_time
'				STEP	Test_step	Step_date	Step_time	Step_status	Step_actual_result
'				END	Test_status	Test_duration
'
'			Description:
'				TEST				- test start keyword
'				TD_user				- displayed tester name
'				OS					- (custom field) test host OS
'				AUT_Build			- (custom field) build of application-under-test
'				Host				- (custom field) host name
'				Parent_folder		- parent folder for the test
'				Test_name			- test name
'				Test_set			- test set name
'				Test_date			- test start date
'				Test_time			- test start time
'
'				STEP				- verification (test step) keyword	
'				Test_step			- test step name
'				Step_date			- test step start date
'				Step_time			- test step start time
'				Step_status			- test step status
'				Step_actual_result	- test step actual result
'			
'				END					- test finish keyword
'				Test_status			- test status
'				Test_duration		- test duration in seconds
'
'			4. All the items of test log specification should be provided 
'				or script changes required
'			5. "custom fields" implemented in FS family projects only. If you need them, 
'				please, customise your project
'			6. Usage:
'				cscript td_import.vbs <project> <user> <password> <log_file>
'
' Notes:
'			
'	
' Procedures:
'           - Msg (strProcedure, strMessage)					- logs message
'			- Set_Connection(strProject, strUser, strPassword)	- sets connection to TD
'			- Release_Connection()								- closes connection to TD
'			- Get_Last_RunID ()									- reads ID of last Run from SEQUENCES table
'			- Increment_RunID ()								- increments Run ID counter
'			- Calculate_Req_Status(lngReqID)					- calculates aggregated exec status of the requirement
'			- Update_Req_Status(lngTestID)						- updates status of requirements covered by the test
'			- Add_Run (colTest)									- imports test Run to Mercury Quality Center
'			- Import_Runs (strFile)								- imports test Runs from specified file to Mercury Quality Center
'
'           - Sub Main()										- Shell for functions and subs
'
'**********************************************************

Option Explicit


' Access rights constants
Const ForReading = 1, ForWriting = 2, ForAppending = 8

' Return codes
Const RES_ERROR		= 1
Const RES_FAILURE	= 2
Const RES_WARNING	= 3
Const RES_OK		= 8

' Last procedure name (for logging)
Dim sstrProcedure

' Mercury TestDirector's connection object
Dim gobjTDConnect


'**********************************************************

' Description:	logs message
' Parameter:	strMessage - text to Log
Private Sub Msg(strProcedure, strMessage)
	
	' Error logging
	If strProcedure = RES_FAILURE Then
		WScript.echo ("ERROR: " & strMessage)	
		Exit Sub
	End If
	
	' Insert empty line if procedure changed
	If (strProcedure <> sstrProcedure) Then 
		WScript.echo ""
	End If
	sstrProcedure = strProcedure
	
	WScript.echo ("(" & strProcedure & ") " & strMessage)	
End Sub


' Description:	sets connection to TD
' Parameter:	(out) gobjTDConnect - TD connection object
'				strServer - TD host in format
'				strUser/strPassword - user credentials
' Result:		RES_OK - TD connection is set
'				RES_FAILURE - error opening TD connection
Function Set_Connection(strProject, strUser, strPassword)
    Const Proc = "Set_Connection"

	' Define default connection settings
	Const TD_SERVER = "http://tlserver:8081/qcbin"
	Const TD_DOMAIN = "DEFAULT"

	' Set default return code
	Dim intResult
		intResult = RES_FAILURE

	' Release any existing project connections. 
	Call Release_Connection()

	'Create the connection 
	Set gobjTDConnect = WScript.CreateObject ("TDApiOle80.TDConnection")
		gobjTDConnect.InitConnectionEx TD_SERVER 

	' Connect to the project and user 
    gobjTDConnect.ConnectProjectEx TD_DOMAIN, strProject, strUser, strPassword 


	' Connection successfully set
    Set_Connection = RES_OK 

End Function 


' Description:	closes connection to TD
' Result:		RES_OK - TD connection is closed
Function Release_Connection()
	
	' Release any existing project connections. 
    If Not IsEmpty(gobjTDConnect) Then 
		If Not (gobjTDConnect Is Nothing) Then 
			If gobjTDConnect.Connected Then 
				If gobjTDConnect.ProjectConnected Then 
	                gobjTDConnect.DisconnectProject 
                End If 
            End If 
            gobjTDConnect.ReleaseConnection 
        End If 
	End If 
    
    ' Connection successfully closed 
    Release_Connection = RES_OK
    
End Function


' Description:	reads ID of last Run from SEQUENCES table
' Result:		last Run ID 
Function Get_Last_RunID () 
	Const Proc = "Get_Last_RunID"
	
	Dim objCommand, objRecordSet

	' Setting and executing the SQL
    Set objCommand = gobjTDConnect.Command
		objCommand.CommandText = _
			"SELECT SQ_SEQ_VALUE FROM td.SEQUENCES WHERE (SQ_SEQ_NAME = 'Run')"			
    Set objRecordSet = objCommand.Execute

	' Return last Run ID
    If objRecordSet.RecordCount > 0 Then
		Get_Last_RunID = objRecordSet.FieldValue("SQ_SEQ_VALUE") 
	
	' Data not found
	Else
		Call Msg(Proc, "No data returned from field SEQUENCES.SQ_SEQ_VALUE")
		Get_Last_RunID = 0
    End If
    
End Function


' Description:	increments Run ID counter
' Result:		new Run ID 
Function Increment_RunID () 
	Const Proc = "Increment_RunID"
	
	Dim objCommand, objRecordSet, lngLastID, lngNewID
	
	' Get last Run ID
	lngLastID = Get_Last_RunID ()
	
	' Increment Run ID counter
	lngNewID = lngLastID + 1
    Set objCommand = gobjTDConnect.Command
		objCommand.CommandText = _
			"UPDATE td.SEQUENCES SET SQ_SEQ_VALUE = " & lngNewID & _
            "WHERE (SQ_SEQ_NAME = 'Run')"			
    Set objRecordSet = objCommand.Execute

	' Chech that no other Runs were added
	lngNewID = Get_Last_RunID ()

	' Run ID successfully increased
	If CLng(lngNewID) = CLng(lngLastID) + 1 Then
		Increment_RunID = lngNewID

	' Unknown Run(s) added
	ElseIf lngNewID > lngLastID + 1 Then
		Call Msg(Proc, "Unknown Run(s) added. Can't get proper Run ID")
		Increment_RunID = 0
	
	' Run ID not incremented
	Else
		Call Msg(Proc, "Run ID not incremented")
		Increment_RunID = 0
	End If
	
End Function


' Description:	calculates aggregated exec status of the requirement
' Params:		lngReqID - ID of the requirement to proceed
' Results:		"Passed | Failed | Not Completed" 
Function Calculate_Req_Status(lngReqID)

	Dim objCommand, objRecordSet
	Dim	strTestStatus, strReqStatus, bCompleted, i
	
	' Select tests' statuses for the requirement
	Set objCommand = gobjTDConnect.Command
	objCommand.CommandText = _			
		"SELECT td.TEST.TS_EXEC_STATUS " & _
		"FROM td.TEST INNER JOIN td.REQ_COVER " & _
		"ON td.TEST.TS_TEST_ID = td.REQ_COVER.RC_TEST_ID " & _
		"WHERE td.REQ_COVER.RC_REQ_ID = " & lngReqID & " " & _
		"GROUP BY td.TEST.TS_EXEC_STATUS " & _
		"ORDER BY td.TEST.TS_EXEC_STATUS"
	Set objRecordSet = objCommand.Execute

	' Consider (by default) all the tests as Passed 
	bCompleted = True
	strReqStatus = "Passed"

	' Analyse available tests' statuses
	With objRecordSet
	For i = 1 To .RecordCount

		' Read status of group of tests
		strTestStatus = UCase(.FieldValue("TS_EXEC_STATUS"))
	
		' Define Req status for the test status
		Select Case UCase(strTestStatus)
			
			' Failed tests found
			Case "FAILED"
				strReqStatus = "Failed"

			' Default status 'Passed' already set
			Case "PASSED" 
			
			' There exist tests that wern't executed or finished
			Case "NOT COMPLETED", "NO RUN", "N/A"
				bCompleted = False

			' Consider custom statuses as 'Failed'
			Case Else
				strReqStatus = "Failed"
				
		End Select			
	
		' Add next run Step
		.Next 
	Next		
	End With
	
	' No failed tests found
	If UCase(strReqStatus) <> "FAILED" Then
	
		' Check if there are any unfinished (not executed) tests
		If Not bCompleted Then
			strReqStatus = "Not Completed"
		End If
	End If
	
	' Return caclulated status 
	Calculate_Req_Status = strReqStatus
	
End Function


' Description:	updates status of requirements covered by the test
' Params:		lngTestID - ID of the test
Function Update_Req_Status(lngTestID)
On Error Resume Next
	Const Proc = "Update_Req_Status"

	Dim objCommand, objRecordSet, objUpdate
	Dim	i, strReqStatus

	
	' Select requirements covered by the test
	Set objCommand = gobjTDConnect.Command
	objCommand.CommandText = _			
		"SELECT td.REQ.RQ_REQ_ID " & _
		"FROM td.REQ INNER JOIN td.REQ_COVER " & _
		"ON td.REQ.RQ_REQ_ID = td.REQ_COVER.RC_REQ_ID " & _
		"WHERE td.REQ_COVER.RC_TEST_ID = " & lngTestID
	Set objRecordSet = objCommand.Execute
	
	' Check if any requirements found
	With objRecordSet
	If .RecordCount = 0 Then
		Call Msg(Proc, "No requirements found covered by test with ID " & lngTestID)

	' Update exec statuses of found requirements
	Else
		For i = 1 To .RecordCount

			' Calculate aggregated status of the requirement
			strReqStatus = Calculate_Req_Status(.FieldValue("RQ_REQ_ID"))

			' Update status of the requirement to aggregated one
			objCommand.CommandText = _			
				"UPDATE td.REQ SET RQ_REQ_STATUS = '" & strReqStatus & "' " & _
				"WHERE RQ_REQ_ID = " & .FieldValue("RQ_REQ_ID")
			Set objUpdate = objCommand.Execute
	
				' Add next run Step
			.Next 
		Next
				
	End If ' Update exec statuses of found requirements 
	End With
	
	' Handle unexpected errors
	If Err.Number <> 0 Then
		Call Msg(Proc, "Unexpected error #" & Err.Number & ": " & Err.Description)
		Exit Function			
	End If
	
End Function


' Description:	imports test Run to Mercury Quality Center
' Params:		colTest - test records to be imported
' Result:		RES_OK - a new Run object successfully impoted
'				RES_FAILURE - unexpected Error
Function Add_Run (colTest) 
On Error Resume Next
	Const Proc = "Add_Run"

	Const INSTANCE = 1

	' TEST Tester strOS strBuild strHost Subject Name TestSet ExecDate ExecTime
	Const IDX_TESTER = 1
	Const IDX_OS = 2
	Const IDX_BUILD = 3
	Const IDX_HOST = 4
	Const IDX_SUBJECT = 5
	Const IDX_TEST = 6
	Const IDX_TESTSET = 7
	Const IDX_DATE = 8
	Const IDX_TIME = 9

	' END Status, Duration
	Const IDX_STATUS = 1
	Const IDX_DURATION = 2

	' STEP Name ExecDate ExecTime Status Actual				
	Const IDX_STEP_NAME = 1
	Const IDX_STEP_DATE = 2
	Const IDX_STEP_TIME = 3
	Const IDX_STEP_STATUS = 4
	Const IDX_STEP_ACTUAL = 5


	Dim objCommand, objRecordSet, objSteps, objWshNetwork
	Dim lngID, lngTestID, lngSetID, i, j
	Dim strLine, arrTest, arr
	
	' Set default return value
	Add_Run = RES_FAILURE

	' Copy test records to array
	arrTest = colTest.Items


	' VALIDATE TEST STATUS

	' Read last test record
	strLine = arrTest(UBound(arrTest))

	' Split line in tokens
	arr = Split(strLine, vbTab)

	' Skip test Run import if run status is 'No Run'
	If UCase(arr(0)) = "END" And UCase(arr(IDX_STATUS)) = "NO RUN" Then
		
		arr = Split(arrTest(LBound(arrTest)), vbTab)
		Call Msg(Proc, "Import test '" & arr(IDX_SUBJECT) & "\" & arr(IDX_TEST) & _
						"': skipped by status 'No Run'")
		
		Add_Run = RES_OK
		Exit Function
	End If

	' IMPORT TEST RUN

	' Get common objects
	Set objWshNetwork = WScript.CreateObject("WScript.Network")

	' Loop by test records
	For j = LBound(arrTest) To UBound(arrTest)
				
		' Split next test record
		arr = Split(arrTest(j), vbTab)
		' Call Msg(Proc, arrTest(j))
			
		' Define instruction type
		Select Case UCase(Trim(arr(0)))
	 
			' Insert a new Run:
			' TEST Tester strOS strBuild strHost Subject Name TestSet ExecDate ExecTime
			Case "TEST"

				' Log test import
				Call Msg(Proc, "Import test '" & arr(IDX_SUBJECT) & "\" & arr(IDX_TEST) & "'")				

				' Get ID for the new Run
				lngID = CLng(Increment_RunID ())
				If lngID = 0 Then
					Call Msg(Proc, "Can't get ID for a new Run object")
					Exit Function
				End If

				' Get Test's & TestSet's IDs	
				Set objCommand = gobjTDConnect.Command
					objCommand.CommandText = _			
						"SELECT td.TESTCYCL.TC_TEST_ID, td.TESTCYCL.TC_CYCLE_ID " & _
						"FROM td.TESTCYCL " & _
						"INNER JOIN td.TEST ON td.TESTCYCL.TC_TEST_ID = td.TEST.TS_TEST_ID " & _
						"INNER JOIN td.ALL_LISTS ON td.TEST.TS_SUBJECT = td.ALL_LISTS.AL_ITEM_ID " & _
						"INNER JOIN td.CYCLE ON td.TESTCYCL.TC_CYCLE_ID = td.CYCLE.CY_CYCLE_ID " & _
						"WHERE (td.TEST.TS_NAME = '" & arr(IDX_TEST) & "') " & _
						"AND (td.ALL_LISTS.AL_DESCRIPTION = '" & arr(IDX_SUBJECT) & "') " & _
						"AND (td.CYCLE.CY_CYCLE = '" & arr(IDX_TESTSET) & "') " & _
						"AND (td.TESTCYCL.TC_TEST_INSTANCE = " & INSTANCE & ")"			
				Set objRecordSet = objCommand.Execute
	
				' Read IDs from the Recordset
				If objRecordSet.RecordCount > 0 Then
					lngTestID = objRecordSet.FieldValue("TC_TEST_ID") 
					lngSetID = objRecordSet.FieldValue("TC_CYCLE_ID") 
				' Data not found
				Else
					Call Msg(Proc, "No test '" & arr(IDX_SUBJECT) & "\" & arr(IDX_TEST) & _
									"' found in TestSet '" & arr(IDX_TESTSET) & "'")
					Exit Function		
				End If
	
				' Add a new Run
				objCommand.CommandText = _			
					"INSERT INTO td.RUN " & _
					"(RN_CYCLE_ID, RN_TEST_ID, RN_RUN_ID, RN_TEST_INSTANCE, " & _
					"RN_RUN_NAME, RN_EXECUTION_DATE, RN_EXECUTION_TIME, " & _
					"RN_HOST, RN_TESTER_NAME, " & _
					"RN_USER_01, RN_USER_04, RN_USER_03, RN_RUN_VER_STAMP) " & _
					"VALUES " & _
					"(" & lngSetID & ", " & lngTestID & ", " & lngID & ", " & INSTANCE & _
					", 'Run-" & lngID & "', '" & arr(IDX_DATE) & "', '" & arr(IDX_TIME) & "'" & _
					", '" & objWshNetwork.ComputerName & "', '" & arr(IDX_TESTER) & "'" & _
					", '" & arr(IDX_OS) & "', '" & arr(IDX_BUILD) & "', '" & arr(IDX_HOST) & "', 1)"
				Set objRecordSet = objCommand.Execute

				' Update Run info in TestCycle table 
				objCommand.CommandText = _			
					"UPDATE td.TESTCYCL " & _
					"SET TC_EXEC_DATE = '" & arr(IDX_DATE) & "', " & _
					"TC_EXEC_TIME = '" & arr(IDX_TIME) & "', " & _
					"TC_ACTUAL_TESTER = '" & arr(IDX_TESTER) & "' " & _
					"WHERE (TC_TEST_INSTANCE = " & INSTANCE & ") " & _
					"AND (TC_TEST_ID = " & lngTestID & ") AND (TC_CYCLE_ID = " & lngSetID & ")"
				Set objRecordSet = objCommand.Execute


			' Insert steps to the Run:				
			' STEP Name ExecDate ExecTime Status Actual				
			Case "STEP"

				' Read test steps
				objCommand.CommandText = _			
					"SELECT DS_STEP_ID, DS_STEP_ORDER, DS_STEP_NAME, DS_DESCRIPTION, DS_EXPECTED " & _
					"FROM td.DESSTEPS WHERE DS_TEST_ID = " & lngTestID & _
					" AND DS_STEP_NAME = '" & arr(IDX_STEP_NAME) & "'"		
				Set objRecordSet = objCommand.Execute
	
				' Check if specified Step found
				With objRecordSet
				If .RecordCount = 0 Then
					Call Msg(Proc, "Step '" & arr(IDX_STEP_NAME) & "' not found in TD")

				' Insert tests steps to Run
				Else
					For i = 1 To .RecordCount
					objCommand.CommandText = _			
						"INSERT INTO td.STEP " & _
						"(ST_RUN_ID, ST_STEP_ID, ST_DESIGN_ID, ST_STEP_NAME, ST_STATUS, ST_TEST_ID, " & _
						"ST_EXECUTION_DATE, ST_EXECUTION_TIME, ST_DESCRIPTION, ST_EXPECTED, " & _
						"ST_ACTUAL, ST_STEP_ORDER) " & _
						"VALUES (" & lngID & ", " & .FieldValue("DS_STEP_ID") & ", " & _
						.FieldValue("DS_STEP_ID") & ", '" & .FieldValue("DS_STEP_NAME") & "', '" & _
						arr(IDX_STEP_STATUS) & "', " & lngTestID & ", '" & arr(IDX_STEP_DATE) & "', '" & _
						arr(IDX_STEP_TIME) & "', '" & .FieldValue("DS_DESCRIPTION") & "', '" & _
						.FieldValue("DS_EXPECTED") & "', '" & arr(IDX_STEP_ACTUAL) & "', " & _
						.FieldValue("DS_STEP_ORDER") & ")"
					Set objSteps = objCommand.Execute
	    
					' Add next run Step
					.Next 
					Next  
				
				End If
				End With


			' Set run status:
			' END Status, Duration
			Case "END"

				' Update record of the Run
				objCommand.CommandText = _			
					"UPDATE td.RUN " & _
					"SET RN_STATUS = '" & arr(IDX_STATUS) & "', " & _
					"RN_DURATION = " & arr(IDX_DURATION) & ", " & "RN_RUN_VER_STAMP = 2 " & _
					"WHERE (RN_CYCLE_ID = " & lngSetID & ") AND (RN_TEST_ID  = " & lngTestID & ") " & _
					"AND (RN_RUN_ID = " & lngID & ") AND (RN_TEST_INSTANCE = 1)" 
				Set objRecordSet = objCommand.Execute

				' Update Run info in TestCycle table 
				objCommand.CommandText = _			
					"UPDATE td.TESTCYCL " & _
					"SET TC_STATUS = '" & arr(IDX_STATUS) & "' " & _
					"WHERE (TC_TEST_INSTANCE = " & INSTANCE & ") " & _
					"AND (TC_TEST_ID = " & lngTestID & ") AND (TC_CYCLE_ID = " & lngSetID & ")"
				Set objRecordSet = objCommand.Execute

				' Update Test info in Test table
				If UCase(arr(IDX_STATUS)) <> "NO RUN" And UCase(arr(IDX_STATUS)) <> "N/A" Then
					objCommand.CommandText = _			
						"UPDATE td.TEST " & _
						"SET TS_EXEC_STATUS = '" & arr(IDX_STATUS) & "' " & _
						"WHERE (TS_TEST_ID = " & lngTestID & ")"
					Set objRecordSet = objCommand.Execute
					
					' Update statuses of associated requirements
					Call Update_Req_Status(lngTestID)
				End If
	
			Case Else
				Call Msg(Proc, "Unknown key '" & arr(0) & "' specified")
				objFile.Close				
				
				Exit Function
			
		End Select	
		
		' Handle unexpected errors
		If Err.Number <> 0 Then
			Call Msg(Proc, arrTest(j))
			Call Msg(RES_FAILURE, "Unexpected error #" & Err.Number & ": " & Err.Description)
			Exit Function			
		End If
				
	
	Next ' Loop by test records
	

	Add_Run = RES_OK
	
End Function


' Description:	imports test Runs from specified file to Mercury Quality Center
' Params:		strFile - log file to be imported
' Result:		RES_OK - a new Run object successfully added
'				RES_FAILURE - unexpected Error
Function Import_Runs (strFile) 
On Error Resume Next
	Const Proc = "Import_Runs"

	Dim objFSO, objFile, colTest
	Dim strLine, k
	
	' Set default return value
	Import_Runs = RES_OK

	' Get common objects
	Set objFSO = WScript.CreateObject("Scripting.FileSystemObject")
	Set colTest = WScript.CreateObject("Scripting.Dictionary")

	' Check if specified log file exists
	If Not objFSO.FileExists(strFile) Then
		Call Msg(Proc, "Log file '" & strFile & "' not found")
		Import_Runs = RES_FAILURE

		Set objFSO = Nothing
		Set colTest = Nothing
		Exit Function
	End If

	' Open the log file for reading
	Set objFile = objFSO.OpenTextFile(strFile, ForReading, False)

	' Loop by tests		
	Do While Not objFile.AtEndofStream
	
		' Note: we can't determine status of the test before the END statement.
		'	However, tests with status No Run should be skipped. To be sure of
		'	the test status before updating the test, we'll read all the test
		'	records, analize the test status and make a decision, if the test
		'	should be skipped

		' Reset counters
		k = 0
		colTest.RemoveAll
		
		' Load test records to collection
		Do While Not objFile.AtEndofStream
				
			' Read line from the file
			strLine = objFile.ReadLine()
			k = k + 1
			
			' Add a new test record to collection
			If strLine <> "" Then
				colTest.Add "Key" & k, strLine
			End If 
		
			' All test records loaded
			strLine = Left(strLine, 3)
			If UCase(strLine) = "END" Then
				Exit Do
			End If
				
		Loop ' Load test records to collection

		' Import test Run 
		If Add_Run(colTest) <> RES_OK Then
			Import_Runs = RES_FAILURE
		End If
	
	Loop ' Loop by file records
	

	' Close open file
	objFile.Close

	Set objFSO = Nothing
	Set colTest = Nothing

End Function


Sub Main()
    Const Proc = "Main"
    
    Dim intResult, objRunFactory, i, objTest
    Dim strTD_PROJECT, strTD_USER, strTD_PASSWORD, strTD_TEST_LOG
    
    ' Read script parameters
    If WScript.Arguments.Count <> 4 Then
    	Call Msg(Proc, "Usage: cscript TD_Import.vbs <project> <user> <password> <log_file>")
    	WScript.Quit RES_FAILURE
    End If
    
	' TÂ connection settings
	strTD_PROJECT = WScript.Arguments(0)
	strTD_USER = WScript.Arguments(1)
	strTD_PASSWORD = WScript.Arguments(2)
	strTD_TEST_LOG = WScript.Arguments(3)
	    
    ' Open TD session
    intResult = Set_Connection(strTD_PROJECT, strTD_USER, strTD_PASSWORD)
	
	' Import test log to TD
	WScript.Echo "Res = " & Import_Runs(strTD_TEST_LOG)
	
	' Finish TD session
	Call Release_Connection() 
	

	' Return value
	WScript.Quit intResult

End Sub


Call Main()