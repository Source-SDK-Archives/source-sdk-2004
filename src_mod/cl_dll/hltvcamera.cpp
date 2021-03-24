//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hltvcamera.h"
#include "cdll_client_int.h"
#include "util_shared.h"
#include <KeyValues.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define CHASE_CAM_DISTANCE		96.0f
#define WALL_OFFSET				6.0f

static Vector WALL_MIN(-WALL_OFFSET,-WALL_OFFSET,-WALL_OFFSET);
static Vector WALL_MAX(WALL_OFFSET,WALL_OFFSET,WALL_OFFSET);

static ConVar hltv_cam_distance( "hltv_cam_distance", "96", 0, "Default HLTV chase camera distance" );
static ConVar hltv_cam_offset( "hltv_cam_offset", "64", 0, "Default HLTV chase camera offset" );
static ConVar hltv_cam_theta( "hltv_cam_theta", "0", 0, "Default HLTV chase camera theta angle" );
static ConVar hltv_cam_phi( "hltv_cam_phi", "0", 0, "Default HLTV chase camera phi angle" );
static ConVar hltv_cam_fov( "hltv_cam_fov", "90", 0, "Default HLTV chase camera FOV" );
static ConVar hltv_cam_inertia( "hltv_cam_inertia", "100", 0, "Default HLTV chase camera inertia" );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static C_HLTVCamera s_HLTVCamera;

C_HLTVCamera *HLTVCamera() { return &s_HLTVCamera; }

C_HLTVCamera::C_HLTVCamera()
{

}

C_HLTVCamera::~C_HLTVCamera()
{

}

void C_HLTVCamera::Init()
{
	gameeventmanager->AddListener( this, false );

	m_vCamOrigin.Init();
	m_aCamAngle.Init();

	Reset();
}

void C_HLTVCamera::Shutdown()
{
	gameeventmanager->RemoveListener( this );
}

void C_HLTVCamera::Reset()
{
	m_nCameraMode = OBS_MODE_FIXED;
	m_iCameraMan  = m_iTraget1 = m_iTraget2 = 0;
	m_flFOV = hltv_cam_fov.GetFloat();
	m_flDistance = m_flLastDistance = hltv_cam_distance.GetFloat();
	m_flInertia = hltv_cam_inertia.GetFloat();
	m_flPhi = hltv_cam_phi.GetFloat();
	m_flTheta = hltv_cam_theta.GetFloat();
	m_flOffset = hltv_cam_offset.GetFloat();
}

void C_HLTVCamera::CalcChaseCamView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{
	if ( m_iTraget1 && m_iTraget2 )
	{
		CalcChaseCamView2( eyeOrigin, eyeAngles, fov );
	}
	else if ( m_iTraget1 )
	{
		CalcChaseCamView1( eyeOrigin, eyeAngles, fov );
	}
}

