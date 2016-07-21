@echo off

echo =====[ begin of localization %1 ]=================================================
echo

call VCVARS32.BAT
rc /dEXT_COMPILING /dAVPM_START_RES /dAVPM_UIPART_RES /dAVPM_UIOBJ_RES TrainingWizard_%1.rc
if errorlevel 1 goto error
link /out:"TrainingWizard.loc" /DLL /NOENTRY /MACHINE:IX86 /ALIGN:4096 /IGNORE:4108 TrainingWizard_%1.RES
del TrainingWizard_%1.res
if errorlevel 1 goto error
goto done

:error
echo Error !!!

:done
tsigner TrainingWizard.loc
copy TrainingWizard.loc ..\..\..\..\GUI60\skin\%1\TrainingWizard.loc
del TrainingWizard.loc
echo Success !!!

echo =====[ end of localization %1 ]=================================================

