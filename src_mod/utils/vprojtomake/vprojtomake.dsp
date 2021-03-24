# Microsoft Developer Studio Project File - Name="vprojtomake" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vprojtomake - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vprojtomake.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vprojtomake.mak" CFG="vprojtomake - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vprojtomake - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vprojtomake - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/utils/vprojtomake", XPQDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\common" /I "..\..\public" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comsupp.lib msxml2.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\lib\public"
# Begin Custom Build
TargetPath=.\Release\vprojtomake.exe
InputPath=.\Release\vprojtomake.exe
SOURCE="$(InputPath)"

"..\..\..\bin\vprojtomake.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist  ..\..\..\bin\vprojtomake.exe attrib -r ..\..\..\bin\vprojtomake.exe 
	if exist $(TargetPath) copy $(TargetPath) ..\..\..\bin\vprojtomake.exe 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /Zi /Od /I "..\..\public" /I "..\common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEBUG" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comsupp.lib msxml2.lib /nologo /subsystem:console /profile /map /debug /machine:I386 /libpath:"..\..\lib\public" /fixed:no
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build
TargetPath=.\Debug\vprojtomake.exe
InputPath=.\Debug\vprojtomake.exe
SOURCE="$(InputPath)"

"..\..\..\bin\vprojtomake.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist  ..\..\..\bin\vprojtomake.exe attrib -r ..\..\..\bin\vprojtomake.exe 
	if exist $(TargetPath) copy $(TargetPath) ..\..\..\bin\vprojtomake.exe 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "vprojtomake - Win32 Release"
# Name "vprojtomake - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\public\characterset.cpp

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.cpp

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.cpp

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\filesystem_tools.cpp

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tier1\interface.cpp

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tier1\KeyValues.cpp

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\makefilecreator.cpp

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlbuffer.cpp

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlsymbol.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\vcprojconvert.cpp

!IF  "$(CFG)" == "vprojtomake - Win32 Release"

!ELSEIF  "$(CFG)" == "vprojtomake - Win32 Debug"

# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vprojtomake.cpp
# ADD CPP /YX
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Public\basetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\characterset.h
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\FileSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.h
# End Source File
# Begin Source File

SOURCE=..\common\filesystem_tools.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\KeyValues.h
# End Source File
# Begin Source File

SOURCE=.\makefilecreator.h
# End Source File
# Begin Source File

SOURCE=.\MsXml2.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utldict.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\tier1\UtlMemory.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\tier1\utlsymbol.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlvector.h
# End Source File
# Begin Source File

SOURCE=.\vcprojconvert.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\vstdlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\public\appframework\IAppSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\vstdlib.lib
# End Source File
# End Target
# End Project
