# Microsoft Developer Studio Project File - Name="vrad" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vrad - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vrad.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vrad.mak" CFG="vrad - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vrad - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vrad - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Src/utils/vrad", TEVBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vrad - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vrad___Win32_Release"
# PROP BASE Intermediate_Dir "vrad___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vrad___Win32_Release"
# PROP Intermediate_Dir "vrad___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MTd /W4 /O2 /I "..\common" /I "..\..\Public" /I "..\..\Public\tier1" /D "NDEBUG" /D "_WIN32" /D "_USRDLL" /D "_WINDOWS" /D "_MBCS" /U "_DEBUG" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G6 /MT /W4 /GX /Zi /O2 /I "..\common" /I "..\..\Public" /I "..\..\Public\tier1" /I "..\vmpi" /I "..\vmpi\mysql\mysqlpp\include" /I "..\vmpi\mysql\include" /D "NDEBUG" /D "_WIN32" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MPI" /D "PROTECTED_THINGS_DISABLE" /U "_DEBUG" /FAs /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /dll /machine:I386 /libpath:"..\..\lib\common" /libpath:"..\..\lib\public"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 user32.lib ws2_32.lib advapi32.lib /nologo /dll /map /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /libpath:"..\..\lib\common" /libpath:"..\..\lib\public"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Copying
TargetDir=.\vrad___Win32_Release
TargetPath=.\vrad___Win32_Release\vrad.dll
TargetName=vrad
InputPath=.\vrad___Win32_Release\vrad.dll
SOURCE="$(InputPath)"

"..\..\..\bin\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\bin\$(TargetName).dll attrib -r ..\..\..\bin\$(TargetName).dll 
	copy $(TargetPath) ..\..\..\bin 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\bin\$(TargetName).map 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "vrad - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vrad___Win32_Debug"
# PROP BASE Intermediate_Dir "vrad___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vrad___Win32_Debug"
# PROP Intermediate_Dir "vrad___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MTd /W4 /Gm /ZI /Od /I "..\common" /I "..\..\Public" /I "..\..\Public\tier1" /D "_DEBUG" /D "_WIN32" /D "_USRDLL" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MTd /W4 /Gm /GX /ZI /Od /I "..\common" /I "..\..\Public" /I "..\..\Public\tier1" /I "..\vmpi" /I "..\vmpi\mysql\mysqlpp\include" /I "..\vmpi\mysql\include" /D "_DEBUG" /D "_WIN32" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "MPI" /D "PROTECTED_THINGS_DISABLE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\lib\common" /libpath:"..\..\lib\public"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 user32.lib ws2_32.lib advapi32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\lib\common" /libpath:"..\..\lib\public"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Copying
TargetDir=.\vrad___Win32_Debug
TargetPath=.\vrad___Win32_Debug\vrad.dll
TargetName=vrad
InputPath=.\vrad___Win32_Debug\vrad.dll
SOURCE="$(InputPath)"

"..\..\..\bin\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\bin\$(TargetName).dll attrib -r ..\..\..\bin\$(TargetName).dll 
	copy $(TargetPath) ..\..\..\bin 
	if exist $(TargetDir)\$(TargetName).map copy $(TargetDir)\$(TargetName).map ..\..\..\bin\$(TargetName).map 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "vrad - Win32 Release"
# Name "vrad - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\bsplib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\builddisp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\bumpvects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\ChunkFile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\DispColl_Common.cpp
# End Source File
# Begin Source File

SOURCE=..\common\map_shared.cpp
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
# Begin Group "Public Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\public\anorms.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\bitbuf.cpp
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

SOURCE=..\..\Public\CollisionUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\KeyValues.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\KeyValues.h
# End Source File
# Begin Source File

SOURCE=..\..\public\map_utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\Mathlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\ScratchPad3D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\ScratchPadUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlsymbol.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Public\BSPTreeData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_common.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_powerinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\disp_vrad.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\imageloader.cpp
# End Source File
# Begin Source File

SOURCE=.\incremental.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\leaf_ambient_lighting.cpp
# End Source File
# Begin Source File

SOURCE=.\lightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\macro_texture.cpp
# End Source File
# Begin Source File

SOURCE=..\common\mpi_stats.cpp
# End Source File
# Begin Source File

SOURCE=.\mpivrad.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MySqlDatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\common\pacifier.cpp
# End Source File
# Begin Source File

SOURCE=..\common\physdll.cpp
# End Source File
# Begin Source File

SOURCE=.\radial.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleHash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\tgaloader.cpp
# End Source File
# Begin Source File

SOURCE=.\trace.cpp
# End Source File
# Begin Source File

SOURCE=..\common\utilmatlib.cpp
# End Source File
# Begin Source File

