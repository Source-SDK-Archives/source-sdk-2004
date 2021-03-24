//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// hltvdirector.cpp: implementation of the CHLTVDirector class.
//
//////////////////////////////////////////////////////////////////////

#include "cbase.h"
#include "hltvdirector.h"
#include "KeyValues.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static CHLTVDirector s_HLTVDirector;	// singelton

static ConVar hltv_delay( "hltv_delay", "10", 0, "HLTV broadcast delay in seconds" );
static ConVar hltv_viewent( "hltv_viewent", "0", 0, "HLTV camera entity index" );

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CHLTVDirector, IHLTVDirector, INTERFACEVERSION_HLTVDIRECTOR, s_HLTVDirector );

static bool DirectorCommandLessFunc( CDirectorCommand const &cmd1, CDirectorCommand const &cmd2 )
{
	return cmd1.m_Tick < cmd2.m_Tick;
}

static float WeightedAngle( Vector vec1, Vector vec2)
{
	VectorNormalize( vec1 );
	VectorNormalize( vec2 );

	float a = DotProduct( vec1, vec2 ); // a = [-1,1]

	a = (a + 1.0f) / 2.0f;

	Assert ( a <= 1 && a >= 0 );
		
	return a*a;	// vectors are facing opposite direction
}

IGameSystem* HLTVDirectorSystem()
{
	return &s_HLTVDirector;
}

IHLTVDirector* HLTVDirector()
{
	return &s_HLTVDirector;
}

CHLTVDirector::CHLTVDirector()
{
	m_iPVSEntity = 1;
	m_fDelay = 30.0;
	m_iLastPlayer = 1;
	m_pHLTVServer = NULL;
	m_pHLTVClient = NULL;
	m_iCameraMan = 0;
	m_nNumFixedCameras = 0;
	m_DirectorCommands.SetLessFunc( DirectorCommandLessFunc );
	m_nNextAnalyzeCameraTick = 0.0f;
}

CHLTVDirector::~CHLTVDirector()
{

}

bool CHLTVDirector::Init()
{
	gameeventmanager->LoadEventsFromFile( "resource/hltvevents.res" );

	return gameeventmanager->AddListener( this, true );
}

void CHLTVDirector::Shutdown()
{
	RemoveCommands(-1); // all

	gameeventmanager->RemoveListener( this );
}

void CHLTVDirector::FireGameEvent( KeyValues * event )
{
	if ( !m_pHLTVServer )
		return;	// don't do anything
	
	AddDirectorCommand( gpGlobals->tickcount, event );
}

IHLTVServer* CHLTVDirector::GetHLTVServer( void )
{
	return m_pHLTVServer;
}

void CHLTVDirector::SetHLTVServer( IHLTVServer *hltv )
{
	RemoveCommands(-1); // all

	if ( hltv ) 
	{
		m_pHLTVClient = UTIL_PlayerByIndex( hltv->GetHLTVSlot() + 1 );

		if ( m_pHLTVClient && m_pHLTVClient->IsHLTV() )
		{
			m_pHLTVServer = hltv;
		}
		else
		{
			m_pHLTVServer  = NULL;
			Error( "Couldn't find HLTV client player." );
		}
	}
	else
	{
		// deaktivate HLTV director
		m_pHLTVServer = NULL;
	}
}

bool CHLTVDirector::IsActive( void )
{
	return (m_pHLTVServer != NULL );
}

float CHLTVDirector::GetDelay( void )
{
	return m_fDelay;
}

int	CHLTVDirector::GetDirectorTick( void )
{
	// just simple delay it
	return gpGlobals->tickcount - TIME_TO_TICKS( m_fDelay );
}

int	CHLTVDirector::GetPVSEntity( void )
{
	return m_iPVSEntity;
}

Vector CHLTVDirector::GetPVSOrigin( void )
{
	return m_vPVSOrigin;
}

bool CHLTVDirector::GetDirectorCommands( int fromTick, int toTick, bf_write &buffer)
{
	return false;
}

void CHLTVDirector::AddDirectorCommand( int tick, KeyValues *data)
{
	CDirectorCommand cmd;

	cmd.m_Tick = tick;
	if ( data )
	{
		cmd.m_Keys = data->MakeCopy();
	}
	else
	{
		cmd.m_Keys = NULL;
	}
	
	m_DirectorCommands.Insert( cmd );
}

