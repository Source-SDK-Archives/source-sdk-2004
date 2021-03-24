# Microsoft Developer Studio Project File - Name="vvis" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vvis - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vvis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vvis.mak" CFG="vvis - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vvis - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vvis - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/utils/vvis", UEVBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vvis - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W4 /GX /Zi /O2 /I "..\common" /I "..\..\public" /I "..\..\public\tier1" /I "..\vmpi" /I "..\vmpi\mysql\include" /D "NDEBUG" /D "_WIN32" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MPI" /D "PROTECTED_THINGS_DISABLE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 user32.lib ws2_32.lib winspool.lib advapi32.lib /nologo /dll /map /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /libpath:"..\..\lib\public" /libpath:"..\..\lib\common"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Copying
TargetDir=.\Release
TargetPath=.\Release\vvis.dll
TargetName=vvis
InputPath=.\Release\vvis.dll
SOURCE="$(InputPath)"

"..\..\..\bin\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\bin\$(TargetName).dll attrib -r ..\..\..\bin\$(TargetName).dll 
	copy $(TargetPath) ..\..\..\bin 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\bin\$(TargetName).map 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "vvis - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MTd /W4 /Gm /GX /ZI /Od /I "..\common" /I "..\..\public" /I "..\..\public\tier1" /I "..\vmpi" /I "..\vmpi\mysql\include" /D "_DEBUG" /D "_WIN32" /D "_USRDLL" /D "WIN32" /D "WINDOWS" /D "_MBCS" /D "MPI" /D "PROTECTED_THINGS_DISABLE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib user32.lib ws2_32.lib winspool.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\lib\public" /libpath:"..\..\lib\common"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Copying to bin
TargetDir=.\Debug
TargetPath=.\Debug\vvis.dll
TargetName=vvis
InputPath=.\Debug\vvis.dll
SOURCE="$(InputPath)"

"..\..\..\bin\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\bin\$(TargetName).dll attrib -r ..\..\..\bin\$(TargetName).dll 
	copy $(TargetPath) ..\..\..\bin 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\bin\$(TargetName).map 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "vvis - Win32 Release"
# Name "vvis - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\common\bsplib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\characterset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\checksum_crc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\checksum_md5.cpp
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\collisionutils.cpp
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

SOURCE=..\common\filesystem_tools.h
# End Source File
# Begin Source File

SOURCE=.\flow.cpp
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

SOURCE=..\common\mpi_stats.cpp
# End Source File
# Begin Source File

SOURCE=.\mpivis.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MySqlDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\common\pacifier.cpp
# End Source File
# Begin Source File

SOURCE=..\common\scriplib.cpp
# End Source File
# Begin Source File

SOURCE=..\common\threads.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlsymbol.cpp
# End Source File
# Begin Source File

SOURCE=..\common\vmpi_tools_shared.cpp
# End Source File
# Begin Source File

SOURCE=.\vvis.cpp
# End Source File
# Begin Source File

SOURCE=.\WaterDist.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Public\amd3dx.h
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

SOURCE=..\common\bsplib.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\BSPTreeData.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\bumpvects.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\checksum_crc.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\checksum_md5.h
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\cmodel.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\commonmacros.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\GameBSPFile.h
# End Source File
# Begin Source File

SOURCE=..\common\ISQLDBReplyTarget.h
# End Source File
# Begin Source File

SOURCE=..\common\ivvisdll.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\MATHLIB.H
# End Source File
# Begin Source File

SOURCE=.\mpivis.h
# End Source File
# Begin Source File

SOURCE=..\common\MySqlDatabase.h
# End Source File
# Begin Source File

SOURCE=..\common\pacifier.h
# End Source File
# Begin Source File

SOURCE=..\common\scriplib.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\strtools.h
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

SOURCE=.\vis.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\vmpi_distribute_work.h
# End Source File
# Begin Source File

SOURCE=..\common\vmpi_tools_shared.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vstdlib\vstdlib.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\wadtypes.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\lib\public\vstdlib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\vmpi\vmpi.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# End Target
# End Project
