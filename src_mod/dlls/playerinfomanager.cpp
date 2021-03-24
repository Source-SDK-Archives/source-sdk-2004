//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: implementation of player info manager
//
//=============================================================================//

#include "cbase.h"
#include "player.h"
#include "playerinfomanager.h"

static CPlayerInfoManager s_PlayerInfoManager;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CPlayerInfoManager, IPlayerInfoManager, INTERFACEVERSION_PLAYERINFOMANAGER, s_PlayerInfoManager);

IPlayerInfo *CPlayerInfoManager::GetPlayerInfo( edict_t *pEdict )
{
	CBasePlayer *pPlayer = ( ( CBasePlayer * )CBaseEntity::Instance( pEdict ));
	if ( pPlayer )
	{
		return pPlayer->GetPlayerInfo();
	}
	else
	{
		return NULL;
	}
}

