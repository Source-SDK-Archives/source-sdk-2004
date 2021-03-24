//====== Copyright © 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#undef PROTECTED_THINGS_ENABLE
#undef fopen
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <io.h> // _chmod
#elif _LINUX
#include <unistd.h>
#define _putenv putenv
#define _chdir chdir
#define _access access
#endif
#include <stdio.h>
#include <sys/stat.h>
#include "vstdlib/strtools.h"
#include "filesystem_init.h"
#include "vstdlib/ICommandLine.h"
#include "KeyValues.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

#define GAMEINFO_FILENAME			"gameinfo.txt"


char g_FileSystemError[256];
bool g_bHandleErrorsAutomatically = true;


// This class lets you modify environment variables, and it restores the original value
// when it goes out of scope.
class CTempEnvVar
{
public:
	CTempEnvVar( const char *pVarName )
	{
		m_pVarName = pVarName;
		const char *pValue = getenv( pVarName );
		if ( pValue )
		{
			m_bExisted = true;
			m_OriginalValue.SetSize( strlen( pValue ) + 1 );
			memcpy( m_OriginalValue.Base(), pValue, m_OriginalValue.Count() );
		}
		else
		{
			m_bExisted = false;
		}
	}

	~CTempEnvVar()
	{
		// Restore the original value.
		if ( m_bExisted )
		{
			SetValue( "%s", m_OriginalValue.Base() );
		}
		else
		{
			ClearValue();
		}
	}

	const char* GetValue()
	{
		return getenv( m_pVarName );
	}

	void SetValue( const char *pValue, ... )
	{
		char valueString[4096];
		va_list marker;
		va_start( marker, pValue );
		Q_vsnprintf( valueString, sizeof( valueString ), pValue, marker );
		va_end( marker );

		char str[4096];
		Q_snprintf( str, sizeof( str ), "%s=%s", m_pVarName, valueString );
		_putenv( str );
	}

	void ClearValue()
	{
		char str[512];
		Q_snprintf( str, sizeof( str ), "%s=", m_pVarName );
		_putenv( str );
	}

private:
	const char *m_pVarName;
	bool m_bExisted;
	CUtlVector<char> m_OriginalValue;
};


class CSteamEnvVars
{
public:
	CSteamEnvVars() :
		m_SteamAppId( "SteamAppId" ),
		m_SteamUserPassphrase( "SteamUserPassphrase" ),
		m_SteamAppUser( "SteamAppUser" ),
		m_Path( "path" )
	{
	}

	CTempEnvVar m_SteamAppId;
	CTempEnvVar m_SteamUserPassphrase;
	CTempEnvVar m_SteamAppUser;
	CTempEnvVar m_Path;
};


// ---------------------------------------------------------------------------------------------------- //
// Helpers.
// ---------------------------------------------------------------------------------------------------- //

void Q_getwd( char *out, int outSize )
{
#if defined( _WIN32 ) || defined( WIN32 )
	_getcwd( out, outSize );
	Q_strncat( out, "\\", outSize, COPY_ALL_CHARACTERS );
#else
	getcwd(out, outSize);
	strcat(out, "/");
#endif
	Q_FixSlashes( out );
}


// ---------------------------------------------------------------------------------------------------- //
// Module interface.
// ---------------------------------------------------------------------------------------------------- //

CFSSearchPathsInit::CFSSearchPathsInit()
{
	m_pDirectoryName = NULL;
	m_pLanguage = NULL;
	m_ModPath[0] = 0;
}


CFSLoadModuleInfo::CFSLoadModuleInfo()
{
	m_pFileSystemDLLName = NULL;
	m_pDirectoryName = NULL;
	m_bOnlyUseDirectoryName = false;
	m_pFileSystem = NULL;
	m_pModule = NULL;
	m_bSteam = false;
	m_bToolsMode = true;
}


CFSMountContentInfo::CFSMountContentInfo()
{
	m_bToolsMode = true;
	m_pDirectoryName = NULL;
	m_pFileSystem = NULL;
}


const char *FileSystem_GetLastErrorString()
{
	return g_FileSystemError;
}


FSReturnCode_t SetupFileSystemError( FSReturnCode_t retVal, const char *pMsg, ... )
{
	va_list marker;
	va_start( marker, pMsg );
	Q_vsnprintf( g_FileSystemError, sizeof( g_FileSystemError ), pMsg, marker );
	va_end( marker );

	if ( g_bHandleErrorsAutomatically )
		Error( "%s\n", g_FileSystemError );
	
	return retVal;
}


