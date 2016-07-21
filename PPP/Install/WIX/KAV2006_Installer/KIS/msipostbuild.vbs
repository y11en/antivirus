Option Explicit
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Const msiOpenDatabaseModeTransact = 1
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Dim MSIFile, sQuery
Dim oMSI
Dim oDB, oView, oView1
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

If Wscript.Arguments.Count < 1 Then
    Wscript.Echo "ISWI Modification Utility" & _
        vbNewline & "1st argument is the full path to the .msi file" 
    Wscript.Quit 1
End If

'MSIFile = "C:\WORK\My Setups\Kaspersky Anti-Virus For NTFServer 5_0\Product Configuration Release Free\Network Image English Uninstall\DiskImages\DISK1\Kaspersky Anti-Virus For Windows File Servers.msi"'
Set oMSI = CreateObject("WindowsInstaller.Installer")
'WScript.Echo "MSI database localtion: " & db_name
Set oDB = oMSI.OpenDatabase(Wscript.Arguments(0), msiOpenDatabaseModeTransact)
'Set oView = oDB.OpenView("INSERT INTO MsiDriverPackages " & _
'				"(MsiDriverPackages.Component,MsiDriverPackages.Flags,MsiDriverPackages.Sequence) " & _
'				"VALUES ('klif.inf', 24, 0)" )							
'oView.Execute : oView.Close : oDB.Commit

'Set oView = oDB.OpenView("UPDATE InstallExecuteSequence " & _
'				"SET Condition = 'Msix64' " & _
'				"WHERE InstallExecuteSequence.Action = 'MsiProcessDrivers' OR InstallExecuteSequence.Action = 'MsiCleanupOnSuccess'" )
'oView.Execute : oView.Close : oDB.Commit

'Set oView = oDB.OpenView("INSERT INTO ActionText " & _
'				"(ActionText.Action,ActionText.Description,ActionText.Template) " & _
'				"VALUES ('MsiUninstallDrivers', 'Removing driver package...', '')" )
'oView.Execute : oView.Close : oDB.Commit
sQuery = "UPDATE Property SET Value = '" & Wscript.Arguments(1) & "' WHERE Property.Property = 'ProductVersion'"
'MsgBox sQuery
Set oView = oDB.OpenView(sQuery)
				
oView.Execute : oView.Close : oDB.Commit

'Cleanup
Set oMSI = Nothing
