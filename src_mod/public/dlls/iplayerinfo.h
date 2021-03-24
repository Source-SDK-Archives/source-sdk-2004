//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: provides an interface for dlls to query information about players from the game dll
//
//=============================================================================//
#ifndef IPLAYERINFO_H
#define IPLAYERINFO_H
#ifdef _WIN32
#pragma once
#endif


class IPlayerInfo
{
public:
	// returns the players name (UTF-8 encoded)
	virtual const char *GetName() = 0;
	// returns the userid (slot number)
	virtual int		GetUserID() = 0;
	// returns the string of their network (i.e Steam) ID
	virtual const char *GetNetworkIDString() = 0;
	// returns the team the player is on
	virtual int GetTeamIndex() = 0;
	// changes the player to a new team (if the game dll logic allows it)
	virtual void ChangeTeam( int iTeamNum ) = 0;
	// returns the number of kills this player has (exact meaning is mod dependent)
	virtual int	GetFragCount() = 0;
	// returns the number of deaths this player has (exact meaning is mod dependent)
	virtual int	GetDeathCount() = 0;
	// returns if this player slot is actually valid
	virtual bool IsConnected() = 0;
	// returns the armor/health of the player (exact meaning is mod dependent)
	virtual int	GetArmorValue() = 0;
};


#define INTERFACEVERSION_PLAYERINFOMANAGER			"PlayerInfoManager001"
class IPlayerInfoManager
{
public:
	virtual IPlayerInfo *GetPlayerInfo( edict_t *pEdict ) = 0;
};


#endif // IPLAYERINFO_H