void AddLanguageGameDir( IFileSystem *pFileSystem, const char *pLocation, const char *pLanguage )
{
#ifndef SWDS
	char temp[MAX_PATH];
	Q_snprintf( temp, sizeof(temp), "%s_%s", pLocation, pLanguage );
	pFileSystem->AddSearchPath( temp, "GAME", PATH_ADD_TO_TAIL );
#endif
}


void AddGameBinDir( IFileSystem *pFileSystem, const char *pLocation )
{
	char temp[MAX_PATH];
	Q_snprintf( temp, sizeof(temp), "%s%cbin", pLocation, CORRECT_PATH_SEPARATOR );
	pFileSystem->AddSearchPath( temp, "GAMEBIN", PATH_ADD_TO_TAIL );
}


KeyValues* ReadKeyValuesFile( const char *pFilename )
{
	FILE *fp = fopen( pFilename, "rb" );
	if ( !fp )
		return NULL;

	// Read in the gameinfo.txt file and null-terminate it.
	CUtlVector<char> buf;
	fseek( fp, 0, SEEK_END );
	buf.SetSize( ftell( fp ) + 1 );
	fseek( fp, 0, SEEK_SET );
	fread( buf.Base(), 1, buf.Count()-1, fp );
	fclose( fp );
	buf[buf.Count()-1] = 0;

	KeyValues *kv = new KeyValues( "" );
	if ( !kv->LoadFromBuffer( pFilename, buf.Base() ) )
	{
		kv->deleteThis();
		return NULL;
	}
	
	return kv;
}

static int Sys_GetExecutableName( char *out, int len )
{
#ifdef _WIN32
        if ( !::GetModuleFileName( ( HINSTANCE )GetModuleHandle( NULL ), out, len ) )
        {
                return 0;
        }
#else
	if ( CommandLine()->GetParm(0) )
	{
		Q_MakeAbsolutePath( out, len, CommandLine()->GetParm(0) );
	}
	else
	{
		return 0;
	}
#endif	
        return 1;
}


static bool FileSystem_GetExecutableDir( char *exedir, int exeDirLen )
{
	exedir[ 0 ] = 0;
	if ( !Sys_GetExecutableName( exedir, exeDirLen ) )
		return false;

	Q_StripFilename( exedir );
	Q_FixSlashes( exedir );

	// Return the bin directory as the executable dir if it's not in there
	// because that's really where we're running from...
	char ext[MAX_PATH];
	Q_StrRight( exedir, 4, ext, sizeof( ext ) );
	if ( ext[0] != CORRECT_PATH_SEPARATOR || Q_stricmp( ext+1, "bin" ) != 0 )
	{
		Q_strncat( exedir, "\\bin", exeDirLen, COPY_ALL_CHARACTERS );
		Q_FixSlashes( exedir );
	}
	return true;
}

static bool FileSystem_GetBaseDir( char *baseDir, int baseDirLen )
{
	if ( FileSystem_GetExecutableDir( baseDir, baseDirLen ) )
	{
		Q_StripFilename( baseDir );
		return true;
	}
	else
	{
		return false;
	}
}


FSReturnCode_t LoadGameInfoFile( 
	const char *pDirectoryName, 
	KeyValues *&pMainFile, 
	KeyValues *&pFileSystemInfo, 
	KeyValues *&pSearchPaths )
{
	// If GameInfo.txt exists under pBaseDir, then this is their game directory.
	// All the filesystem mappings will be in this file.
	char gameinfoFilename[MAX_PATH];
	Q_strncpy( gameinfoFilename, pDirectoryName, sizeof( gameinfoFilename ) );
	Q_AppendSlash( gameinfoFilename, sizeof( gameinfoFilename ) );
	Q_strncat( gameinfoFilename, GAMEINFO_FILENAME, sizeof( gameinfoFilename ), COPY_ALL_CHARACTERS );
	Q_FixSlashes( gameinfoFilename );
	pMainFile = ReadKeyValuesFile( gameinfoFilename );
	if ( !pMainFile )
		return SetupFileSystemError( FS_MISSING_GAMEINFO_FILE, "%s is missing.", gameinfoFilename );

	pFileSystemInfo = pMainFile->FindKey( "FileSystem" );
	if ( !pFileSystemInfo )
	{
		pMainFile->deleteThis();
		return SetupFileSystemError( FS_INVALID_GAMEINFO_FILE, "%s is not a valid format.", gameinfoFilename );
	}

	// Now read in all the search paths.
	pSearchPaths = pFileSystemInfo->FindKey( "SearchPaths" );
	if ( !pSearchPaths )
	{
		pMainFile->deleteThis();
		return SetupFileSystemError( FS_INVALID_GAMEINFO_FILE, "%s is not a valid format.", gameinfoFilename );
	}

	return FS_OK;
}