void CHLTVDirector::UpdateSettings()
{
	// set delay
	m_fDelay = hltv_delay.GetFloat();
	m_fDelay = clamp( m_fDelay, HLTV_MIN_DELAY, HLTV_MAX_DELAY );

	m_nBroadcastTick = gpGlobals->tickcount - TIME_TO_TICKS( m_fDelay );
	m_nBroadcastTick = max( 0, m_nBroadcastTick );



	//set cameraman
	// m_pViewEntity = UTIL_PlayerByIndex( hltv_viewent.GetInt() );
}

// this is called with every new map 
void CHLTVDirector::LevelInitPostEntity( void )
{
	m_nNumFixedCameras = 0;
	
	CBaseEntity *pCamera = gEntList.FindEntityByClassname( NULL, "point_viewcontrol" );

	while ( pCamera && m_nNumFixedCameras < MAX_NUM_CAMERAS)
	{
		CBaseEntity *pTarget = gEntList.FindEntityByName( NULL, STRING(pCamera->m_target), pCamera );

		m_pFixedCameras[m_nNumFixedCameras] = pCamera;
		m_pCameraTargets[m_nNumFixedCameras] = pTarget; 

		m_nNumFixedCameras++;
		pCamera = gEntList.FindEntityByClassname( pCamera, "point_viewcontrol" );
	}

	m_vPVSOrigin.Init();
	m_iPVSEntity = 1;
	m_nNextShotTick = 0;
	m_nNextAnalyzeCameraTick = 0.0f;

	// DevMsg("HLTV Director: found %i fixed cameras.\n", m_nNumFixedCameras );
}

void CHLTVDirector::FrameUpdatePostEntityThink( void )
{
	if ( !m_pHLTVServer )
		return;	// don't do anything

	UpdateSettings();	// update settings from cvars

	if ( m_nNextAnalyzeCameraTick < gpGlobals->tickcount )
	{
		m_nNextAnalyzeCameraTick = gpGlobals->tickcount + TIME_TO_TICKS( 0.5f );

		BuildActivePlayerList();

		AnalyzePlayers();

		AnalyzeCameras();
	}

	// This function is called each tick
	if ( m_nNextShotTick == m_nBroadcastTick )
	{
		StartNewShot();		
	}

	

	// do something clever here each frame
}

void CHLTVDirector::StartPlayerShotChase1(int iPlayer, int offset, int distance, int fov, int theta, int phi, int inertia)
{
	m_iPVSEntity = iPlayer; // use this entity as center of PVS

	KeyValues * cmd = new KeyValues( "hltv_chase1" );
	cmd->SetInt("target", iPlayer );
	cmd->SetInt("offset", offset );
	cmd->SetInt("distance", distance );
	cmd->SetInt("fov", fov );
	cmd->SetInt("theta", theta );
	cmd->SetInt("phi", phi );
	cmd->SetInt("inertia", inertia );
		
	// send spectators the HLTV director command as a game event
	m_pHLTVServer->FireDirectorCommand( cmd );

	cmd->deleteThis();
}

void CHLTVDirector::StartPlayerShotChase2(int iTraget1, int iTraget2 )
{
	m_iPVSEntity = iTraget1; // use this entity as center of PVS

	KeyValues * cmd = new KeyValues( "hltv_chase2" );
	cmd->SetInt("target1", iTraget1 );
	cmd->SetInt("target2", iTraget2 );
			
	// send spectators the HLTV director command as a game event
	m_pHLTVServer->FireDirectorCommand( cmd );

	cmd->deleteThis();
}

void CHLTVDirector::StartFixedCameraShot(int iCamera, int iTarget)
{
	CBaseEntity *pCamera = m_pFixedCameras[iCamera];
	CBaseEntity *pTarget = m_pCameraTargets[iCamera];

	Vector vCamPos = pCamera->GetAbsOrigin();
	Vector vTargetPos = pTarget->GetAbsOrigin();
	QAngle aViewAngle;

	m_iPVSEntity = 0;	// don't use camera entity, since it may not been transmitted
	m_vPVSOrigin = vCamPos;

	VectorAngles( vTargetPos-vCamPos, aViewAngle );

	KeyValues * cmd = new KeyValues( "hltv_fixed" );
	cmd->SetInt("posx", vCamPos.x );
	cmd->SetInt("posy", vCamPos.y );
	cmd->SetInt("posz", vCamPos.z );
	cmd->SetInt("anglea", aViewAngle.x );
	cmd->SetInt("angleb", aViewAngle.y );

	if ( iTarget > 0 )
	{
		cmd->SetInt("target", iTarget );
	}
	else
	{
		cmd->SetInt("target", 0 );
	}
	
	// send spectators the HLTV director command as a game event
	m_pHLTVServer->FireDirectorCommand( cmd );

	cmd->deleteThis();
}

