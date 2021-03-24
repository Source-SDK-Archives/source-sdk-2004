//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include <vgui/ISystem.h>
#include <vgui_controls/Controls.h>

#define STEAM_OFFLINE_MODE "HKEY_CURRENT_USER\\Software\\Valve\\Steam\\Offline"
#define OFFLINE_FILE "Steam\\cached\\offline_" // first part of filename

inline bool IsSteamInOfflineMode()
{
	int offline = 0;
	vgui::system()->GetRegistryInteger( STEAM_OFFLINE_MODE, offline );
	return ( offline == 1 );
}