FSReturnCode_t FileSystem_LoadSearchPaths( CFSSearchPathsInit &initInfo )
{
	if ( !initInfo.m_pFileSystem || !initInfo.m_pDirectoryName )
		return SetupFileSystemError( FS_INVALID_PARAMETERS, "FileSystem_LoadSearchPaths: Invalid parameters specified." );

	KeyValues *pMainFile, *pFileSystemInfo, *pSearchPaths;
	FSReturnCode_t retVal = LoadGameInfoFile( initInfo.m_pDirectoryName, pMainFile, pFileSystemInfo, pSearchPaths );
	if ( retVal != FS_OK )
		return retVal;

	
	// All paths except those marked with |gameinfo_path| are relative to the base dir.
	char baseDir[MAX_PATH];
	if ( !FileSystem_GetBaseDir( baseDir, sizeof( baseDir ) ) )
		return SetupFileSystemError( FS_INVALID_PARAMETERS, "FileSystem_GetBaseDir failed." );

	initInfo.m_ModPath[0] = 0;

	bool bFirstGamePath = true;
	for ( KeyValues *pCur=pSearchPaths->GetFirstValue(); pCur; pCur=pCur->GetNextValue() )
	{
		const char *pPathID = pCur->GetName();
		
		char fullLocationPath[MAX_PATH];
		const char *pLocation = pCur->GetString();
		if ( Q_stristr( pLocation, "|gameinfo_path|" ) == pLocation )
			Q_MakeAbsolutePath( fullLocationPath, sizeof( fullLocationPath ), pLocation + strlen( "|gameinfo_path|" ), initInfo.m_pDirectoryName );
		else
			Q_MakeAbsolutePath( fullLocationPath, sizeof( fullLocationPath ), pLocation, baseDir );
	
		// Add language, mod, and gamebin search paths automatically.
		if ( Q_stricmp( pPathID, "game" ) == 0 )
		{
			if ( bFirstGamePath )
			{
				bFirstGamePath = false;
				initInfo.m_pFileSystem->AddSearchPath( fullLocationPath, "MOD", PATH_ADD_TO_TAIL );
				Q_strncpy( initInfo.m_ModPath, fullLocationPath, sizeof( initInfo.m_ModPath ) );
			}
		
			if ( initInfo.m_pLanguage )
				AddLanguageGameDir( initInfo.m_pFileSystem, fullLocationPath, initInfo.m_pLanguage );
			
			AddGameBinDir( initInfo.m_pFileSystem, fullLocationPath );
		}

		initInfo.m_pFileSystem->AddSearchPath( fullLocationPath, pPathID, PATH_ADD_TO_TAIL );
	}

	pMainFile->deleteThis();

	// Also, mark specific path IDs as "by request only". That way, we won't waste time searching in them
	// when people forget to specify a search path.
	initInfo.m_pFileSystem->MarkPathIDByRequestOnly( "executable_path", true );
	initInfo.m_pFileSystem->MarkPathIDByRequestOnly( "gamebin", true );
	initInfo.m_pFileSystem->MarkPathIDByRequestOnly( "mod", true );
	if ( initInfo.m_ModPath[0] != 0 )
	{
		// Add the write path last.
		initInfo.m_pFileSystem->AddSearchPath( initInfo.m_ModPath, "DEFAULT_WRITE_PATH", PATH_ADD_TO_TAIL );
	}

#ifdef _DEBUG	
	initInfo.m_pFileSystem->PrintSearchPaths();
#endif

	return FS_OK;
}


