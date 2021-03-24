//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HLTVDIRECTOR_H
#define HLTVDIRECTOR_H

#ifdef _WIN32
#pragma once
#endif

#include <igameevents.h>
#include <igamesystem.h>
#include <ihltvdirector.h>
#include <ihltv.h>
#include <utlrbtree.h>

#define	HLTV_MIN_DELAY				0	// minimum delay
#define	HLTV_MIN_DIRECTOR_DELAY		10	// minimum delay if director is enabled
#define	HLTV_MAX_DELAY				90	// maximum delay


#define MAX_NUM_CAMERAS				64	// support up to 64 fixed cameras per level

#define MIN_SHOT_LENGTH				2.0f  // minimum time of a cut (seconds)
#define MAX_SHOT_LENGTH				8.0f  // maximum time of a cut (seconds)
#define DEF_SHOT_LENGTH				4.0f  // average time of a cut (seconds)

class CDirectorCommand
{
public:
		int			m_Tick;		// tick of this command
		KeyValues	*m_Keys;	// KeyValue describing data types, if NULL only name 
};

class CHLTVDirector : public CBaseGameSystem, public IHLTVDirector, public IGameEventListener
{
public:
	CHLTVDirector();
	virtual ~CHLTVDirector();

	void	SetHLTVServer( IHLTVServer *hltv ); // give the director an HLTV interface 
	IHLTVServer* GetHLTVServer( void ); 
	int		GetDirectorTick( void );	// get current broadcast tick from director
	int		GetPVSEntity( void ); // get current view entity (PVS)
	Vector	GetPVSOrigin( void ); // get current PVS origin, if PVS entity is 0
	bool	GetDirectorCommands( int fromTick, int toTick, bf_write &buffer);
	void	AddDirectorCommand( int tick, KeyValues *data);
	float	GetDelay( void ); // returns current delay in seconds
	bool	IsActive( void );
		

public: // IGameEventListener Interface
	void FireGameEvent( KeyValues * event );
	
public: // CBaseGameSystem overrides

	bool Init();
	void Shutdown();
	void FrameUpdatePostEntityThink();
	void LevelInitPostEntity();

private:

	void	StartNewShot();	
	void	StartRandomShot();
	void	StartBestFixedCameraShot();
	void	StartFixedCameraShot(int iCamera, int iTarget);
	void	StartPlayerShotChase1(int iPlayer, int offset, int distance, int fov, int theta, int phi, int inertia);
	void	StartPlayerShotChase2(int iTraget1, int iTraget2 );
	void	UpdateSettings();
	void	AnalyzePlayers();
	void 	AnalyzeCameras();
	int		FindFirstCommand( int tick ); // finds first command >= tick
	void	RemoveCommands(int tick); // removes all commands < tick, or all if tick -1
	bool	StartCameraManShot();
	void	FinishCameraManShot();
	void	BuildActivePlayerList();
	CDirectorCommand *FindBestGameEvent();
	void	StartGameEventShot( CDirectorCommand *dc );
	bool	IsKnownGameEvent( const char *name );
		

	IHLTVServer		*m_pHLTVServer;	// interface to servers HLTV object
	float			m_fDelay;	// hltv delay in seconds
	int				m_nBroadcastTick; // world time that is currently "on the air"
	int				m_iPVSEntity;	// entity for PVS center
	Vector			m_vPVSOrigin;	// PVS origin if PVS entity is 0
	int				m_iCameraMan;	// bot NULL if current viewentity is a cameraman
	CBasePlayer		*m_pHLTVClient; // the HLTV fakeclient
	int				m_nNextShotTick;	// time for the next scene cut
	int				m_iLastPlayer;		// last player in random rotation

	int				m_nNextAnalyzeCameraTick;	
	int				m_nNextAnalyzePlayerTick;	
	


	int				m_nNumFixedCameras;	//number of cameras in current map
	CBaseEntity		*m_pFixedCameras[MAX_NUM_CAMERAS]; // fixed cameras (point_viewcontrol)
	CBaseEntity		*m_pCameraTargets[MAX_NUM_CAMERAS]; // targets for fixed cameras

	int				m_nNumActivePlayers;	//number of cameras in current map
	CBasePlayer		*m_pActivePlayers[MAX_PLAYERS]; // fixed cameras (point_viewcontrol)
	
	CUtlRBTree<CDirectorCommand,int>	m_DirectorCommands;
};

extern IGameSystem* HLTVDirectorSystem();
extern IHLTVDirector* HLTVDirector();

#endif // HLTVDIRECTOR_H
