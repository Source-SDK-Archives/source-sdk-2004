# Microsoft Developer Studio Project File - Name="serverplugin_empty" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=serverplugin_empty - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "serverplugin_empty.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "serverplugin_empty.mak" CFG="serverplugin_empty - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "serverplugin_empty - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "serverplugin_empty - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/utils/serverplugin_empty", SCQCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "serverplugin_empty - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GR /GX /Zi /O2 /I "..\..\public" /I "..\..\public\tier1" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "serverplugin_emptyONLY" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /map /debug /machine:I386 /libpath:"..\lib\public" /libpath:"..\lib\common"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=.\Release\serverplugin_empty.dll
InputPath=.\Release\serverplugin_empty.dll
SOURCE="$(InputPath)"

"..\..\..\bin\serverplugin_empty.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	attrib -r ..\..\..\bin\serverplugin_empty.dll 
	copy "$(TargetPath)" ..\..\..\bin\serverplugin_empty.dll 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "serverplugin_empty - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "..\..\public" /I "..\..\public\tier1" /I "..\..\dlls" /I "..\..\game_shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "serverplugin_emptyONLY" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /map /debug /machine:I386 /nodefaultlib:"LIBC" /pdbtype:sept /libpath:"..\lib\public" /libpath:"..\lib\common"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=.\Debug\serverplugin_empty.dll
InputPath=.\Debug\serverplugin_empty.dll
SOURCE="$(InputPath)"

"..\..\..\bin\serverplugin_empty.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	attrib -r ..\..\..\bin\serverplugin_empty.dll 
	copy "$(TargetPath)" ..\..\..\bin\serverplugin_empty.dll 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "serverplugin_empty - Win32 Release"
# Name "serverplugin_empty - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\tier1\convar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\interface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\KeyValues.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memoverride.cpp
# End Source File
# Begin Source File

SOURCE=.\serverplugin_convar.cpp
# End Source File
# Begin Source File

SOURCE=.\serverplugin_empty.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlbuffer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Public\basetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\public\Color.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\dbg.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\eiface.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\FileSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\ICommandLine.h
# End Source File
# Begin Source File

SOURCE=..\..\public\igameevents.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\interface.h
# End Source File
# Begin Source File

SOURCE=..\..\public\dlls\iplayerinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\public\engine\iserverplugin.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\KeyValues.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\mem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memalloc.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memdbgon.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\strtools.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlvector.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\vstdlib.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\lib\public\vstdlib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# End Target
# End Project
