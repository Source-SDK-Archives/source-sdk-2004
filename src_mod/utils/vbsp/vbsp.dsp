# Microsoft Developer Studio Project File - Name="vbsp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vbsp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vbsp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vbsp.mak" CFG="vbsp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vbsp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vbsp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/utils/vbsp", SEVBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vbsp - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /Zi /O2 /I "..\..\Public" /I "..\..\Public\tier1" /I "..\common" /I "..\vmpi" /I "..\..\common" /D "NDEBUG" /D "MACRO_MATHLIB" /D "_WIN32" /D "_CONSOLE" /D "_MBCS" /D "PROTECTED_THINGS_DISABLE" /Fr /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /map /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcd"
# Begin Custom Build - Copying
TargetDir=.\Release
TargetPath=.\Release\vbsp.exe
TargetName=vbsp
InputPath=.\Release\vbsp.exe
SOURCE="$(InputPath)"

"..\..\..\bin\$(TargetName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\bin\$(TargetName).exe attrib -r ..\..\..\bin\$(TargetName).exe 
	copy $(TargetPath)  ..\..\..\bin 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\bin\$(TargetName).map 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "vbsp - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MTd /W3 /Gm /ZI /Od /I "..\..\Public" /I "..\..\Public\tier1" /I "..\common" /I "..\vmpi" /I "..\..\common" /D "_DEBUG" /D "DEBUG" /D "MACRO_MATHLIB" /D "_WIN32" /D "_CONSOLE" /D "_MBCS" /D "PROTECTED_THINGS_DISABLE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libc" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Copying
TargetDir=.\Debug
TargetPath=.\Debug\vbsp.exe
TargetName=vbsp
InputPath=.\Debug\vbsp.exe
SOURCE="$(InputPath)"

"..\..\..\bin\$(TargetName).exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\bin\$(TargetName).exe attrib -r ..\..\..\bin\$(TargetName).exe 
	copy $(TargetPath)  ..\..\..\bin 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\bin\$(TargetName).map 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "vbsp - Win32 Release"
# Name "vbsp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Common Files"

# PROP Default_Filter "*.c, *.cpp"
# Begin Source File

SOURCE=..\common\bsplib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\builddisp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\characterset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\checksum_crc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\ChunkFile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\common\filesystem_tools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\interface.cpp
# End Source File
# Begin Source File

SOURCE=..\common\map_shared.cpp
# End Source File
# Begin Source File

SOURCE=..\common\pacifier.cpp
# End Source File
# Begin Source File

SOURCE=..\common\polylib.cpp
# End Source File
# Begin Source File

SOURCE=..\common\scriplib.cpp
# End Source File
# Begin Source File

SOURCE=..\common\threads.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\tokenreader.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\brushbsp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\CollisionUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\csg.cpp
# End Source File
# Begin Source File

SOURCE=.\cubemap.cpp
# End Source File
# Begin Source File

SOURCE=.\detail.cpp
# End Source File
# Begin Source File

SOURCE=.\detailObjects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_common.cpp
# End Source File
# Begin Source File

SOURCE=.\disp_ivp.cpp
# End Source File
# Begin Source File

SOURCE=.\disp_lightmap_alpha.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_powerinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\disp_vbsp.cpp
# End Source File
# Begin Source File

SOURCE=.\faces.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_init.cpp
# End Source File
# Begin Source File

SOURCE=.\glfile.cpp
# End Source File
# Begin Source File

SOURCE=.\ivp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\KeyValues.cpp
# End Source File
# Begin Source File

SOURCE=.\leakfile.cpp
# End Source File
# Begin Source File

SOURCE=.\map.cpp
# End Source File
# Begin Source File

SOURCE=.\materialpatch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\Mathlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\mempool.cpp
# End Source File
# Begin Source File

SOURCE=..\common\mstristrip.cpp
# End Source File
# Begin Source File

SOURCE=.\nodraw.cpp
# End Source File
# Begin Source File

SOURCE=.\normals.cpp
# End Source File
# Begin Source File

SOURCE=.\overlay.cpp
# End Source File
# Begin Source File

SOURCE=..\common\physdll.cpp
# End Source File
# Begin Source File

SOURCE=.\portals.cpp
# End Source File
# Begin Source File

SOURCE=.\prtfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\ScratchPad3D.cpp
# End Source File
# Begin Source File

SOURCE=..\common\scratchpad_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\StaticProp.cpp
# End Source File
# Begin Source File

SOURCE=.\textures.cpp
# End Source File
# Begin Source File

SOURCE=.\tree.cpp
# End Source File
# Begin Source File

SOURCE=..\common\utilmatlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlsymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\vbsp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\vmatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\writebsp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Common header files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\common\bsplib.h
# End Source File
# Begin Source File

SOURCE=..\..\public\builddisp.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\ChunkFile.h
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.h
# End Source File
# Begin Source File

SOURCE=.\disp_ivp.h
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

SOURCE=..\..\Public\GameBSPFile.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\interface.h
# End Source File
# Begin Source File

SOURCE=.\ivp.h
# End Source File
# Begin Source File

SOURCE=..\common\map_shared.h
# End Source File
# Begin Source File

SOURCE=..\common\pacifier.h
# End Source File
# Begin Source File

SOURCE=..\common\polylib.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\tokenreader.h
# End Source File
# Begin Source File

SOURCE=..\common\utilmatlib.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\vmpi.h
# End Source File
# Begin Source File

SOURCE=..\common\zip_uncompressed.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\csg.h
# End Source File
# Begin Source File

SOURCE=.\detail.h
# End Source File
# Begin Source File

SOURCE=.\disp_lightmap_alpha.h
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_powerinfo.h
# End Source File
# Begin Source File

SOURCE=.\disp_vbsp.h
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_vertindex.h
# End Source File
# Begin Source File

SOURCE=.\faces.h
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_init.h
# End Source File
# Begin Source File

SOURCE=.\map.h
# End Source File
# Begin Source File

SOURCE=.\materialpatch.h
# End Source File
# Begin Source File

SOURCE=..\common\scratchpad_helpers.h
# End Source File
# Begin Source File

SOURCE=.\vbsp.h
# End Source File
# Begin Source File

SOURCE=.\writebsp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Public Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Public\amd3dx.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\arraystack.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\basetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\BSPFILE.H
# End Source File
# Begin Source File

SOURCE=..\..\Public\bspflags.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\BSPTreeData.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\bumpvects.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\cmodel.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\CollisionUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\commonmacros.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\dbg\dbg.h
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_common.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\ImageLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\IScratchPad3D.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\MATHLIB.H
# End Source File
# Begin Source File

SOURCE=..\common\mstristrip.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\nmatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\NTree.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\nvector.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\phyfile.h
# End Source File
# Begin Source File

SOURCE=..\common\physdll.h
# End Source File
# Begin Source File

SOURCE=..\common\qfiles.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\ScratchPad3D.h
# End Source File
# Begin Source File

SOURCE=..\common\scriplib.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\studio.h
# End Source File
# Begin Source File

SOURCE=..\common\threads.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlbuffer.h
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

SOURCE=..\..\Public\wadtypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\worldsize.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\notes.txt
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\vtf.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\vstdlib.lib
# End Source File
# End Target
# End Project