void CHLTVDirector::StartBestFixedCameraShot()
{
	float	flCameraRanking[MAX_NUM_CAMERAS];

	if ( m_nNumFixedCameras <= 0 )
		return;

	memset( flCameraRanking, 0, sizeof(flCameraRanking) );

	int firstIndex = FindFirstCommand( m_nBroadcastTick );

	int index = firstIndex;

	float flBestRank = -1.0f;
	int iBestCamera = -1;
	int iBestTarget = -1;

	// sum all ranking values for the cameras

	while( index != m_DirectorCommands.InvalidIndex() )
	{
		CDirectorCommand &dc = m_DirectorCommands[index];

		if ( dc.m_Tick >= m_nNextShotTick )
			break; 

		if ( Q_strcmp( dc.m_Keys->GetName(), "rank_camera") == 0 )
		{
			int index = dc.m_Keys->GetInt("index"); 
			flCameraRanking[index] += dc.m_Keys->GetFloat("rank" );

			// find best camera
			if ( flCameraRanking[index] > flBestRank )
			{
				iBestCamera = index;
				flBestRank = flCameraRanking[index];
				iBestTarget = dc.m_Keys->GetInt("target"); 
			}
		}

		index = m_DirectorCommands.NextInorder( index );
	}

	if ( iBestCamera == -1 )
	{
		//choose random camera with fixed view
		iBestCamera = RandomInt(0,m_nNumFixedCameras-1);
	}

	StartFixedCameraShot( iBestCamera, iBestTarget );
}

void CHLTVDirector::StartRandomShot() 
{
	int toTick = m_nBroadcastTick + TIME_TO_TICKS ( DEF_SHOT_LENGTH + RandomFloat(-1.0f, 1.0f) );

	// duartion is ok, set new m_nNextShotTick, make sure it's not bigger
	m_nNextShotTick = min( m_nNextShotTick, toTick );
	
	if ( RandomInt(0,1)	)
	{
		if ( m_nNumActivePlayers > 0 )
		{
			StartPlayerShotChase1
			( 
				m_pActivePlayers[ RandomInt(0,m_nNumActivePlayers-1) ]->entindex(),
				64,
				96,
				90,
				10,
				0,
				100
			);
			return;
		}
	}

	StartBestFixedCameraShot();
}

bool CHLTVDirector::IsKnownGameEvent( const char *name )
{
	char * gameEventNames[] = 
	{	"player_use",
		"player_shoot",
		"player_hurt",
		"player_death"	};

	for( int i=0; i < ARRAYSIZE(gameEventNames); i++ )
	{
		if ( Q_strcmp(gameEventNames[i], name ) == 0 )
			return true;
	}

	return false;
}

void CHLTVDirector::StartGameEventShot( CDirectorCommand *dc )
{
	m_nNextShotTick = min( m_nNextShotTick, (dc->m_Tick+TIME_TO_TICKS(2.0)) );

	const char * name = dc->m_Keys->GetName();

	if ( !Q_strcmp( "player_death", name ) ||
		 !Q_strcmp( "player_hurt", name ) )
	{
		CBasePlayer *target = UTIL_PlayerByUserId( dc->m_Keys->GetInt("userid") );
		CBasePlayer *killer = UTIL_PlayerByUserId( dc->m_Keys->GetInt("attacker") );

		if ( target && killer )
		{
			StartPlayerShotChase2( target->entindex(), killer->entindex() );
		}
		else if ( target )
		{
			StartPlayerShotChase1
			( 
				target->entindex(),
				54,
				64,
				60,
				20,
				180,
				10
			);
		}
		else
		{
			DevMsg("DrcCmd: couldn't find targets for %s\n", name );
		}
	}
		
	DevMsg("DrcCmd: %s\n", name );
}