void C_HLTVCamera::CalcChaseCamView1( Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{
	QAngle tmpangles;

	Vector forward;

	if ( m_iTraget1 == 0 )
		return;

	C_BaseEntity *target = ClientEntityList().GetBaseEntity( m_iTraget1 );

	if ( !target )
		return;

	Vector targetOrigin, cameraOrigin;
	QAngle targetAngles, cameraAngles;	

	targetAngles = target->EyeAngles();
	targetOrigin = target->GetAbsOrigin(); // use abs origin, not eye origin

	targetAngles.x = 0; // no PITCH
	targetAngles.z = 0; // no ROLL

	targetOrigin.z += m_flOffset; // add offset

	QAngle angleOffset( m_flTheta, m_flPhi, 0 );

	cameraAngles = targetAngles + angleOffset;

	InterpolateAngles( m_aCamAngle,	cameraAngles, cameraAngles, m_flInertia/100.0f );

	AngleVectors( cameraAngles, &forward );

	VectorNormalize( forward );

	if ( m_flLastDistance < m_flDistance )
	{
		// grow distance by 32 unit a second
		m_flLastDistance += gpGlobals->frametime * 32.0f; 

		// but not more than m_flDistance
		m_flLastDistance = min( m_flDistance, m_flLastDistance );
	}
	else
	{
		m_flLastDistance = m_flDistance;
	}

	// calc optimal camer aposition
	VectorMA(targetOrigin, -m_flLastDistance, forward, cameraOrigin );

	// clip against walls
	trace_t trace;
	C_BaseEntity::EnableAbsRecomputations( false ); // HACK don't recompute positions while doing RayTrace
	UTIL_TraceHull( targetOrigin, cameraOrigin, WALL_MIN, WALL_MAX, MASK_SOLID, target, COLLISION_GROUP_NONE, &trace );
	C_BaseEntity::EnableAbsRecomputations( true );

	if (trace.fraction < 1.0)
	{
		// we hit a wall, correct poistion and distance
		cameraOrigin = trace.endpos;
		m_flLastDistance = VectorLength( cameraOrigin -  targetOrigin );
	}


	VectorCopy( cameraAngles, m_aCamAngle );
	VectorCopy( cameraOrigin, m_vCamOrigin );

	VectorCopy( m_aCamAngle, eyeAngles );
	VectorCopy( m_vCamOrigin, eyeOrigin );
}

void C_HLTVCamera::CalcChaseCamView2( Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{

}

int  C_HLTVCamera::GetMode()
{
	return m_nCameraMode;	
}

int	 C_HLTVCamera::GetPrimaryTraget()
{
	return m_iTraget1;
}

void C_HLTVCamera::CalcInEyeCamView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{
	C_BasePlayer *pPlayer = UTIL_PlayerByIndex( m_iTraget1 );

	if ( !pPlayer )
		return;

	m_aCamAngle	= pPlayer->EyeAngles();
	m_vCamOrigin = pPlayer->GetAbsOrigin();
	fov = m_flFOV =	pPlayer->GetFOV();

	if ( pPlayer->GetFlags() & FL_DUCKING )
	{
		m_vCamOrigin += GameRules()->GetViewVectors()->m_vDuckView;
	}
	else
	{
		m_vCamOrigin += GameRules()->GetViewVectors()->m_vView;
	}

	eyeOrigin = m_vCamOrigin;
	eyeAngles = m_aCamAngle;
	
}

void C_HLTVCamera::CalcRoamingView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( !pPlayer )
		return;

	eyeOrigin = m_vCamOrigin = pPlayer->EyePosition();
	eyeAngles = m_aCamAngle = pPlayer->EyeAngles();
	fov = m_flFOV = pPlayer->GetFOV();
}

void C_HLTVCamera::CalcFixedView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
{
	eyeOrigin = m_vCamOrigin;
	eyeAngles = m_aCamAngle;
	fov = m_flFOV;

	if ( m_iTraget1 == 0 )
		return;

	C_BaseEntity * traget = ClientEntityList().GetBaseEntity( m_iTraget1 );
	
	if ( traget )
	{
		// if we're chasing a target, change viewangles
		VectorAngles( traget->GetAbsOrigin() - m_vCamOrigin, m_aCamAngle );
	}
}

void C_HLTVCamera::GetCurrentView(Vector& origin, QAngle& angles, float& fov)
{
	if ( m_iCameraMan > 0 )
	{
		CBasePlayer * cameraman = ToBasePlayer( ClientEntityList().GetBaseEntity(m_iCameraMan) );

		if ( cameraman )
		{
			float zNear;
			float zFar;
			cameraman->CalcView( origin, angles, zNear, zFar, fov );
		}

		return;
	}

	switch ( m_nCameraMode )
	{
		case OBS_MODE_ROAMING	:	CalcRoamingView( origin, angles, fov );
									break;

		case OBS_MODE_FIXED		:	CalcFixedView( origin, angles, fov );
									break;

		case OBS_MODE_IN_EYE	:	CalcInEyeCamView( origin, angles, fov );
									break;

		case OBS_MODE_CHASE		:	CalcChaseCamView( origin, angles, fov  );
									break;
	}
}

void C_HLTVCamera::SetMode(int iMode)
{
	Assert( iMode > OBS_MODE_NONE && iMode <= OBS_MODE_ROAMING );
	m_nCameraMode = iMode;
}

void C_HLTVCamera::SpecNextPlayer( bool bInverse )
{
	int start = 1;

	if ( m_iTraget1 > 0 && m_iTraget1 <= gpGlobals->maxClients )
		start = m_iTraget1;

	int index = start;

	while ( true )
	{	
		// got next/prev player
		if ( bInverse )
			index--;
		else
			index++;

		// check bounds
		if ( index < 1 )
			index = gpGlobals->maxClients;
		else if ( index > gpGlobals->maxClients )
			index = 1;

		if ( index == start )
			break; // couldn't find a new valid player

		C_BasePlayer *pPlayer =	UTIL_PlayerByIndex( index );

		if ( !pPlayer )
			continue;

		// only follow living players or dedicated spectators
		if ( pPlayer->IsObserver() && pPlayer->GetTeamNumber() != TEAM_SPECTATOR )
			continue;

		break; // found a new player
	}

	m_iTraget1 = index;
}

void C_HLTVCamera::FireGameEvent( KeyValues * event)
{
	if ( !engine->IsHLTV() )
		return;	// not in HLTV mode

	const char *type = event->GetName();

	if ( Q_strcmp( "game_newmap", type ) == 0 )
	{
		Reset();	// reset all camera settings
		return;
	}

	else if ( Q_strcmp( "hltv_cameraman", type ) == 0 )
	{
		Reset();

		m_nCameraMode = OBS_MODE_ROAMING;
		
		int mode = event->GetInt("mode");

		if ( mode > 0 )
		{
			m_iCameraMan = event->GetInt( "index" );
		}
		else 
		{
			m_iCameraMan = 0;
		}

		return;
	}

	else if ( Q_strcmp( "hltv_fixed", type ) == 0 )
	{
		Reset();

		m_nCameraMode = OBS_MODE_FIXED;
		
		m_vCamOrigin.x = event->GetInt( "posx" );
		m_vCamOrigin.y = event->GetInt( "posy" );
		m_vCamOrigin.z = event->GetInt( "posz" );

		m_aCamAngle.x = event->GetInt( "anglea" );
		m_aCamAngle.y = event->GetInt( "angleb" );
		m_aCamAngle.z = 0; // no roll yet

		m_iTraget1 = event->GetInt( "target" );
		
		return;
	}

	else if ( Q_strcmp( "hltv_chase1", type ) == 0 )
	{
		Reset();

		m_nCameraMode = OBS_MODE_CHASE;
				
		m_iTraget1 = event->GetInt( "target" );

		m_flFOV		= event->GetFloat( "fov", hltv_cam_fov.GetFloat() );
		m_flDistance= event->GetFloat( "distance", hltv_cam_distance.GetFloat() );
		m_flOffset	= event->GetFloat( "offset", hltv_cam_offset.GetFloat() );
		m_flTheta	= event->GetFloat( "theta", hltv_cam_theta.GetFloat() );
		m_flPhi		= event->GetFloat( "phi", hltv_cam_phi.GetFloat() );
		m_flInertia = event->GetFloat( "inertia", hltv_cam_inertia.GetFloat() );
				
		return;
	}

	else if ( Q_strcmp( "hltv_chase2", type ) == 0 )
	{
		Reset();

		m_nCameraMode = OBS_MODE_CHASE;
		
		m_iTraget1	= event->GetInt( "target1" );
		m_iTraget2	= event->GetInt( "target2" );

		return;
	}
}