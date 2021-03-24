//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// nav_edit.cpp
// Implementation of Navigation Mesh edit mode
// Author: Michael Booth, 2003-2004

#include "cbase.h"
#include "nav_mesh.h"
#include "nav_pathfind.h"

#define DrawLine( from, to, duration, red, green, blue )		NDebugOverlay::Line( from, to, red, green, blue, true, 0.1f )

//--------------------------------------------------------------------------------------------------------------
static CNavArea *lastSelectedArea = NULL;
static NavCornerType markedCorner = NUM_CORNERS;

static bool isCreatingNavArea = false;						///< if true, we are manually creating a new nav area
static bool isAnchored = false;
static Vector anchor;

static bool isPlaceMode = false;							///< if true, we are in place editing mode
static bool isPlacePainting = false;						///< if true, we set an area's place by pointing at it

static float editTimestamp = 0.0f;
static float lastDrawTimestamp = 0.0f;

/*
void EditNav_Precache(void *pUser)
{
	CBaseEntity::PrecacheScriptSound( "Bot.EditSwitchOn" );
	CBaseEntity::PrecacheScriptSound( "EDIT_TOGGLE_PLACE_MODE" );
	CBaseEntity::PrecacheScriptSound( "Bot.EditSwitchOff" );
	CBaseEntity::PrecacheScriptSound( "Bot.EditSwitchOn" );
	CBaseEntity::PrecacheScriptSound( "EDIT_PLACE_PICK" );
	CBaseEntity::PrecacheScriptSound( "EDIT_TOGGLE_PLACE_MODE" );
	CBaseEntity::PrecacheScriptSound( "EDIT_DELETE" );
	CBaseEntity::PrecacheScriptSound( "EDIT.ToggleAttribute" );
	CBaseEntity::PrecacheScriptSound( "EDIT_SPLIT.MarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_SPLIT.NoMarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MERGE.Enable" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MERGE.Disable" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MERGE.Disable" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MARK.Enable" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MARK.Disable" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MARK_UNNAMED.Enable" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MARK_UNNAMED.NoMarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MARK_UNNAMED.MarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_CONNECT.AllDirections" );
	CBaseEntity::PrecacheScriptSound( "EDIT_CONNECT.Added" );
	CBaseEntity::PrecacheScriptSound( "EDIT_CONNECT.AllDirections" );
	CBaseEntity::PrecacheScriptSound( "EDIT_DISCONNECT.MarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_DISCONNECT.NoMarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_SPLICE.MarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_SPLICE.NoMarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_SPLICE.NoMarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_SELECT_CORNER.MarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_SELECT_CORNER.NoMarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MOVE_CORNER.MarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MOVE_CORNER.NoMarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MOVE_CORNER.MarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_MOVE_CORNER.NoMarkedArea" );
	CBaseEntity::PrecacheScriptSound( "EDIT_BEGIN_AREA.Creating" );
	CBaseEntity::PrecacheScriptSound( "EDIT_BEGIN_AREA.NotCreating" );
	CBaseEntity::PrecacheScriptSound( "EDIT_END_AREA.Creating" );
	CBaseEntity::PrecacheScriptSound( "EDIT_END_AREA.NotCreating" );
}

PRECACHE_REGISTER_FN( EditNav_Precache );
*/

/**
 * Draw navigation areas and edit them
 */
void CNavMesh::EditReset( void )
{
	lastSelectedArea = NULL;
	isCreatingNavArea = false;
	editTimestamp = 0.0f;
	isPlacePainting = false;
	lastDrawTimestamp = 0.0f;
}