void CHLTVDirector::RemoveCommands(int tick)
{
	int index = m_DirectorCommands.FirstInorder();

	while ( index != m_DirectorCommands.InvalidIndex() )
	{
		CDirectorCommand &dc = m_DirectorCommands[index];

		int orgIndex = index;

		index = m_DirectorCommands.NextInorder( index );

		if ( (dc.m_Tick < tick) || (tick == -1) )
		{
			dc.m_Keys->deleteThis(); 
			dc.m_Keys = NULL;
			m_DirectorCommands.RemoveAt( orgIndex );
		}
	}
}

int CHLTVDirector::FindFirstCommand( int tick )
{
	// TODO cache last queried ticks

	int index = m_DirectorCommands.FirstInorder();

	if ( index == m_DirectorCommands.InvalidIndex() )
		return index; // no commands in list

	CDirectorCommand dc = m_DirectorCommands[index];

	while ( dc.m_Tick < tick )
	{
		index = m_DirectorCommands.NextInorder( index );

		if ( index == m_DirectorCommands.InvalidIndex() )
			break;

		dc = m_DirectorCommands[index];
	}

	return index;
}

void CHLTVDirector::FinishCameraManShot()
{
	Assert( m_iCameraMan == m_iPVSEntity );

	// check next frame again if we don't find any commands in buffer( should never happen)
	m_nNextShotTick = m_nBroadcastTick+1;

	int index = FindFirstCommand( m_nBroadcastTick );

	//check if camera turns camera off within broadcast time and game time
	while( index != m_DirectorCommands.InvalidIndex() )
	{
		CDirectorCommand &dc = m_DirectorCommands[index];

		m_nNextShotTick = dc.m_Tick+1; 

		if ( Q_strcmp( dc.m_Keys->GetName(), "hltv_cameraman") == 0 )
		{
			if ( dc.m_Keys->GetInt("mode") == 0 &&
				 dc.m_Keys->GetInt("index") == m_iCameraMan )
			{
				// current camera man swicth camera off
				m_iCameraMan = 0;
				return;
			}
		}

		index = m_DirectorCommands.NextInorder( index );
	}

	// cameraman ist still recording, don't change anything
}


bool CHLTVDirector::StartCameraManShot()
{
	int index = FindFirstCommand( m_nBroadcastTick );

	// check for cameraman mode
	while( index != m_DirectorCommands.InvalidIndex() )
	{
		CDirectorCommand &dc = m_DirectorCommands[index];

		// only check if this is the current tick
		if ( dc.m_Tick > m_nBroadcastTick )
			break; 

		if ( Q_strcmp( dc.m_Keys->GetName(), "hltv_cameraman") == 0 )
		{
			if ( dc.m_Keys->GetInt("mode") > 0 )
			{
				// ok, this guy is now the active camera man
				m_iCameraMan = dc.m_Keys->GetInt("index");
				m_iPVSEntity = m_iCameraMan;
				m_nNextShotTick = m_nBroadcastTick+1; // check setting right on next frame

				// send camera man command to client
				m_pHLTVServer->FireDirectorCommand( dc.m_Keys );

				return true;
			}
		}

		index = m_DirectorCommands.NextInorder( index );
	}

	return false;	// no camera man found
}

