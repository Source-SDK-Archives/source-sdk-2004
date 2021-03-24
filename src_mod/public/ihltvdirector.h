//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef IHLTVDIRECTOR_H
#define IHLTVDIRECTOR_H
#ifdef _WIN32
#pragma once
#endif

class IHLTVServer;
class KeyValues;
class Vector;

#define INTERFACEVERSION_HLTVDIRECTOR			"HLTVDirector001"

class IHLTVDirector
{
public:
	virtual	~IHLTVDirector() {}

	virtual void	SetHLTVServer( IHLTVServer *hltv ) = 0; // give the director the engine HLTV interface 
	virtual IHLTVServer* GetHLTVServer( void ) = 0; // get current HLTV server interface
	
	virtual int		GetDirectorTick( void ) = 0;	// get current broadcast tick from director
	virtual int		GetPVSEntity( void ) = 0; // get current view entity (PVS), 0 if coords are used
	virtual Vector	GetPVSOrigin( void ) = 0; // get current PVS origin
	virtual bool	GetDirectorCommands( int fromTick, int toTick, bf_write &buffer) = 0;
	virtual void	AddDirectorCommand( int tick, KeyValues *data) = 0;
	virtual float	GetDelay( void ) = 0; // returns current delay in seconds
	virtual bool	IsActive( void ) = 0; // true if director is active
};

#endif // IHLTVDIRECTOR_H