bool DoesFileExistIn( const char *pDirectoryName, const char *pFilename )
{
	char filename[MAX_PATH];
	Q_strncpy( filename, pDirectoryName, sizeof( filename ) );
	Q_AppendSlash( filename, sizeof( filename ) );
	Q_strncat( filename, pFilename, sizeof( filename ), COPY_ALL_CHARACTERS );
	Q_FixSlashes( filename );
	return ( _access( filename, 0 ) == 0 );
}


FSReturnCode_t LocateGameInfoFile( const CFSLoadModuleInfo &fsInfo, char *pOutDir, int outDirLen )
{
	// Engine and Hammer don't want to search around for it.
	if ( fsInfo.m_bOnlyUseDirectoryName )
	{
		if ( !fsInfo.m_pDirectoryName )
			return SetupFileSystemError( FS_MISSING_GAMEINFO_FILE, "bOnlyUseDirectoryName=1 and pDirectoryName=NULL." );

		if ( !DoesFileExistIn( fsInfo.m_pDirectoryName, GAMEINFO_FILENAME ) )
			return SetupFileSystemError( FS_MISSING_GAMEINFO_FILE, "%s doesn't exist in %s.", GAMEINFO_FILENAME, fsInfo.m_pDirectoryName );

		Q_strncpy( pOutDir, fsInfo.m_pDirectoryName, outDirLen );
		return FS_OK;
	}


	// First, check for overrides on the command line or environment variables.
	const char *pProject = CommandLine()->ParmValue( "-vproject" );
	if ( !pProject )
	{
		pProject = CommandLine()->ParmValue( "-game" );
		if ( !pProject )
			pProject = getenv("VPROJECT");
	}

	if ( pProject && DoesFileExistIn( pProject, GAMEINFO_FILENAME ) )
	{
		Q_MakeAbsolutePath( pOutDir, outDirLen, pProject );
		return FS_OK;
	}

	
	// Now look for it in the directory they passed in.
	if ( fsInfo.m_pDirectoryName )
		Q_MakeAbsolutePath( pOutDir, outDirLen, fsInfo.m_pDirectoryName );
	else
		Q_MakeAbsolutePath( pOutDir, outDirLen, "." );

	do 
	{
		if ( DoesFileExistIn( pOutDir, GAMEINFO_FILENAME ) )
			return FS_OK;
	} while ( Q_StripLastDir( pOutDir, outDirLen ) );


	// use the cwd and hunt down the tree until we find something
	Q_getwd( pOutDir, outDirLen );
	do
	{
		if ( DoesFileExistIn( pOutDir, GAMEINFO_FILENAME ) )
			return FS_OK;
	} while ( Q_StripLastDir( pOutDir, sizeof( outDirLen ) ) );


	if ( fsInfo.m_pDirectoryName )
	{
		return SetupFileSystemError( FS_MISSING_GAMEINFO_FILE, 
			"Unable to find %s relative to '%s'. Solutions:\n\n"
			"1. Set environment variable 'VPROJECT' to the path where %s is.\n"
			"2. Place '%s' below the path where %s is.\n"
			"3. Add -game <path> on the command line where <path> is the directory that %s is in.\n",
			GAMEINFO_FILENAME, fsInfo.m_pDirectoryName, GAMEINFO_FILENAME, fsInfo.m_pDirectoryName, GAMEINFO_FILENAME, GAMEINFO_FILENAME );
	}
	else
	{
		return SetupFileSystemError( FS_MISSING_GAMEINFO_FILE, 
			"Unable to find %s. Solutions:\n\n"
			"1. Set environment variable 'VPROJECT' to the path where %s is.\n"
			"2. Add -game <path> on the command line where <path> is the directory that %s is in.\n",
			GAMEINFO_FILENAME, GAMEINFO_FILENAME, GAMEINFO_FILENAME );
	}

	return FS_OK;			
}


