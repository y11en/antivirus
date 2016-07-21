DoCommand "banner_deny:" + WScript.Arguments(0)

Sub DoCommand(ByVal params)
	On Error Resume Next
	set AVPGui=CreateObject("AVPGUI.Controller")
	if err<>0 then
		MsgBox("AVPGUI.Controller is not enabled!"+ vbCrLf+"Please enable ActiveX before")
	else
		call AVPGui.DoCommand(params)
	end if
end sub 