void CHLTVDirector::StartNewShot()
{
	Assert( m_nBroadcastTick == m_nNextShotTick );

	// we can remove all commands up to now
	RemoveCommands( m_nBroadcastTick );

	if ( m_iCameraMan > 0 )
	{
		// we already have an active camera man,
		// wait until he releases the "record" lock
		FinishCameraManShot();
		return;	
	}

	if ( StartCameraManShot() )
	{
		// now we have an active camera man
		return;
	}

	// ok, no camera man active, now check how much time
	// we have for the next shot, if the time diff to the next
	// important event we have to switch to is too short (<2sec)
	// just extent the current shot and don't start a new one

	// check the next 8 seconds for interrupts/important events
	m_nNextShotTick = m_nBroadcastTick + TIME_TO_TICKS( MAX_SHOT_LENGTH );

	int index = FindFirstCommand( m_nBroadcastTick );

	while( index != m_DirectorCommands.InvalidIndex() )
	{
		CDirectorCommand &dc = m_DirectorCommands[index];

		if ( dc.m_Tick >= m_nNextShotTick )
			break; // we have searched enough

		// a camera man is always interrupting auto director
		if ( Q_strcmp( dc.m_Keys->GetName(), "hltv_cameraman") == 0 )
		{
			if ( dc.m_Keys->GetInt("mode") > 0 )
			{
				// stop the next cut when this cameraman starts recording
				m_nNextShotTick = dc.m_Tick;
				break;
			}
		}

		index = m_DirectorCommands.NextInorder( index );
	}

	float flDuration = TICKS_TO_TIME(m_nNextShotTick - m_nBroadcastTick);

	if ( flDuration < MIN_SHOT_LENGTH )
		return;	// not enough time for a new shot

	CDirectorCommand *dc = FindBestGameEvent();

	if ( dc )
	{
		// show the game event
		StartGameEventShot( dc );
	}
	else
	{
		// no commands found, start random shot
		StartRandomShot();
	}
}

CDirectorCommand *CHLTVDirector::FindBestGameEvent()
{
	int	bestEvent[4];
	int	bestEventPrio[4];

	Q_memset( bestEvent, 0, sizeof(bestEvent) );
	Q_memset( bestEventPrio, 0, sizeof(bestEventPrio) );
	
	int index = FindFirstCommand( m_nBroadcastTick );
		
	// search for next 4 best events
	for (int i = 0; i<4; i ++)
	{
		bestEventPrio[i] = 0;
		bestEvent[i] = 0;
	
		int tillTick = m_nBroadcastTick + TIME_TO_TICKS( 2.0f*(1.0f+i) );

		if ( tillTick > m_nNextShotTick )
			break;

		// sum all action for the next time
		while ( index != m_DirectorCommands.InvalidIndex()  )
		{
			CDirectorCommand &dc = m_DirectorCommands[index];

			if ( dc.m_Tick > tillTick )
				break;

			// only check events that are known to the director
			if ( IsKnownGameEvent( dc.m_Keys->GetName() ) )
			{
				int priority = dc.m_Keys->GetInt("priority");

				if ( priority > bestEventPrio[i] )
				{
					bestEvent[i] = index;
					bestEventPrio[i] = priority;
				}
			}

			index = m_DirectorCommands.NextInorder( index );
		}
	}

	if ( !( bestEventPrio[0] || bestEventPrio[1] || bestEventPrio[2] ) )
		return NULL; // no event found at all, give generic algorith a chance

	// camera cut rules :

	if ( bestEventPrio[1] >= bestEventPrio[0] &&
		 bestEventPrio[1] >= bestEventPrio[2] &&
		 bestEventPrio[1] >= bestEventPrio[3] )
	{
		return &m_DirectorCommands[ bestEvent[1] ];	// best case
	}
	else if ( bestEventPrio[0] > bestEventPrio[1] &&
			  bestEventPrio[0] > bestEventPrio[2] )
	{
		return &m_DirectorCommands[ bestEvent[0] ];	// event 0 is very important
	}
	else if (	bestEventPrio[2] > bestEventPrio[3] ) 
	{
		return &m_DirectorCommands[ bestEvent[2] ];	
	}
	else
	{
		// event 4 ist the best but too far away, so show event 1 
		if ( bestEvent[0] )
			return &m_DirectorCommands[ bestEvent[0] ];
		else
			return NULL;
	}
}