FSReturnCode_t SetSteamInstallPath( KeyValues *pSteamInfo, char *steamInstallPath, int steamInstallPathLen, CSteamEnvVars &steamEnvVars )
{
	// Start at our bin directory and move up until we find a directory with steam.dll in it.
	char executablePath[MAX_PATH];
	if ( !FileSystem_GetExecutableDir( executablePath, sizeof( executablePath ) )	)
		return SetupFileSystemError( FS_INVALID_PARAMETERS, "FileSystem_GetExecutableDir failed." );

	Q_strncpy( steamInstallPath, executablePath, steamInstallPathLen );
	while ( 1 )
	{
		// Ignore steamapp.cfg here in case they're debugging. We still need to know the real steam path so we can find their username.
		// find 
		if ( DoesFileExistIn( steamInstallPath, "steam.dll" ) && !DoesFileExistIn( steamInstallPath, "steamapp.cfg" ) )
			break;
	
		if ( !Q_StripLastDir( steamInstallPath, steamInstallPathLen ) )
		{
			return SetupFileSystemError( FS_MISSING_STEAM_DLL, "Can't find steam.dll relative to executable path: %s.", executablePath );
		}			
	}

	// Also, add the install path to their PATH environment variable, so filesystem_steam.dll can get to steam.dll.
	const char* pPath = steamEnvVars.m_Path.GetValue();
	if ( pPath && !Q_stristr( pPath, steamInstallPath ) )
	{
		steamEnvVars.m_Path.SetValue( "%s;%s", steamInstallPath, pPath );
	}
	return FS_OK;
}


void SetSteamAppUser( KeyValues *pSteamInfo, const char *steamInstallPath, CSteamEnvVars &steamEnvVars )
{
	// Always inherit the Steam user if it's already set, since it probably means we (or the
	// the app that launched us) were launched from Steam.
	if ( steamEnvVars.m_SteamAppUser.GetValue() )
		return;

	char appUser[MAX_PATH];
	const char *pTempAppUser = NULL;
	if ( pSteamInfo && (pTempAppUser = pSteamInfo->GetString( "SteamAppUser", NULL )) != NULL )
	{
		Q_strncpy( appUser, pTempAppUser, sizeof( appUser ) );
	}
	else
	{
		// They don't have SteamInfo.txt, or it's missing SteamAppUser. Try to figure out the user
		// by looking in <steam install path>\config\SteamAppData.vdf.
		char fullFilename[MAX_PATH];
		Q_strncpy( fullFilename, steamInstallPath, sizeof( fullFilename ) );
		Q_AppendSlash( fullFilename, sizeof( fullFilename ) );
		Q_strncat( fullFilename, "config\\SteamAppData.vdf", sizeof( fullFilename ), COPY_ALL_CHARACTERS );

		KeyValues *pSteamAppData = ReadKeyValuesFile( fullFilename );
		if ( !pSteamAppData || (pTempAppUser = pSteamAppData->GetString( "AutoLoginUser", NULL )) == NULL )
		{
			Error( "Can't find steam app user info." );
		}
		Q_strncpy( appUser, pTempAppUser, sizeof( appUser ) ); 
		
		pSteamAppData->deleteThis();
	}

	Q_strlower( appUser );
	steamEnvVars.m_SteamAppUser.SetValue( "%s", appUser );
}


void SetSteamUserPassphrase( KeyValues *pSteamInfo, CSteamEnvVars &steamEnvVars )
{
	// Always inherit the passphrase if it's already set, since it probably means we (or the
	// the app that launched us) were launched from Steam.
	if ( steamEnvVars.m_SteamUserPassphrase.GetValue() )
		return;

	// SteamUserPassphrase.
	const char *pStr;
	if ( pSteamInfo && (pStr = pSteamInfo->GetString( "SteamUserPassphrase", NULL )) != NULL )
	{
		steamEnvVars.m_SteamUserPassphrase.SetValue( "%s", pStr );
	}
}


void SetSteamAppId( KeyValues *pFileSystemInfo, const char *pGameInfoDirectory, CSteamEnvVars &steamEnvVars )
{
	// SteamAppId is in gameinfo.txt->FileSystem->FileSystemInfo_Steam->SteamAppId.
	int iAppId = pFileSystemInfo->GetInt( "SteamAppId", -1 );
	if ( iAppId == -1 )
		Error( "Missing SteamAppId in %s\\%s.", pGameInfoDirectory, GAMEINFO_FILENAME );

	steamEnvVars.m_SteamAppId.SetValue( "%d", iAppId );
}


