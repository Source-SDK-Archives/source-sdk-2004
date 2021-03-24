//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef FILESYSTEM_INIT_H
#define FILESYSTEM_INIT_H
#ifdef _WIN32
#pragma once
#endif


#include "filesystem.h"


#if defined( _WIN32 ) || defined( WIN32 )
#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')
#else	//_WIN32
#define PATHSEPARATOR(c) ((c) == '/')
#endif	//_WIN32


enum FSReturnCode_t
{
	FS_OK,
	FS_MISSING_GAMEINFO_FILE,
	FS_INVALID_GAMEINFO_FILE,
	FS_INVALID_PARAMETERS,
	FS_UNABLE_TO_INIT,
	FS_MISSING_STEAM_DLL
};



class CFSLoadModuleInfo
{
public:
	CFSLoadModuleInfo();

// Inputs.
public:
	// Full path to the file system DLL (gotten from FileSystem_GetFileSystemDLLName).
	const char		*m_pFileSystemDLLName;

	// If this is set, then the init code will look in this directory up to the root for gameinfo.txt.
	// It must be set for FileSystem_LoadSearchPaths to work.
	//
	// (default: null)
	const char		*m_pDirectoryName;

	// If this is true, then it won't look at -vproject, -game, or the vproject environment variable
	// to find gameinfo.txt. If this is true, then m_pDirectoryName must be set.
	//
	// (default: false)
	bool			m_bOnlyUseDirectoryName;

	// If this is true, then:
	// 1. It will set the environment variables that steam.dll looks at for startup info.
	// 2. It will look for ToolsAppId in the gameinfo.txt file and load the
	// steam caches associated with that cache if it's there. This is so apps like Hammer and hlmv
	// can load the main steam caches (like for Counter-Strike or Half-Life 2), and also load the
	// caches that include tools-specific materials (materials\editor, materials\debug, etc).
	//
	// (default: true - should be FALSE for the engine)
	bool			m_bToolsMode;

	// Passed to IFileSystem::Connect.
	CreateInterfaceFn	m_ConnectFactory;

	// Are we loading the Steam filesystem? This should be the same value that 
	// FileSystem_GetFileSystemDLLName gave you.
	bool			m_bSteam;

// Outputs (if it returns FS_OK).
public:
	// The filesystem you got from FileSystem_LoadFileSystemModule.
	IFileSystem		*m_pFileSystem;
	CSysModule		*m_pModule;

	char			m_GameInfoPath[512];	// The directory that gameinfo.txt lives in.
};	


class CFSMountContentInfo
{
public:
	CFSMountContentInfo();

// Inputs.
public:

	// See CFSLoadModuleInfo::m_bToolsMode (this valid should always be the same as you passed to CFSLoadModuleInfo::m_bToolsMode).
	bool				m_bToolsMode;

	// This specifies the directory where gameinfo.txt is. This must be set.
	// It can come from CFSLoadModuleInfo::m_GameInfoPath.
	const char			*m_pDirectoryName;

	// Gotten from CFSLoadModuleInfo::m_pFileSystem.
	IFileSystem			*m_pFileSystem;
};


class CFSSearchPathsInit
{
public:
	CFSSearchPathsInit();

// Inputs.
public:
	// This specifies the directory where gameinfo.txt is. This must be set.
	const char		*m_pDirectoryName;

	// If this is set, then it will add a search path with _<language> appended to the pathname
	// for each search path with a path ID of "game".
	// (default: null)
	const char		*m_pLanguage;

	// This is the filesystem FileSystem_LoadSearchPaths is talking to.
	IFileSystem		*m_pFileSystem;

// Outputs.
public:
	// This is the location of the first search path called "game", which also becomes your "mod" search path.
	char			m_ModPath[512];
};



// This is used by all things that use the application framework:
// Note that the application framework automatically takes care of steps 1 and 2 if you use CSteamApplication.
// Step 1: Ask filesystem_init for the name of the filesystem DLL to load
FSReturnCode_t FileSystem_GetFileSystemDLLName( char *pFileSystemDLL, int nMaxLen, bool &bSteam );

// Step 2: Use filesystem framework to load/connect/init that filesystem DLL
FSReturnCode_t FileSystem_LoadFileSystemModule( CFSLoadModuleInfo &info );

// Step 3: Ask filesystem_init to set up the executable search path, and mount the steam content based on the mod gameinfo.txt file
FSReturnCode_t FileSystem_MountContent( CFSMountContentInfo &fsInfo );

// Step 4: Load the search paths out of pGameDirectory\gameinfo.txt.
FSReturnCode_t FileSystem_LoadSearchPaths( CFSSearchPathsInit &initInfo );


// This is automatically done during step 1, but if you want to redo all the search
// paths (like Hammer does), you can call this to reset executable_path.
FSReturnCode_t FileSystem_SetBasePaths( IFileSystem *pFileSystem );


// If this is true, then it will automatically call Error() if it has problems.
// Otherwise, it will return a status code and you can call FileSystem_GetLastErrorString().
// (default: true)
void FileSystem_SetHandleErrorsAutomatically( bool bAuto );

// Returns the last error.
const char *FileSystem_GetLastErrorString();

void Q_getwd( char *out, int outSize );




#endif // FILESYSTEM_INIT_H