SOURCE=.\vismat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\vmatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\common\vmpi_tools_shared.cpp
# End Source File
# Begin Source File

SOURCE=..\common\vmpi_tools_shared.h
# End Source File
# Begin Source File

SOURCE=.\vrad.cpp
# End Source File
# Begin Source File

SOURCE=.\VRAD_DispColl.cpp
# End Source File
# Begin Source File

SOURCE=.\VradDetailProps.cpp
# End Source File
# Begin Source File

SOURCE=.\VRadDisps.cpp
# End Source File
# Begin Source File

SOURCE=.\vraddll.cpp
# End Source File
# Begin Source File

SOURCE=.\VRadStaticProps.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Common Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\bsplib.h
# End Source File
# Begin Source File

SOURCE=..\common\cmdlib.h
# End Source File
# Begin Source File

SOURCE=..\common\consolewnd.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\ichannel.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\imysqlwrapper.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\iphelpers.h
# End Source File
# Begin Source File

SOURCE=..\common\ISQLDBReplyTarget.h
# End Source File
# Begin Source File

SOURCE=..\common\map_shared.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\messbuf.h
# End Source File
# Begin Source File

SOURCE=..\common\mpi_stats.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\mysql_wrapper.h
# End Source File
# Begin Source File

SOURCE=..\common\MySqlDatabase.h
# End Source File
# Begin Source File

SOURCE=..\common\pacifier.h
# End Source File
# Begin Source File

SOURCE=..\common\polylib.h
# End Source File
# Begin Source File

SOURCE=..\common\scriplib.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\threadhelpers.h
# End Source File
# Begin Source File

SOURCE=..\common\threads.h
# End Source File
# Begin Source File

SOURCE=..\common\utilmatlib.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\vmpi_defs.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\vmpi_dispatch.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\vmpi_distribute_work.h
# End Source File
# Begin Source File

SOURCE=..\vmpi\vmpi_filesystem.h
# End Source File
# End Group
# Begin Group "Public Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Public\amd3dx.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\ANORMS.H
# End Source File
# Begin Source File

SOURCE=..\..\public\basehandle.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\basetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\bitbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\bitvec.h
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

SOURCE=..\..\public\builddisp.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\bumpvects.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\characterset.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\checksum_crc.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\checksum_md5.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\ChunkFile.h
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

SOURCE=..\..\public\compressed_vector.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\const.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\coordsize.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\dbg.h
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_common.h
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_powerinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\public\disp_vertindex.h
# End Source File
# Begin Source File

SOURCE=..\..\public\DispColl_Common.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\fasttimer.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\FileSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.h
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_passthru.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\GameBSPFile.h
# End Source File
# Begin Source File

SOURCE=..\..\public\gametrace.h
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

SOURCE=..\..\public\imageloader.h
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

SOURCE=..\..\public\iscratchpad3d.h
# End Source File
# Begin Source File

SOURCE=..\..\public\ivraddll.h
# End Source File
# Begin Source File

SOURCE=..\..\public\materialsystem\materialsystem_config.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\MATHLIB.H
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memdbgon.h
# End Source File
# Begin Source File

SOURCE=..\..\public\optimize.h
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

SOURCE=..\..\Public\s3_intrf.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\ScratchPad3D.h
# End Source File
# Begin Source File

SOURCE=..\..\public\ScratchPadUtils.h
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

SOURCE=..\..\public\terrainmod.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tgaloader.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\tokenreader.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\trace.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utldict.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlhash.h
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

SOURCE=..\vmpi\vmpi.h
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
# Begin Source File

SOURCE=..\..\public\vtf\vtf.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\wadtypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\worldsize.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\disp_vrad.h
# End Source File
# Begin Source File

SOURCE=.\iincremental.h
# End Source File
# Begin Source File

SOURCE=.\incremental.h
# End Source File
# Begin Source File

SOURCE=.\leaf_ambient_lighting.h
# End Source File
# Begin Source File

SOURCE=.\lightmap.h
# End Source File
# Begin Source File

SOURCE=.\macro_texture.h
# End Source File
# Begin Source File

SOURCE=..\..\public\map_utils.h
# End Source File
# Begin Source File

SOURCE=.\mpivrad.h
# End Source File
# Begin Source File

SOURCE=.\radial.h
# End Source File
# Begin Source File

SOURCE=.\vismat.h
# End Source File
# Begin Source File

SOURCE=.\vrad.h
# End Source File
# Begin Source File

SOURCE=.\VRAD_DispColl.h
# End Source File
# Begin Source File

SOURCE=.\vraddetailprops.h
# End Source File
# Begin Source File

SOURCE=.\vraddll.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\notes.txt
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\vstdlib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\vmpi\vmpi.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\nvtc.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\vtf.lib
# End Source File
# End Target
# End Project