void DrawHidingSpots( const CNavArea *area )
{
	const HidingSpotList *list = area->GetHidingSpotList();

	FOR_EACH_LL( (*list), it )
	{
		const HidingSpot *spot = (*list)[ it ];

		unsigned char r, g, b;

		if (spot->IsIdealSniperSpot())
		{
			r = 255; g = 0; b = 0;
		}
		else if (spot->IsGoodSniperSpot())
		{
			r = 255; g = 0; b = 255;
		}
		else if (spot->HasGoodCover())
		{
			r = 0; g = 255; b = 0;
		}
		else
		{
			r = 0; g = 0; b = 1;
		}

		DrawLine( *spot->GetPosition(), *spot->GetPosition() + Vector( 0, 0, 50 ), 3, r, g, b );
	}
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Draw ourselves and adjacent areas
 */
void CNavArea::DrawConnectedAreas( void )
{
	CBasePlayer *player = UTIL_GetListenServerHost();
	if (player == NULL)
		return;

	// draw self
	if (isPlaceMode)
	{
		if (GetPlace() == 0)
			Draw( 50, 0, 0, 3 );
		else if (GetPlace() != TheNavMesh->GetNavPlace())
			Draw( 0, 0, 200, 3 );
		else
			Draw( 0, 255, 0, 3 );
	}
	else
	{
		if (TheNavMesh->GetMarkedArea() == this)
		{
			Draw( 255, 255, 255, 3 );
		}
		else
		{
			Draw( 255, 255, 0, 3 );
		}

		DrawHidingSpots( this );
	}

	// draw connected areas
	for( int i=0; i<NUM_DIRECTIONS; ++i )
	{
		NavDirType dir = (NavDirType)i;

		int count = GetAdjacentCount( dir );

		for( int a=0; a<count; ++a )
		{
			CNavArea *adj = GetAdjacentArea( dir, a );

			if (isPlaceMode)
			{
				if (adj->GetPlace() == 0)
					adj->Draw( 50, 0, 0, 3 );
				else if (adj->GetPlace() != TheNavMesh->GetNavPlace())
					adj->Draw( 0, 0, 200, 3 );
				else
					adj->Draw( 0, 255, 0, 3 );
			}
			else
			{
				if (adj->IsDegenerate())
				{
					static IntervalTimer blink;
					static bool blinkOn = false;

					if (blink.GetElapsedTime() > 1.0f)
					{
						blink.Reset();
						blinkOn = !blinkOn;
					}

					if (blinkOn)
						adj->Draw( 255, 255, 255, 3 );
					else
						adj->Draw( 255, 0, 255, 3 );
				}
				else
				{
					adj->Draw( 255, 0, 0, 3 );
				}

				DrawHidingSpots( adj );

				Vector from, to;
				Vector hookPos;
				float halfWidth;
				float size = 5.0f;
				ComputePortal( adj, dir, &hookPos, &halfWidth );

				switch( dir )
				{
					case NORTH:
						from = hookPos + Vector( 0.0f, size, 0.0f );
						to = hookPos + Vector( 0.0f, -size, 0.0f );
						break;
					case SOUTH:
						from = hookPos + Vector( 0.0f, -size, 0.0f );
						to = hookPos + Vector( 0.0f, size, 0.0f );
						break;
					case EAST:
						from = hookPos + Vector( -size, 0.0f, 0.0f );
						to = hookPos + Vector( +size, 0.0f, 0.0f );
						break;
					case WEST:
						from = hookPos + Vector( size, 0.0f, 0.0f );
						to = hookPos + Vector( -size, 0.0f, 0.0f );
						break;
				}

				from.z = GetZ( &from );
				to.z = adj->GetZ( &to );

				Vector drawTo;
				adj->GetClosestPointOnArea( &to, &drawTo );

				if (adj->IsConnected( this, OppositeDirection( dir ) ) )
					DrawLine( from, drawTo, 3, 0, 255, 255 );
				else
					DrawLine( from, drawTo, 3, 0, 0, 255 );
			}
		}
	}
}

/**
 * Flood fills all areas with current place
 */
class PlaceFloodFillFunctor
{
public:
	PlaceFloodFillFunctor( CNavArea *area )
	{
		m_initialPlace = area->GetPlace();
	}

	bool operator() ( CNavArea *area )
	{
		if (area->GetPlace() != m_initialPlace)
			return false;

		area->SetPlace( TheNavMesh->GetNavPlace() );

		return true;
	}

private:
	unsigned int m_initialPlace;
};

//--------------------------------------------------------------------------------------------------------------
/**
 * Draw navigation areas and edit them
 * @todo Clean the whole edit system up - its structure is legacy from peculiarities in GoldSrc.
 */
void CNavMesh::UpdateEditMode( void )
{
	CBasePlayer *player = UTIL_GetListenServerHost();
	if (player == NULL)
		return;

	Vector playerOrigin = player->GetAbsOrigin();

	const float maxRange = 1000.0f;		// 500

	int beamTime = 1;

	// show ladder connections
	FOR_EACH_LL( m_ladderList, it )
	{
		CNavLadder *ladder = m_ladderList[ it ];

		float dx = playerOrigin.x - ladder->m_bottom.x;
		float dy = playerOrigin.y - ladder->m_bottom.y;
		if (dx*dx + dy*dy > maxRange*maxRange)
			continue;


		DrawLine( ladder->m_top, ladder->m_bottom, beamTime, 255, 0, 255 );

		Vector bottom = ladder->m_bottom;
		Vector top = ladder->m_top;

		AddDirectionVector( &top, ladder->m_dir, HalfHumanWidth );
		AddDirectionVector( &bottom, ladder->m_dir, HalfHumanWidth );

		DrawLine( top, bottom, beamTime, 0, 0, 255 );

		if (ladder->m_bottomArea)
			DrawLine( bottom + Vector( 0, 0, GenerationStepSize ), *ladder->m_bottomArea->GetCenter(), beamTime, 0, 0, 255 );

		if (ladder->m_topForwardArea)
			DrawLine( top, *ladder->m_topForwardArea->GetCenter(), beamTime, 0, 0, 255 );

		if (ladder->m_topLeftArea)
			DrawLine( top, *ladder->m_topLeftArea->GetCenter(), beamTime, 0, 0, 255 );

		if (ladder->m_topRightArea)
			DrawLine( top, *ladder->m_topRightArea->GetCenter(), beamTime, 0, 0, 255 );

		if (ladder->m_topBehindArea)
			DrawLine( top, *ladder->m_topBehindArea->GetCenter(), beamTime, 0, 0, 255 );
	}

	// draw approach points for marked area
	if (nav_show_approach_points.GetBool() && GetMarkedArea())
	{
		Vector ap;
		float halfWidth;
		for( int i=0; i<GetMarkedArea()->GetApproachInfoCount(); ++i )
		{
			const CNavArea::ApproachInfo *info = GetMarkedArea()->GetApproachInfo( i );

			// compute approach point
			if (info->hereToNextHow <= GO_WEST)
			{
				info->here.area->ComputePortal( info->next.area, (NavDirType)info->hereToNextHow, &ap, &halfWidth );
				ap.z = info->next.area->GetZ( &ap );
			}
			else
			{
				// use the area's center as an approach point
				ap = *info->here.area->GetCenter();
			}

			DrawLine( ap + Vector( 0, 0, 50 ), ap + Vector( 10, 0, 0 ), beamTime, 255, 100, 0 );
			DrawLine( ap + Vector( 0, 0, 50 ), ap + Vector( -10, 0, 0 ), beamTime, 255, 100, 0 );
			DrawLine( ap + Vector( 0, 0, 50 ), ap + Vector( 0, 10, 0 ), beamTime, 255, 100, 0 );
			DrawLine( ap + Vector( 0, 0, 50 ), ap + Vector( 0, -10, 0 ), beamTime, 255, 100, 0 );
		}
	}

	Vector dir;
	AngleVectors( player->EyeAngles() + player->GetPunchAngle(), &dir );

	Vector from = player->EyePosition();
	Vector to = from + maxRange * dir;

	trace_t result;
	UTIL_TraceLine( from, to, MASK_PLAYERSOLID_BRUSHONLY, player, COLLISION_GROUP_NONE, &result );

	if (result.fraction != 1.0f)
	{
		// draw cursor
		m_editCursorPos = result.endpos;
		float cursorSize = 10.0f;

		DrawLine( m_editCursorPos + Vector( 0, 0, cursorSize ), m_editCursorPos,																beamTime, 255, 255, 255 );
		DrawLine( m_editCursorPos + Vector( cursorSize, 0, 0 ), m_editCursorPos + Vector( -cursorSize, 0, 0 ), beamTime, 255, 255, 255 );
		DrawLine( m_editCursorPos + Vector( 0, cursorSize, 0 ), m_editCursorPos + Vector( 0, -cursorSize, 0 ), beamTime, 255, 255, 255 );

		if (isCreatingNavArea)
		{
			if (isAnchored)
			{
				// show drag rectangle
				float z = anchor.z + 2.0f;
				DrawLine( Vector( m_editCursorPos.x, m_editCursorPos.y, z ), Vector( anchor.x, m_editCursorPos.y, z ), beamTime, 0, 255, 255 );
				DrawLine( Vector( anchor.x, anchor.y, z ), Vector( anchor.x, m_editCursorPos.y, z ), beamTime, 0, 255, 255 );
				DrawLine( Vector( anchor.x, anchor.y, z ), Vector( m_editCursorPos.x, anchor.y, z ), beamTime, 0, 255, 255 );
				DrawLine( Vector( m_editCursorPos.x, m_editCursorPos.y, z ), Vector( m_editCursorPos.x, anchor.y, z ), beamTime, 0, 255, 255 );
			}
			else
			{
				// anchor starting corner
				anchor = m_editCursorPos;
				isAnchored = true;
			}
		}

		// find the area the player is pointing at
		CNavArea *area = TheNavMesh->GetNearestNavArea( &result.endpos );

		if (area)
		{
			// if area changed, print its ID
			if (area != lastSelectedArea)
			{
				lastSelectedArea = area;

				char buffer[80];
				char attrib[80];
				char locName[80];

				if (area->GetPlace())
				{
					const char *name = TheNavMesh->PlaceToName( area->GetPlace() );
					if (name)
						strcpy( locName, name );
					else
						strcpy( locName, "ERROR" );
				}
				else
				{
					locName[0] = '\000';
				}

				if (isPlaceMode)
				{
					attrib[0] = '\000';
				}
				else
				{
					Q_snprintf( attrib, sizeof( attrib ), "%s%s%s%s", 
								(area->GetAttributes() & NAV_MESH_CROUCH) ? "CROUCH " : "",
								(area->GetAttributes() & NAV_MESH_JUMP) ? "JUMP " : "",
								(area->GetAttributes() & NAV_MESH_PRECISE) ? "PRECISE " : "",
								(area->GetAttributes() & NAV_MESH_NO_JUMP) ? "NO_JUMP " : "" );
				}

				Q_snprintf( buffer, sizeof( buffer ), "Area #%d %s %s\n", area->GetID(), locName, attrib );
				NDebugOverlay::ScreenText( 0.5, 0.5, buffer, 255, 255, 0, 128, 0.5 );

				// do "place painting"
				if (isPlacePainting)
				{				
					if (area->GetPlace() != TheNavMesh->GetNavPlace())
					{
						area->SetPlace( TheNavMesh->GetNavPlace() );
						player->EmitSound( "Bot.EditSwitchOn" );
					}
				}
			}

			if (isPlaceMode)
			{
				area->DrawConnectedAreas();

				switch( m_editCmd )
				{
					case EDIT_TOGGLE_PLACE_MODE:
						player->EmitSound( "EDIT_TOGGLE_PLACE_MODE" );
						isPlaceMode = false;
						return;

					case EDIT_TOGGLE_PLACE_PAINTING:
					{
						if (isPlacePainting)
						{
							isPlacePainting = false;
							player->EmitSound( "Bot.EditSwitchOff" );
						}
						else
						{
							isPlacePainting = true;

							player->EmitSound( "Bot.EditSwitchOn" );

							// paint the initial area
							area->SetPlace( TheNavMesh->GetNavPlace() );
						}
						break;
					}

					case EDIT_PLACE_PICK:
						player->EmitSound( "EDIT_PLACE_PICK" );
						TheNavMesh->SetNavPlace( area->GetPlace() );
						break;

					case EDIT_PLACE_FLOODFILL:
						PlaceFloodFillFunctor pff( area );
						SearchSurroundingAreas( area, area->GetCenter(), pff );
						break;
				}
			}
			else	// normal editing mode
			{
				// draw the "marked" area
				if (GetMarkedArea())
				{
					GetMarkedArea()->Draw( 0, 255, 255, beamTime );

					if ( markedCorner != NUM_CORNERS )
					{
						GetMarkedArea()->DrawMarkedCorner( markedCorner, 0, 0, 255, beamTime );
					}
				}


				// draw split line
				const Extent *extent = area->GetExtent();

				float yaw = player->EyeAngles().y;
				while( yaw > 360.0f )
					yaw -= 360.0f;

				while( yaw < 0.0f )
					yaw += 360.0f;

				float splitEdge;
				bool splitAlongX;

				if ((yaw < 45.0f || yaw > 315.0f) || (yaw > 135.0f && yaw < 225.0f))
				{
					splitEdge = GenerationStepSize * (int)(result.endpos.y/GenerationStepSize);

					from.x = extent->lo.x;
					from.y = splitEdge;
					from.z = area->GetZ( &from );

					to.x = extent->hi.x;
					to.y = splitEdge;
					to.z = area->GetZ( &to );

					splitAlongX = true;
				}
				else
				{
					splitEdge = GenerationStepSize * (int)(result.endpos.x/GenerationStepSize);

					from.x = splitEdge;
					from.y = extent->lo.y;
					from.z = area->GetZ( &from );

					to.x = splitEdge;
					to.y = extent->hi.y;
					to.z = area->GetZ( &to );

					splitAlongX = false;
				}

				DrawLine( from, to, beamTime, 255, 255, 255 );

				// draw the area we are pointing at and all connected areas
				area->DrawConnectedAreas();


				// do area-dependant edit commands, if any
				switch( m_editCmd )
				{
					case EDIT_TOGGLE_PLACE_MODE:
						player->EmitSound( "EDIT_TOGGLE_PLACE_MODE" );
						isPlaceMode = true;
						return;

					case EDIT_DELETE:
						player->EmitSound( "EDIT_DELETE" );
						TheNavAreaList.FindAndRemove( area );
						delete area;
						return;

					case EDIT_ATTRIB_CROUCH:
						player->EmitSound( "EDIT.ToggleAttribute" );
						area->SetAttributes( area->GetAttributes() ^ NAV_MESH_CROUCH );
						break;

					case EDIT_ATTRIB_JUMP:
						player->EmitSound( "EDIT.ToggleAttribute" );
						area->SetAttributes( area->GetAttributes() ^ NAV_MESH_JUMP );
						break;

					case EDIT_ATTRIB_PRECISE:
						player->EmitSound( "EDIT.ToggleAttribute" );
						area->SetAttributes( area->GetAttributes() ^ NAV_MESH_PRECISE );
						break;

					case EDIT_ATTRIB_NO_JUMP:
						player->EmitSound( "EDIT.ToggleAttribute" );
						area->SetAttributes( area->GetAttributes() ^ NAV_MESH_NO_JUMP );
						break;

					case EDIT_SPLIT:
						if (area->SplitEdit( splitAlongX, splitEdge ))
							player->EmitSound( "EDIT_SPLIT.MarkedArea" );
						else
							player->EmitSound( "EDIT_SPLIT.NoMarkedArea" );
						break;

					case EDIT_MERGE:
						if (GetMarkedArea())
						{
							if (area->MergeEdit( GetMarkedArea() ))
								player->EmitSound( "EDIT_MERGE.Enable" );
							else
								player->EmitSound( "EDIT_MERGE.Disable" );
						}
						else
						{
							Msg( "To merge, mark an area, highlight a second area, then invoke the merge command" );
							player->EmitSound( "EDIT_MERGE.Disable" );
						}
						break;

					case EDIT_MARK:
						if (GetMarkedArea())
						{
							player->EmitSound( "EDIT_MARK.Enable" );
							SetMarkedArea( NULL );
						}
						else
						{
							player->EmitSound( "EDIT_MARK.Disable" );
							SetMarkedArea( area );

							int connected = 0;
							connected += GetMarkedArea()->GetAdjacentCount( NORTH );
							connected += GetMarkedArea()->GetAdjacentCount( SOUTH );
							connected += GetMarkedArea()->GetAdjacentCount( EAST );
							connected += GetMarkedArea()->GetAdjacentCount( WEST );

							Msg( "Marked Area is connected to %d other Areas\n", connected );
						}
						break;

					case EDIT_MARK_UNNAMED:
						if (GetMarkedArea())
						{
							player->EmitSound( "EDIT_MARK_UNNAMED.Enable" );
							SetMarkedArea( NULL );
						}
						else
						{
							SetMarkedArea( NULL );
							FOR_EACH_LL( TheNavAreaList, it )
							{
								CNavArea *area = TheNavAreaList[ it ];

								if ( area->GetPlace() == 0 )
								{
									SetMarkedArea( area );
									break;
								}
							}
							if ( !GetMarkedArea() )
							{
								player->EmitSound( "EDIT_MARK_UNNAMED.NoMarkedArea" );
							}
							else
							{
								player->EmitSound( "EDIT_MARK_UNNAMED.MarkedArea" );

								int connected = 0;
								connected += GetMarkedArea()->GetAdjacentCount( NORTH );
								connected += GetMarkedArea()->GetAdjacentCount( SOUTH );
								connected += GetMarkedArea()->GetAdjacentCount( EAST );
								connected += GetMarkedArea()->GetAdjacentCount( WEST );

								int totalUnnamedAreas = 0;
								FOR_EACH_LL( TheNavAreaList, it )
								{
									CNavArea *area = TheNavAreaList[ it ];
									if ( area->GetPlace() == 0 )
									{
										++totalUnnamedAreas;
									}
								}

								Msg( "Marked Area is connected to %d other Areas - there are %d total unnamed areas\n", connected, totalUnnamedAreas );
							}
						}
						break;

					/*
					case EDIT_WARP_TO_MARK:
						if (GetMarkedArea())
						{
							CBasePlayer *pLocalPlayer = player;
							if ( pLocalPlayer && pLocalPlayer->GetTeamNumber() == SPECTATOR && pLocalPlayer->pev->iuser1 == OBS_ROAMING )
							{
								Vector origin = *GetMarkedArea()->GetCenter() + Vector( 0, 0, 0.75f * HumanHeight );
								UTIL_SetOrigin( pLocalPlayer->pev, origin );
							}
						}
						else
						{
							player->EmitSound( "xxx" );
						}
						break;
					*/

					case EDIT_CONNECT:
						if (GetMarkedArea())
						{
							NavDirType dir = GetMarkedArea()->ComputeDirection( &m_editCursorPos );
							if (dir == NUM_DIRECTIONS)
							{
								player->EmitSound( "EDIT_CONNECT.AllDirections" );
							}
							else
							{
								GetMarkedArea()->ConnectTo( area, dir );
								player->EmitSound( "EDIT_CONNECT.Added" );
							}
						}
						else
						{
							Msg( "To connect areas, mark an area, highlight a second area, then invoke the connect command. Make sure the cursor is directly north, south, east, or west of the marked area." );
							player->EmitSound( "EDIT_CONNECT.AllDirections" );
						}
						break;

					case EDIT_DISCONNECT:
						if (GetMarkedArea())
						{
							GetMarkedArea()->Disconnect( area );
							area->Disconnect( GetMarkedArea() );
							player->EmitSound( "EDIT_DISCONNECT.MarkedArea" );
						}
						else
						{
							Msg( "To disconnect areas, mark an area, highlight a second area, then invoke the disconnect command. This will remove all connections between the two areas." );
							player->EmitSound( "EDIT_DISCONNECT.NoMarkedArea" );
						}
						break;

					case EDIT_SPLICE:
						if (GetMarkedArea())
						{
							if (area->SpliceEdit( GetMarkedArea() ))
								player->EmitSound( "EDIT_SPLICE.MarkedArea" );
							else
								player->EmitSound( "EDIT_SPLICE.NoMarkedArea" );
						}
						else
						{
							Msg( "To splice, mark an area, highlight a second area, then invoke the splice command to create an area between them" );
							player->EmitSound( "EDIT_SPLICE.NoMarkedArea" );
						}
						break;

					case EDIT_SELECT_CORNER:
						if (GetMarkedArea())
						{
							int corner = (markedCorner + 1) % (NUM_CORNERS + 1);
							markedCorner = (NavCornerType)corner;
							player->EmitSound( "EDIT_SELECT_CORNER.MarkedArea" );
						}
						else
						{
							player->EmitSound( "EDIT_SELECT_CORNER.NoMarkedArea" );
						}
						break;

					case EDIT_RAISE_CORNER:
						if (GetMarkedArea())
						{
							GetMarkedArea()->RaiseCorner( markedCorner, 1 );
							player->EmitSound( "EDIT_MOVE_CORNER.MarkedArea" );
						}
						else
						{
							player->EmitSound( "EDIT_MOVE_CORNER.NoMarkedArea" );
						}
						break;

					case EDIT_LOWER_CORNER:
						if (GetMarkedArea())
						{
							GetMarkedArea()->RaiseCorner( markedCorner, -1 );
							player->EmitSound( "EDIT_MOVE_CORNER.MarkedArea" );
						}
						else
						{
							player->EmitSound( "EDIT_MOVE_CORNER.NoMarkedArea" );
						}
						break;
				}
			}
		}

		// do area-independant edit commands, if any
		switch( m_editCmd )
		{
			case EDIT_BEGIN_AREA:
			{
				if (isCreatingNavArea)
				{
					isCreatingNavArea = false;
					player->EmitSound( "EDIT_BEGIN_AREA.Creating" );
				}
				else
				{
					player->EmitSound( "EDIT_BEGIN_AREA.NotCreating" );
					isCreatingNavArea = true;
					isAnchored = false;
				}
				break;
			}

			case EDIT_END_AREA:
			{
				if (isCreatingNavArea)
				{
					// create the new nav area
					CNavArea *newArea = new CNavArea( &anchor, &m_editCursorPos );
					TheNavAreaList.AddToTail( newArea );
					TheNavMesh->AddNavArea( newArea );
					player->EmitSound( "EDIT_END_AREA.Creating" );

					// if we have a marked area, inter-connect the two
					if (GetMarkedArea())
					{
						const Extent *extent = GetMarkedArea()->GetExtent();

						if (anchor.x > extent->hi.x && m_editCursorPos.x > extent->hi.x)
						{
							GetMarkedArea()->ConnectTo( newArea, EAST );
							newArea->ConnectTo( GetMarkedArea(), WEST );
						}
						else if (anchor.x < extent->lo.x && m_editCursorPos.x < extent->lo.x)
						{
							GetMarkedArea()->ConnectTo( newArea, WEST );
							newArea->ConnectTo( GetMarkedArea(), EAST );
						}
						else if (anchor.y > extent->hi.y && m_editCursorPos.y > extent->hi.y)
						{
							GetMarkedArea()->ConnectTo( newArea, SOUTH );
							newArea->ConnectTo( GetMarkedArea(), NORTH );
						}
						else if (anchor.y < extent->lo.y && m_editCursorPos.y < extent->lo.y)
						{
							GetMarkedArea()->ConnectTo( newArea, NORTH );
							newArea->ConnectTo( GetMarkedArea(), SOUTH );
						}

						// propogate marked area to new area
						SetMarkedArea( newArea );
					}

					isCreatingNavArea = false;
				}
				else
				{
					player->EmitSound( "EDIT_END_AREA.NotCreating" );
				}
				break;
			}
		}
	}


	// if our last command was not mark (or no command), clear the mark area
	if (m_editCmd != EDIT_MARK && m_editCmd != EDIT_BEGIN_AREA && m_editCmd != EDIT_END_AREA &&
		m_editCmd != EDIT_MARK_UNNAMED && m_editCmd != EDIT_WARP_TO_MARK &&
		m_editCmd != EDIT_SELECT_CORNER && m_editCmd != EDIT_RAISE_CORNER && m_editCmd != EDIT_LOWER_CORNER &&
		m_editCmd != EDIT_NONE)
		SetMarkedArea( NULL );

	// if our last command was not affecting the corner, clear the corner selection
	if (m_editCmd != EDIT_SELECT_CORNER && m_editCmd != EDIT_RAISE_CORNER && m_editCmd != EDIT_LOWER_CORNER && m_editCmd != EDIT_NONE)
		markedCorner = NUM_CORNERS;


	if (isCreatingNavArea && m_editCmd != EDIT_BEGIN_AREA && m_editCmd != EDIT_END_AREA && m_editCmd != EDIT_NONE)
		isCreatingNavArea = false;
}