FSReturnCode_t SetupSteamStartupEnvironment( KeyValues *pFileSystemInfo, const char *pGameInfoDirectory, CSteamEnvVars &steamEnvVars )
{
	// Ok, we're going to run Steam. See if they have SteamInfo.txt. If not, we'll try to deduce what we can.
	char steamInfoFile[MAX_PATH];
	Q_strncpy( steamInfoFile, pGameInfoDirectory, sizeof( steamInfoFile ) );
	Q_AppendSlash( steamInfoFile, sizeof( steamInfoFile ) );
	Q_strncat( steamInfoFile, "steaminfo.txt", sizeof( steamInfoFile ), COPY_ALL_CHARACTERS );
	KeyValues *pSteamInfo = ReadKeyValuesFile( steamInfoFile );

	char steamInstallPath[MAX_PATH];
	FSReturnCode_t ret = SetSteamInstallPath( pSteamInfo, steamInstallPath, sizeof( steamInstallPath ), steamEnvVars );
	if ( ret != FS_OK )
		return ret;

	SetSteamAppUser( pSteamInfo, steamInstallPath, steamEnvVars );
	SetSteamUserPassphrase( pSteamInfo, steamEnvVars );
	SetSteamAppId( pFileSystemInfo, pGameInfoDirectory, steamEnvVars );

	if ( pSteamInfo )
		pSteamInfo->deleteThis();

	return FS_OK;
}


FSReturnCode_t GetSteamExtraAppId( const char *pDirectoryName, int *nExtraAppId )
{
	// Now, load gameinfo.txt (to make sure it's there)
	KeyValues *pMainFile, *pFileSystemInfo, *pSearchPaths;
	FSReturnCode_t ret = LoadGameInfoFile( pDirectoryName, pMainFile, pFileSystemInfo, pSearchPaths );
	if ( ret != FS_OK )
		return ret;

	*nExtraAppId = pFileSystemInfo->GetInt( "ToolsAppId", -1 );
	pMainFile->deleteThis();
	return FS_OK;
}


FSReturnCode_t FileSystem_SetBasePaths( IFileSystem *pFileSystem )
{
	char executablePath[MAX_PATH];
	if ( !FileSystem_GetExecutableDir( executablePath, sizeof( executablePath ) )	)
		return SetupFileSystemError( FS_INVALID_PARAMETERS, "FileSystem_GetExecutableDir failed." );

	pFileSystem->AddSearchPath( executablePath, "EXECUTABLE_PATH" );
	return FS_OK;
}


//-----------------------------------------------------------------------------
// Returns the name of the file system DLL to use
//-----------------------------------------------------------------------------
FSReturnCode_t FileSystem_GetFileSystemDLLName( char *pFileSystemDLL, int nMaxLen, bool &bSteam )
{
	bSteam = false;

	// Inside of here, we don't have a filesystem yet, so we have to assume that the filesystem_stdio or filesystem_steam
	// is in this same directory with us.
	char executablePath[MAX_PATH];
	if ( !FileSystem_GetExecutableDir( executablePath, sizeof( executablePath ) )	)
		return SetupFileSystemError( FS_INVALID_PARAMETERS, "FileSystem_GetExecutableDir failed." );
	
#ifdef _WIN32
	Q_snprintf( pFileSystemDLL, nMaxLen, "%s%cfilesystem_stdio.dll", executablePath, CORRECT_PATH_SEPARATOR );

	// If filesystem_stdio.dll is missing or -steam is specified, then load filesystem_steam.dll.
	// There are two command line parameters for Steam:
	//		1) -steam (runs Steam in remote filesystem mode; requires Steam backend)
	//		2) -steamlocal (runs Steam in local filesystem mode (all content off HDD)
	if ( CommandLine()->FindParm( "-steam" ) || CommandLine()->FindParm( "-steamlocal" ) || _access( pFileSystemDLL, 0 ) != 0 )
	{
		Q_snprintf( pFileSystemDLL, nMaxLen, "%s%cfilesystem_steam.dll", executablePath, CORRECT_PATH_SEPARATOR );
		bSteam = true;
	}
#elif _LINUX
	Q_snprintf( pFileSystemDLL, nMaxLen, "%s%cfilesystem_i486.so", executablePath, CORRECT_PATH_SEPARATOR );
#else
	#error "define a filesystem dll name"
#endif

	return FS_OK;
}


