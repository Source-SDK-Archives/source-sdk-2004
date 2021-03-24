//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_AI_BaseNPC.h"
#include "engine/IVDebugOverlay.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAX_DEATHPOSE_FRAMES 6

IMPLEMENT_CLIENTCLASS_DT( C_AI_BaseNPC, DT_AI_BaseNPC, CAI_BaseNPC )
	RecvPropInt( RECVINFO( m_lifeState ) ),
	RecvPropBool( RECVINFO( m_bPerformAvoidance ) ),
	RecvPropBool( RECVINFO( m_bIsMoving ) ),
	RecvPropBool( RECVINFO( m_bFadeCorpse ) ),
	RecvPropInt( RECVINFO ( m_iDeathPose) ),
	RecvPropInt( RECVINFO( m_iDeathFrame) ),
END_RECV_TABLE()

C_AI_BaseNPC::C_AI_BaseNPC()
{
}

//-----------------------------------------------------------------------------
// Makes ragdolls ignore npcclip brushes
//-----------------------------------------------------------------------------
unsigned int C_AI_BaseNPC::PhysicsSolidMaskForEntity( void ) const 
{
	// This allows ragdolls to move through npcclip brushes
	if ( !IsRagdoll() )
	{
		return MASK_NPCSOLID; 
	}
	return MASK_SOLID;
}


void C_AI_BaseNPC::GetRagdollPreSequence( matrix3x4_t *preBones, float flTime )
{
	Interpolate( flTime );

	// Setup previous bone state to extrapolate physics velocity
	SetupBones( preBones, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, flTime );
}

void C_AI_BaseNPC::GetRagdollCurSequence( matrix3x4_t *curBones, float flTime )
{
	// blow the cached prev bones
	InvalidateBoneCache();

	Vector vPrevOrigin = GetAbsOrigin();

	Interpolate( flTime );
	
	if ( m_iDeathPose != ACT_INVALID )
	{
		Vector vNewOrigin = GetAbsOrigin();
		Vector vDirection = vNewOrigin - vPrevOrigin;

		float flVelocity = VectorNormalize( vDirection );

		Vector vAdjustedOrigin = vNewOrigin + vDirection * ( ( flVelocity * flVelocity ) * gpGlobals->frametime );

		int iTempSequence = GetSequence();
		float flTempCycle = GetCycle();

		SetEffects( EF_NOINTERP );

		SetSequence( m_iDeathPose );

		SetCycle( (float)m_iDeathFrame / MAX_DEATHPOSE_FRAMES );

		SetAbsOrigin( vAdjustedOrigin );

		// Now do the current bone setup
		SetupBones( curBones, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, flTime );

		SetAbsOrigin( vNewOrigin );

		// blow the cached prev bones
		InvalidateBoneCache();

		SetSequence( iTempSequence );
		SetCycle( flTempCycle );

		Interpolate( gpGlobals->curtime );

		SetupBones( NULL, -1, BONE_USED_BY_ANYTHING, gpGlobals->curtime );

		RemoveEffects( EF_NOINTERP );
	}
	else
	{
		SetupBones( curBones, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, flTime );

		// blow the cached prev bones
		InvalidateBoneCache();

		SetupBones( NULL, -1, BONE_USED_BY_ANYTHING, flTime );
	}
}
