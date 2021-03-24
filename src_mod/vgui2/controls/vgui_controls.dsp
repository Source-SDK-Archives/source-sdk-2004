# Microsoft Developer Studio Project File - Name="vgui_controls" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgui_controls - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgui_controls.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgui_controls.mak" CFG="vgui_controls - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgui_controls - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vgui_controls - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/GoldSrc/vgui2/controls", YCLCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgui_controls - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GR /O2 /I "..\..\public" /I "..\..\public\tier1" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
ProjDir=.
TargetPath=.\Release\vgui_controls.lib
TargetName=vgui_controls
InputPath=.\Release\vgui_controls.lib
SOURCE="$(InputPath)"

"$(ProjDir)\..\..\lib\public\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call ..\..\filecopy.bat $(TargetPath) $(ProjDir)\..\..\lib\public\$(TargetName).lib

# End Custom Build

!ELSEIF  "$(CFG)" == "vgui_controls - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /W4 /Gm /GR /ZI /Od /I "..\..\public" /I "..\..\public\tier1" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
ProjDir=.
TargetPath=.\Debug\vgui_controls.lib
TargetName=vgui_controls
InputPath=.\Debug\vgui_controls.lib
SOURCE="$(InputPath)"

"$(ProjDir)\..\..\lib\public\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	call ..\..\filecopy.bat $(TargetPath) $(ProjDir)\..\..\lib\public\$(TargetName).lib

# End Custom Build

!ENDIF 

# Begin Target

# Name "vgui_controls - Win32 Release"
# Name "vgui_controls - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\public\vgui_controls\AnimatingImagePanel.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\vgui_controls\AnimationController.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\BitmapImagePanel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\BuildGroup.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\BuildModeDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Button.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\CheckButton.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\CheckButtonList.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui\Color.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ComboBox.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Controls.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\DialogManager.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\DirectorySelectDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Divider.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\EditablePanel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\FileOpenDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\Public\FileSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\filesystem_helpers.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\FocusNavGroup.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Frame.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\GraphPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\HTML.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Image.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ImageList.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ImagePanel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\interface.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\KeyValues.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Label.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ListPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ListViewPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memdbgoff.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier0\memdbgon.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\mempool.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Menu.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\MenuBar.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\MenuButton.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\MenuItem.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\MessageBox.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Panel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\PanelAnimationVar.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\PanelListPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\PHandle.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ProgressBar.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ProgressBox.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\PropertyDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\PropertyPage.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\PropertySheet.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\QueryBox.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\RadioButton.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\RichText.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ScrollBar.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ScrollBarSlider.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\SectionedListPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Slider.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\TextEntry.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\TextImage.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\ToggleButton.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\Tooltip.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\TreeView.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\TreeViewListControl.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\URLLabel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlrbtree.h
# End Source File
# Begin Source File

SOURCE=..\..\public\tier1\utlvector.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\WizardPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\public\vgui_controls\WizardSubPanel.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AnimatingImagePanel.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationController.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapImagePanel.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildModeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Button.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\characterset.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckButtonList.cpp
# End Source File
# Begin Source File

SOURCE=.\ComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\controls.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectorySelectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Divider.cpp
# End Source File
# Begin Source File

SOURCE=.\EditablePanel.cpp
# End Source File
# Begin Source File

SOURCE=.\FileOpenDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Public\filesystem_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\FocusNavGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\HTML.cpp
# End Source File
# Begin Source File

SOURCE=.\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageList.cpp
# End Source File
# Begin Source File

SOURCE=.\ImagePanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\KeyValues.cpp
# End Source File
# Begin Source File

SOURCE=.\Label.cpp
# End Source File
# Begin Source File

SOURCE=.\ListPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ListViewPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\mempool.cpp
# End Source File
# Begin Source File

SOURCE=.\Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuButton.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuItem.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel.cpp
# End Source File
# Begin Source File

SOURCE=.\PanelListPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressBox.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\QueryBox.cpp
# End Source File
# Begin Source File

SOURCE=.\RadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\RichText.cpp
# End Source File
# Begin Source File

SOURCE=.\ScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ScrollBarSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\SectionedListPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\Slider.cpp
# End Source File
# Begin Source File

SOURCE=.\TextEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\TextImage.cpp
# End Source File
# Begin Source File

SOURCE=.\ToggleButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Tooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeViewListControl.cpp
# End Source File
# Begin Source File

SOURCE=.\URLLabel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tier1\utlsymbol.cpp
# End Source File
# Begin Source File

SOURCE=.\WizardPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\WizardSubPanel.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# End Target
# End Project