void CHLTVDirector::AnalyzeCameras()
{
	for ( int i = 0; i<m_nNumFixedCameras; i++ )
	{
		CBaseEntity *pCamera = m_pFixedCameras[ i ];

		float	flRank = 0.0f;
		int		iClosestPlayer = 0;
		float	flClosestPlayerDist = 100000.0f;
		int		nCount = 0; // Number of visible targets
		Vector	vDistribution; vDistribution.Init(); // distribution of targets
		
		Vector vCamPos = pCamera->GetAbsOrigin();

		for ( int j=0; j<m_nNumActivePlayers; j++ )
		{
			CBasePlayer *pPlayer = m_pActivePlayers[j];

			Vector vPlayerPos = pPlayer->GetAbsOrigin();

			float dist = VectorLength( vPlayerPos - vCamPos );

			if ( dist > 1024.0f || dist < 4.0f )
				continue;	// too colse or far away

			// check visibility
			trace_t tr;
			UTIL_TraceLine( vCamPos, pPlayer->GetAbsOrigin(), MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr  );

			if ( tr.fraction < 1.0 )
				continue;	// not visible for camera

			nCount++;

			// remember closest player
			if ( dist <  flClosestPlayerDist )
			{
				iClosestPlayer = pPlayer->entindex();
				flClosestPlayerDist = dist;
			}

			Vector v1; AngleVectors( pPlayer->EyeAngles(), &v1 );

			// check players orientation towards camera
			Vector v2 = vCamPos - vPlayerPos;
			VectorNormalize( v2 );

			// player/camera cost function:
			flRank += ( 1.0f/sqrt(dist) ) * WeightedAngle( v1, v2 );

			vDistribution += v2;
		}

		if ( nCount == 0 )
			continue; // camera doesn't see anybody

		float flDistribution =  VectorLength( vDistribution ) / nCount; // normalize distribution

		flRank *= flDistribution;

		KeyValues *event = new KeyValues( "rank_camera" );
		event->SetFloat("rank", flRank );
		event->SetInt("index",  i ); // index in m_pFixedCameras
		event->SetInt("target",  iClosestPlayer ); // ent index

		AddDirectorCommand( gpGlobals->tickcount, event );

		event->deleteThis();
	}
}

void CHLTVDirector::BuildActivePlayerList()
{
	// first build list of all active players

	m_nNumActivePlayers = 0;

	for ( int i =0; i < gpGlobals->maxClients; i++ )
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex( i+1 );

		if ( !pPlayer )
			continue;

		if ( !pPlayer->IsAlive() )
			continue;

		if ( pPlayer->IsObserver() )
			continue;

		m_pActivePlayers[m_nNumActivePlayers] = pPlayer;
		m_nNumActivePlayers++;
	}
}

void CHLTVDirector::AnalyzePlayers()
{
	// analyzes every active player

	for ( int i = 0; i<m_nNumActivePlayers; i++ )
	{
		CBasePlayer *pCamera = m_pActivePlayers[ i ];

		float	flRank = 0.0f;
		int		iBestFacingPlayer = 0;
		float	flBestFacingPlayer = 0.0f;
		int		nCount = 0; // Number of visible targets
		Vector	vDistribution; vDistribution.Init(); // distribution of targets
		
		Vector vCamPos = pCamera->GetAbsOrigin();

		Vector v1; AngleVectors( pCamera->EyeAngles(), &v1 );

		v1 *= -1; // inverted

		for ( int j=0; j<m_nNumActivePlayers; j++ )
		{
			if ( i == j )
				continue;  // don't check against itself

			CBasePlayer *pPlayer = m_pActivePlayers[j];

			Vector vPlayerPos = pPlayer->GetAbsOrigin();

			float dist = VectorLength( vPlayerPos - vCamPos );

			if ( dist > 1024.0f || dist < 4.0f )
				continue;	// too close or far away

			// check visibility
			trace_t tr;
			UTIL_TraceLine( vCamPos, pPlayer->GetAbsOrigin(), MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr  );

			if ( tr.fraction < 1.0 )
				continue;	// not visible for camera

			nCount++;

			// check players orientation towards camera
			Vector v2; AngleVectors( pPlayer->EyeAngles(), &v2 );

			float facing = WeightedAngle( v1, v2 );

			// remember closest player
			if ( facing > flBestFacingPlayer )
			{
				iBestFacingPlayer = pPlayer->entindex();
				flBestFacingPlayer = facing;
			}

			// player/camera cost function:
			flRank += ( 1.0f/sqrt(dist) ) * facing;

			vDistribution += v2;
		}

		if ( nCount == 0 )
			continue; // camera doesn't see anybody

		float flDistribution =  VectorLength( vDistribution ) / nCount; // normalize distribution

		flRank *= flDistribution;

		KeyValues *event = new KeyValues( "rank_player" );
		event->SetFloat("rank", flRank );
		event->SetInt("index",  pCamera->entindex() );
		event->SetInt("target",  iBestFacingPlayer ); // ent index

		AddDirectorCommand( gpGlobals->tickcount, event );

		event->deleteThis();
	}
}

