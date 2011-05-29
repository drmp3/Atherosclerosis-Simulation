# Microsoft Developer Studio Project File - Name="atherosclerosis simulation" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=atherosclerosis simulation - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "atherosclerosis simulation_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "atherosclerosis simulation_vc6.mak" CFG="atherosclerosis simulation - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "atherosclerosis simulation - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "atherosclerosis simulation - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "atherosclerosis simulation - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W2 /Gi /GX /O2 /Ob2 /I "D:\devstudio6\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"athero.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib d3dx.lib /nologo /subsystem:windows /profile /map /machine:I386 /out:"Release/atherosclerosis game.exe" /libpath:"D:\devstudio6\Lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "atherosclerosis simulation - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gr /MDd /W3 /Gi /GX /ZI /Od /I "D:\devstudio6\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"athero.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib d3dx.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/atherosclerosis game.exe" /pdbtype:sept /libpath:"D:\devstudio6\Lib"
# SUBTRACT LINK32 /profile /nodefaultlib

!ENDIF 

# Begin Target

# Name "atherosclerosis simulation - Win32 Release"
# Name "atherosclerosis simulation - Win32 Debug"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\Athero.h
# End Source File
# Begin Source File

SOURCE=.\Athero.rc
# End Source File
# Begin Source File

SOURCE=.\AtheroApp.cpp
# End Source File
# Begin Source File

SOURCE=.\AtheroApp.h
# End Source File
# Begin Source File

SOURCE=.\buildpch.cpp
# ADD CPP /Yc"athero.h"
# End Source File
# Begin Source File

SOURCE=.\Ddrawx.cpp
# End Source File
# Begin Source File

SOURCE=.\Ddrawx.h
# End Source File
# Begin Source File

SOURCE=.\EnterValueForx0Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EnterValueForx0Dlg.h
# End Source File
# Begin Source File

SOURCE=.\Funcadj.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionAdjusterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionAdjusterDlg.h
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\MacroGame.cpp
# End Source File
# Begin Source File

SOURCE=.\MacroGame.h
# End Source File
# Begin Source File

SOURCE=.\Macrophg.cpp

!IF  "$(CFG)" == "atherosclerosis simulation - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "atherosclerosis simulation - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Macrophg.h
# End Source File
# Begin Source File

SOURCE=.\Macrophg3D.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.h
# End Source File
# Begin Source File

SOURCE=.\olourDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\olourDialog.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SimulationInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\SimulationInfo.h
# End Source File
# Begin Source File

SOURCE=.\SimulationSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\SimulationSettings.h
# End Source File
# Begin Source File

SOURCE=.\SplashScreenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashScreenDlg.h
# End Source File
# Begin Source File

SOURCE=.\Sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Sprite.h
# End Source File
# Begin Source File

SOURCE=.\startup.bmp
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\XSectionDisplayDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XSectionDisplayDlg.h
# End Source File
# End Target
# End Project
