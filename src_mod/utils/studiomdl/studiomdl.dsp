# Microsoft Developer Studio Project File - Name="studiomdl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=studiomdl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "studiomdl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "studiomdl.mak" CFG="studiomdl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "studiomdl - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "studiomdl - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/utils/studiomdl", ZKGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "studiomdl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Zi /O2 /I "..\common" /I "..\..\Public" /I "..\..\Public\tier1" /I "..\nvtristriplib" /I "..\..\Game_Shared" /D "_CONSOLE" /D "PROTECTED_THINGS_DISABLE" /D "NDEBUG" /D "_WIN32" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:console /map /debug /machine:I386 /nodefaultlib:"libcd.lib" /libpath:"..\..\lib\common" /libpath:"..\..\lib\public"
# Begin Custom Build - copy to ..\..\..\bin\studiomdl.exe
TargetDir=.\Release
TargetPath=.\Release\studiomdl.exe
TargetName=studiomdl
InputPath=.\Release\studiomdl.exe
SOURCE="$(InputPath)"

"..\..\..\bin\studiomdl.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\bin\studiomdl.exe attrib -r ..\..\..\bin\studiomdl.exe 
	copy $(TargetPath) ..\..\..\bin\studiomdl.exe 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\bin\$(TargetName).map 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "studiomdl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /ZI /Od /I "..\common" /I "..\..\Public" /I "..\..\Public\tier1" /I "..\nvtristriplib" /I "..\..\Game_Shared" /D "_CONSOLE" /D "PROTECTED_THINGS_DISABLE" /D "_DEBUG" /D "_WIN32" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libc.lib" /libpath:"..\..\lib\common" /libpath:"..\..\lib\public"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - copy to ..\..\..\bin\studiomdl.exe
TargetDir=.\Debug
TargetPath=.\Debug\studiomdl.exe
TargetName=studiomdl
InputPath=.\Debug\studiomdl.exe
SOURCE="$(InputPath)"

"..\..\..\bin\studiomdl.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\bin\studiomdl.exe attrib -r ..\..\..\bin\studiomdl.exe 
	copy $(TargetPath) ..\..\..\bin\studiomdl.exe 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\bin\$(TargetName).map 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "studiomdl - Win32 Release"
# Name "studiomdl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\public\bone_setup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\characterset.cpp
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.cpp
# End Source File
# Begin Source File

SOURCE=.\collisionmodel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\CollisionUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_init.cpp
# End Source File
# Begin Source File

SOURCE=..\common\filesystem_tools.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwareMatrixState.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwareVertexCache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\interface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\KeyValues.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\Mathlib.cpp
# End Source File
# Begin Source File

SOURCE=.\matsys.cpp
# End Source File
# Begin Source File

SOURCE=.\mrmsupport.cpp
# End Source File
# Begin Source File

SOURCE=..\common\mstristrip.cpp
# End Source File
# Begin Source File

SOURCE=.\objsupport.cpp
# End Source File
# Begin Source File

SOURCE=.\optimize.cpp
# End Source File
# Begin Source File

SOURCE=.\perfstats.cpp
# End Source File
# Begin Source File

SOURCE=..\common\physdll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\resourcemanager.cpp
# End Source File
# Begin Source File

SOURCE=..\common\scriplib.cpp
# End Source File
# Begin Source File

SOURCE=.\simplify.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\studio.cpp
# End Source File
# Begin Source File

SOURCE=.\studiomdl.cpp
# End Source File
# Begin Source File

SOURCE=.\UnifyLODs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlsymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\v1support.cpp
# End Source File
# Begin Source File

SOURCE=.\write.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\Public\amd3dx.h
# End Source File
# Begin Source File

SOURCE=..\..\public\basehandle.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\basetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\bitvec.h
# End Source File
# Begin Source File

SOURCE=..\..\public\bone_accessor.h
# End Source File
# Begin Source File

SOURCE=..\..\public\bone_setup.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\bspflags.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\characterset.h
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\cmodel.h
# End Source File
# Begin Source File

SOURCE=.\collisionmodel.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\CollisionUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\commonmacros.h
# End Source File
# Begin Source File

SOURCE=..\..\public\compressed_vector.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\const.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vphysics\constraints.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\dbg.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\fasttimer.h
# End Source File
# Begin Source File

SOURCE=.\FileBuffer.h
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

SOURCE=..\..\public\gametrace.h
# End Source File
# Begin Source File

SOURCE=.\HardwareMatrixState.h
# End Source File
# Begin Source File

SOURCE=.\HardwareVertexCache.h
# End Source File
# Begin Source File

SOURCE=..\..\public\appframework\IAppSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\ICommandLine.h
# End Source File
# Begin Source File

SOURCE=..\..\public\ihandleentity.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\ImageLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\public\materialsystem\imaterial.h
# End Source File
# Begin Source File

SOURCE=..\..\public\materialsystem\imaterialsystem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\materialsystem\imaterialvar.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\interface.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\IStudioRender.h
# End Source File
# Begin Source File

SOURCE=..\..\public\materialsystem\materialsystem_config.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\MATHLIB.H
# End Source File
# Begin Source File

SOURCE=.\matsys.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memdbgoff.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memdbgon.h
# End Source File
# Begin Source File

SOURCE=..\common\mstristrip.h
# End Source File
# Begin Source File

SOURCE=..\NvTriStripLib\NvTriStrip.h
# End Source File
# Begin Source File

SOURCE=..\..\public\optimize.h
# End Source File
# Begin Source File

SOURCE=.\perfstats.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\phyfile.h
# End Source File
# Begin Source File

SOURCE=..\common\physdll.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\public\protected_things.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\random.h
# End Source File
# Begin Source File

SOURCE=..\common\scriplib.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\string_t.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\strtools.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\studio.h
# End Source File
# Begin Source File

SOURCE=.\studiomdl.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utldict.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utllinkedlist.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlrbtree.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlsymbol.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlvector.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vcollide.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vcollide_parse.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vector.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vector2d.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vector4d.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vmatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vphysics_interface.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vplane.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\vprof.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\vstdlib.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\lib\public\vstdlib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\nvtristrip.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# End Target
# End Project