//-----------------------------------------------------------------------------
// Loads the file system module
//-----------------------------------------------------------------------------
FSReturnCode_t FileSystem_LoadFileSystemModule( CFSLoadModuleInfo &fsInfo )
{
	// First, locate the directory with gameinfo.txt.
	FSReturnCode_t ret = LocateGameInfoFile( fsInfo, fsInfo.m_GameInfoPath, sizeof( fsInfo.m_GameInfoPath ) );
	if ( ret != FS_OK )
		return ret;

	CSteamEnvVars steamEnvVars;
	if ( fsInfo.m_bSteam && fsInfo.m_bToolsMode )
	{
		// Now, load gameinfo.txt (to make sure it's there)
		KeyValues *pMainFile, *pFileSystemInfo, *pSearchPaths;
		ret = LoadGameInfoFile( fsInfo.m_GameInfoPath, pMainFile, pFileSystemInfo, pSearchPaths );
		if ( ret != FS_OK )
			return ret;

		// If filesystem_stdio.dll is missing or -steam is specified, then load filesystem_steam.dll.
		// There are two command line parameters for Steam:
		//		1) -steam (runs Steam in remote filesystem mode; requires Steam backend)
		//		2) -steamlocal (runs Steam in local filesystem mode (all content off HDD)

		// Setup all the environment variables related to Steam so filesystem_steam.dll knows how to initialize Steam.
		ret = SetupSteamStartupEnvironment( pFileSystemInfo, fsInfo.m_GameInfoPath, steamEnvVars );

		// We're done with main file
		pMainFile->deleteThis();
	}

	// Now that the environment is setup, load the filesystem module.
	if ( !Sys_LoadInterface(
		fsInfo.m_pFileSystemDLLName,
		FILESYSTEM_INTERFACE_VERSION,
		&fsInfo.m_pModule,
		(void**)&fsInfo.m_pFileSystem ) )
	{
		return SetupFileSystemError( FS_UNABLE_TO_INIT, "Can't load %s.", fsInfo.m_pFileSystemDLLName );
	}

	if ( !fsInfo.m_pFileSystem->Connect( fsInfo.m_ConnectFactory ) )
		return SetupFileSystemError( FS_UNABLE_TO_INIT, "%s IFileSystem::Connect failed.", fsInfo.m_pFileSystemDLLName );

	if ( fsInfo.m_pFileSystem->Init() != INIT_OK )
		return SetupFileSystemError( FS_UNABLE_TO_INIT, "%s IFileSystem::Init failed.", fsInfo.m_pFileSystemDLLName );

	return FS_OK;
}


//-----------------------------------------------------------------------------
// Mounds a particular steam cache
//-----------------------------------------------------------------------------
FSReturnCode_t FileSystem_MountContent( CFSMountContentInfo &mountContentInfo )
{
	// This part is Steam-only.
	if ( mountContentInfo.m_pFileSystem->IsSteam() )
	{
		// Find out the "extra app id". This is for tools, which want to mount a base app's filesystem
		// like HL2, then mount the SDK content (tools materials and models, etc) in addition.
		int nExtraAppId = -1;
		if ( mountContentInfo.m_bToolsMode )
		{
			FSReturnCode_t ret = GetSteamExtraAppId( mountContentInfo.m_pDirectoryName, &nExtraAppId );
			if ( ret != FS_OK )
				return ret;
		}

		// Set our working directory temporarily so Steam can remember it.
		// This is what Steam strips off absolute filenames like c:\program files\valve\steam\steamapps\username\sourcesdk
		// to get to the relative part of the path.
		char baseDir[MAX_PATH], oldWorkingDir[MAX_PATH];
		if ( !FileSystem_GetBaseDir( baseDir, sizeof( baseDir ) ) )
			return SetupFileSystemError( FS_INVALID_PARAMETERS, "FileSystem_GetBaseDir failed." );

		Q_getwd( oldWorkingDir, sizeof( oldWorkingDir ) );
		_chdir( baseDir );

		// Filesystem_tools needs to add dependencies in here beforehand.
		FilesystemMountRetval_t retVal = mountContentInfo.m_pFileSystem->MountSteamContent( nExtraAppId );
		
		_chdir( oldWorkingDir );

		if ( retVal != FILESYSTEM_MOUNT_OK )
			return SetupFileSystemError( FS_UNABLE_TO_INIT, "Unable to mount Steam content in the file system" );
	}

	return FileSystem_SetBasePaths( mountContentInfo.m_pFileSystem );
}


void FileSystem_SetHandleErrorsAutomatically( bool bAuto )
{
	g_bHandleErrorsAutomatically = bAuto;
}


