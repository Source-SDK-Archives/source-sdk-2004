//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//
#if !defined( IGAMEEVENTS_H )
#define IGAMEEVENTS_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

#define INTERFACEVERSION_GAMEEVENTSMANAGER	"GAMEEVENTSMANAGER001"

//-----------------------------------------------------------------------------
// Purpose: Engine interface into global game event management
//-----------------------------------------------------------------------------

/* 

The GameEventManager keeps track and fires of all global game events. Game events 
are fired by game.dll for events like player death or team wins. Each event has a
unique name and comes with a KeyValue structure providing informations about this
event. Some events are generated also by the engine.

Events are networked to connected clients and invoked there to. Therefore you
have to specify all data fields and there data types in an public resource
file which is parsed by server and client. A typical game event is defined 
like this:

	"game_start"				// a new game starts
	{
		"roundslimit"	"long"		// max round
		"timelimit"		"long"		// time limit
		"fraglimit"		"long"		// frag limit
		"objective"		"string"	// round objective
	}

All events must have unique names (case sensitive) and may have a list
of data fields. each data field must specify a data type, so the engine
knows how to serialize/unserialize that event for network transmission.
Valid data types are string, float, long, short, byte & bool. If a 
data field should not be broadcasted to clients, use the type "local".

It's possible to load event definitions from multiple files and 
to extent already existing events. A MOD may extent the "game_start"
event by a new data field just by adding it:

	"game_start"				// a new game starts
	{
		"capturelimit"	"long"		// flag capture limit
	}

It's no possible to remove or change existing data fields. Some
data fields exists by default and control event firing: 
		
		"local"			"1"	// fired only on sider side, not networked
		"suppress"		"1"	// supresses this event
		"unreliable"	"1"	// send event as unreliable message
*/


#define MAX_EVENT_NAME_LENGTH	32			// max game event name length
#define MAX_EVENT_NUMBER		512		// max number of events allowed

class KeyValues;
class bf_read;
class bf_write;
class CGameEvent;

class IGameEventListener
{
public:
	virtual	~IGameEventListener( void ) {};
	
	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent( KeyValues * event) = 0;
};

class IGameEventManager : public IBaseInterface
{
public:
	virtual	~IGameEventManager( void ) {};
	
	// load game event descriptions from a file eg "resource\gameevents.res"
	virtual int LoadEventsFromFile( const char * filename ) = 0;

	// removes all and anything
	virtual void  Reset() = 0;	
				
	virtual KeyValues *GetEvent(const char * name) = 0; // returns keys for event
	
	// adds a listener for a particular event
	virtual bool AddListener( IGameEventListener * listener, const char * event, bool bIsServerSide ) = 0;
	// registers for all known events
	virtual bool AddListener( IGameEventListener * listener, bool bIsServerSide ) = 0; 
	
	// removes a listener 
	virtual void RemoveListener( IGameEventListener * listener) = 0;
		
	// fires an global event, specific event data is stored in KeyValues
	// local listeners will receive the event instantly
	// a network message will be send to all connected clients to invoke
	// the same event there
	virtual bool FireEvent( KeyValues * event ) = 0;

	// fire a side server event, that it wont be broadcasted to player clients
	virtual bool FireEventServerOnly( KeyValues * event ) = 0;

	// fires an event only on this local client
	// can be used to fake events coming over the network 
	virtual bool FireEventClientOnly( KeyValues * event ) = 0;

	// write/read event to/from bitbuffer
	virtual bool SerializeKeyValues( KeyValues *event, bf_write *buf, CGameEvent *eventtype = NULL ) = 0;
	virtual KeyValues *UnserializeKeyValue( bf_read *msg ) = 0; // create new KeyValues, must be deleted
};

#endif // IGAMEEVENTS_H