//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// nav_generate.cpp
// Auto-generate a Navigation Mesh by sampling the current map
// Author: Michael S. Booth (mike@turtlerockstudios.com), 2003

#include "cbase.h"
#include "nav_mesh.h"
#include "nav_node.h"
#include "nav_pathfind.h"

enum { MAX_BLOCKED_AREAS = 256 };
static unsigned int blockedID[ MAX_BLOCKED_AREAS ];
static int blockedIDCount = 0;

//--------------------------------------------------------------------------------------------------------------
/**
 * Shortest path cost, paying attention to "blocked" areas
 */
class ApproachAreaCost
{
public:
	float operator() ( CNavArea *area, CNavArea *fromArea, const CNavLadder *ladder )
	{
		// check if this area is "blocked"
		for( int i=0; i<blockedIDCount; ++i )
		{
			if (area->GetID() == blockedID[i])
			{
				return -1.0f;
			}
		}

		if (fromArea == NULL)
		{
			// first area in path, no cost
			return 0.0f;
		}
		else
		{
			// compute distance travelled along path so far
			float dist;

			if (ladder)
				dist = ladder->m_length;
			else
				dist = (*area->GetCenter() - *fromArea->GetCenter()).Length();

			float cost = dist + fromArea->GetCostSoFar();

			return cost;
		}
	}
};

//--------------------------------------------------------------------------------------------------------------
/**
 * Can we see this area?
 * For now, if we can see any corner, we can see the area
 * @todo Need to check LOS to more than the corners for large and/or long areas
 */
inline bool IsAreaVisible( const Vector *pos, const CNavArea *area )
{
	Vector corner;
	trace_t result;

	for( int c=0; c<NUM_CORNERS; ++c )
	{
		corner = *area->GetCorner( (NavCornerType)c );
		corner.z += 0.75f * HumanHeight;

		UTIL_TraceLine( *pos, corner, MASK_PLAYERSOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &result );
		if (result.fraction == 1.0f)
		{
			// we can see this area
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Determine the set of "approach areas".
 * An approach area is an area representing a place where players 
 * move into/out of our local neighborhood of areas.
 */
void CNavArea::ComputeApproachAreas( void )
{
	m_approachCount = 0;

	if (nav_quicksave.GetBool())
		return;

	// use the center of the nav area as the "view" point
	Vector eye = m_center;
	if (TheNavMesh->GetGroundHeight( &eye, &eye.z ) == false)
		return;

	// approximate eye position
	if (GetAttributes() & NAV_MESH_CROUCH)
		eye.z += 0.9f * HalfHumanHeight;
	else
		eye.z += 0.9f * HumanHeight;

	enum { MAX_PATH_LENGTH = 256 };
	CNavArea *path[ MAX_PATH_LENGTH ];

	//
	// In order to enumerate all of the approach areas, we need to
	// run the algorithm many times, once for each "far away" area
	// and keep the union of the approach area sets
	//
	FOR_EACH_LL( TheNavAreaList, it )
	{
		CNavArea *farArea = TheNavAreaList[ it ];

		blockedIDCount = 0;

		// skip the small areas
		const float minSize = 200.0f;		// 150
		const Extent *extent = farArea->GetExtent();
		if (extent->SizeX() < minSize || extent->SizeY() < minSize)
			continue;

		// if we can see 'farArea', try again - the whole point is to go "around the bend", so to speak
		if (IsAreaVisible( &eye, farArea ))
			continue;
	
		// make first path to far away area
		ApproachAreaCost cost;
		if (NavAreaBuildPath( this, farArea, NULL, cost ) == false)
			continue;

		//
		// Keep building paths to farArea and blocking them off until we
		// cant path there any more.
		// As areas are blocked off, all exits will be enumerated.
		//
		while( m_approachCount < MAX_APPROACH_AREAS )
		{
			// find number of areas on path
			int count = 0;
			CNavArea *area;
			for( area = farArea; area; area = area->GetParent() )
				++count;

			if (count > MAX_PATH_LENGTH)
				count = MAX_PATH_LENGTH;

			// build path in correct order - from eye outwards
			int i = count;
			for( area = farArea; i && area; area = area->GetParent() )
				path[ --i ] = area;

			// traverse path to find first area we cannot see (skip the first area)
			for( i=1; i<count; ++i )
			{
				// if we see this area, continue on
				if (IsAreaVisible( &eye, path[i] ))
					continue;

				// we can't see this area.
				// mark this area as "blocked" and unusable by subsequent approach paths
				if (blockedIDCount == MAX_BLOCKED_AREAS)
				{
					Msg( "Overflow computing approach areas for area #%d.\n", m_id );
					return;
				}

				// if the area to be blocked is actually farArea, block the one just prior
				// (blocking farArea will cause all subsequent pathfinds to fail)
				int block = (path[i] == farArea) ? i-1 : i;

				blockedID[ blockedIDCount++ ] = path[ block ]->GetID();

				if (block == 0)
					break;

				// store new approach area if not already in set
				int a;
				for( a=0; a<m_approachCount; ++a )
					if (m_approach[a].here.area == path[block-1])
						break;

				if (a == m_approachCount)
				{
					m_approach[ m_approachCount ].prev.area = (block >= 2) ? path[block-2] : NULL;

					m_approach[ m_approachCount ].here.area = path[block-1];
					m_approach[ m_approachCount ].prevToHereHow = path[block-1]->GetParentHow();

					m_approach[ m_approachCount ].next.area = path[block];
					m_approach[ m_approachCount ].hereToNextHow = path[block]->GetParentHow();

					++m_approachCount;
				}

				// we are done with this path
				break;
			}

			// find another path to 'farArea'
			ApproachAreaCost cost;
			if (NavAreaBuildPath( this, farArea, NULL, cost ) == false)
			{
				// can't find a path to 'farArea' means all exits have been already tested and blocked
				break;
			}
		}
	}
}


//--------------------------------------------------------------------------------------------------------------
/**
 * For each ladder in the map, create a navigation representation of it.
 */
void CNavMesh::BuildLadders( void )
{
/* BOTPORT: Implement ladder navigation

  // remove any left-over ladders
	DestroyLadders();

	trace_t result;
	CBaseEntity *entity = UTIL_FindEntityByClassname( NULL, "func_ladder" );
	while( entity && !FNullEnt( entity->edict() ) )
	{
		CNavLadder *ladder = new CNavLadder;

		// compute top & bottom of ladder
		ladder->m_top.x = (entity->pev->absmin.x + entity->pev->absmax.x) / 2.0f;
		ladder->m_top.y = (entity->pev->absmin.y + entity->pev->absmax.y) / 2.0f;
		ladder->m_top.z = entity->pev->absmax.z;

		ladder->m_bottom.x = ladder->m_top.x;
		ladder->m_bottom.y = ladder->m_top.y;
		ladder->m_bottom.z = entity->pev->absmin.z;

		// determine facing - assumes "normal" runged ladder
		float xSize = entity->pev->absmax.x - entity->pev->absmin.x;
		float ySize = entity->pev->absmax.y - entity->pev->absmin.y;
		if (xSize > ySize)
		{
			// ladder is facing north or south - determine which way
			// "pull in" traceline from bottom and top in case ladder abuts floor and/or ceiling
			Vector from = ladder->m_bottom + Vector( 0.0f, GenerationStepSize, GenerationStepSize );
			Vector to = ladder->m_top + Vector( 0.0f, GenerationStepSize, -GenerationStepSize );

			UTIL_TraceLine( from, to, MASK_PLAYERSOLID_BRUSHONLY, entity, COLLISION_GROUP_NONE, &result );

			if (result.fraction != 1.0f || result.startsolid)
				ladder->m_dir = NORTH;
			else
				ladder->m_dir = SOUTH;
		}
		else
		{
			// ladder is facing east or west - determine which way
			Vector from = ladder->m_bottom + Vector( GenerationStepSize, 0.0f, GenerationStepSize );
			Vector to = ladder->m_top + Vector( GenerationStepSize, 0.0f, -GenerationStepSize );

			UTIL_TraceLine( from, to, MASK_PLAYERSOLID_BRUSHONLY, entity, COLLISION_GROUP_NONE, &result );

			if (result.fraction != 1.0f || result.startsolid)
				ladder->m_dir = WEST;
			else
				ladder->m_dir = EAST;
		}

		// adjust top and bottom of ladder to make sure they are reachable
		// (cs_office has a crate right in front of the base of a ladder)
		Vector along = ladder->m_top - ladder->m_bottom;
		float length = along.NormalizeInPlace();
		Vector on, out;
		const float minLadderClearance = 32.0f;

		// adjust bottom to bypass blockages
		const float inc = 10.0f;
		float t;		
		for( t = 0.0f; t <= length; t += inc )
		{
			on = ladder->m_bottom + t * along;

			out = on;
			AddDirectionVector( &out, ladder->m_dir, minLadderClearance );

			UTIL_TraceLine( on, out, MASK_PLAYERSOLID_BRUSHONLY, entity, COLLISION_GROUP_NONE, &result );

			if (result.fraction == 1.0f && !result.startsolid)
			{
				// found viable ladder bottom
				ladder->m_bottom = on;
				break;
			}
		}

		// adjust top to bypass blockages
		for( t = 0.0f; t <= length; t += inc )
		{
			on = ladder->m_top - t * along;

			out = on;
			AddDirectionVector( &out, ladder->m_dir, minLadderClearance );

			UTIL_TraceLine( on, out, MASK_PLAYERSOLID_BRUSHONLY, entity, COLLISION_GROUP_NONE, &result );

			if (result.fraction == 1.0f && !result.startsolid)
			{
				// found viable ladder top
				ladder->m_top = on;
				break;
			}
		}

		ladder->m_length = (ladder->m_top - ladder->m_bottom).Length();

		DirectionToVector2D( ladder->m_dir, &ladder->m_dirVector );

		ladder->m_entity = entity;
		const float nearLadderRange = 75.0f;		// 50

		//
		// Find naviagtion area at bottom of ladder
		//

		// get approximate postion of player on ladder
		Vector center = ladder->m_bottom + Vector( 0, 0, GenerationStepSize );
		AddDirectionVector( &center, ladder->m_dir, HalfHumanWidth );

		ladder->m_bottomArea = TheNavMesh->GetNearestNavArea( &center, true );
		if (!ladder->m_bottomArea)
		{
			ALERT( at_console, "ERROR: Unconnected ladder bottom at ( %g, %g, %g )\n", ladder->m_bottom.x, ladder->m_bottom.y, ladder->m_bottom.z );
		}
		else
		{
			// store reference to ladder in the area
			ladder->m_bottomArea->AddLadderUp( ladder );
		}

		//
		// Find adjacent navigation areas at the top of the ladder
		//

		// get approximate postion of player on ladder
		center = ladder->m_top + Vector( 0, 0, GenerationStepSize );
		AddDirectionVector( &center, ladder->m_dir, HalfHumanWidth );

		// find "ahead" area
		ladder->m_topForwardArea = FindFirstAreaInDirection( &center, OppositeDirection( ladder->m_dir ), nearLadderRange, 120.0f, entity );
		if (ladder->m_topForwardArea == ladder->m_bottomArea)
			ladder->m_topForwardArea = NULL;

		// find "left" area
		ladder->m_topLeftArea = FindFirstAreaInDirection( &center, DirectionLeft( ladder->m_dir ), nearLadderRange, 120.0f, entity );
		if (ladder->m_topLeftArea == ladder->m_bottomArea)
			ladder->m_topLeftArea = NULL;

		// find "right" area
		ladder->m_topRightArea = FindFirstAreaInDirection( &center, DirectionRight( ladder->m_dir ), nearLadderRange, 120.0f, entity );
		if (ladder->m_topRightArea == ladder->m_bottomArea)
			ladder->m_topRightArea = NULL;

		// find "behind" area - must look farther, since ladder is against the wall away from this area
		ladder->m_topBehindArea = FindFirstAreaInDirection( &center, ladder->m_dir, 2.0f*nearLadderRange, 120.0f, entity );
		if (ladder->m_topBehindArea == ladder->m_bottomArea)
			ladder->m_topBehindArea = NULL;

		// can't include behind area, since it is not used when going up a ladder
		if (!ladder->m_topForwardArea && !ladder->m_topLeftArea && !ladder->m_topRightArea)
			ALERT( at_console, "ERROR: Unconnected ladder top at ( %g, %g, %g )\n", ladder->m_top.x, ladder->m_top.y, ladder->m_top.z );

		// store reference to ladder in the area(s)
		if (ladder->m_topForwardArea)
			ladder->m_topForwardArea->AddLadderDown( ladder );

		if (ladder->m_topLeftArea)
			ladder->m_topLeftArea->AddLadderDown( ladder );

		if (ladder->m_topRightArea)
			ladder->m_topRightArea->AddLadderDown( ladder );

		if (ladder->m_topBehindArea)
			ladder->m_topBehindArea->AddLadderDown( ladder );

		// adjust top of ladder to highest connected area
		float topZ = -99999.9f;
		bool topAdjusted = false;
		CNavArea *topAreaList[4];
		topAreaList[0] = ladder->m_topForwardArea;
		topAreaList[1] = ladder->m_topLeftArea;
		topAreaList[2] = ladder->m_topRightArea;
		topAreaList[3] = ladder->m_topBehindArea;

		for( int a=0; a<4; ++a )
		{
			CNavArea *topArea = topAreaList[a];
			if (topArea == NULL)
				continue;

			Vector close;
			topArea->GetClosestPointOnArea( &ladder->m_top, &close );
			if (topZ < close.z)
			{
				topZ = close.z;
				topAdjusted = true;
			}
		}

		if (topAdjusted)
			ladder->m_top.z = topZ;

		//
		// Determine whether this ladder is "dangling" or not
		// "Dangling" ladders are too high to go up
		//
		ladder->m_isDangling = false;
		if (ladder->m_bottomArea)
		{
			Vector bottomSpot;
			ladder->m_bottomArea->GetClosestPointOnArea( &ladder->m_bottom, &bottomSpot );
			if (ladder->m_bottom.z - bottomSpot.z > HumanHeight)
				ladder->m_isDangling = true;
		}

		// add ladder to global list
		m_ladderList.AddToTail( ladder );		

		entity = UTIL_FindEntityByClassname( entity, "func_ladder" );
	}
*/
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Mark all areas that require a jump to get through them.
 * This currently relies on jump areas having extreme slope.
 */
void CNavMesh::MarkJumpAreas( void )
{
	FOR_EACH_LL( TheNavAreaList, it )
	{
		CNavArea *area = TheNavAreaList[ it ];
		Vector u, v;

		// compute our unit surface normal
		u.x = area->m_extent.hi.x - area->m_extent.lo.x;
		u.y = 0.0f;
		u.z = area->m_neZ - area->m_extent.lo.z;

		v.x = 0.0f;
		v.y = area->m_extent.hi.y - area->m_extent.lo.y;
		v.z = area->m_swZ - area->m_extent.lo.z;

		Vector normal = CrossProduct( u, v );
		normal.NormalizeInPlace();

		if (normal.z < MaxUnitZSlope)
			area->SetAttributes( area->GetAttributes() | NAV_MESH_JUMP );
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Recursively chop area in half along X until child areas are roughly square
 */
static void splitX( CNavArea *area )
{
	if (area->IsRoughlySquare())
		return;

	float split = area->GetSizeX();
	split /= 2.0f;
	split += area->GetExtent()->lo.x;

	split = TheNavMesh->SnapToGrid( split );

	const float epsilon = 0.1f;
	if (fabs(split - area->GetExtent()->lo.x) < epsilon ||
		fabs(split - area->GetExtent()->hi.x) < epsilon)
	{
		// too small to subdivide
		return;
	}

	CNavArea *alpha, *beta;
	if (area->SplitEdit( false, split, &alpha, &beta ))
	{
		// split each new area until square
		splitX( alpha );
		splitX( beta );
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Recursively chop area in half along Y until child areas are roughly square
 */
static void splitY( CNavArea *area )
{
	if (area->IsRoughlySquare())
		return;

	float split = area->GetSizeY();
	split /= 2.0f;
	split += area->GetExtent()->lo.y;

	split = TheNavMesh->SnapToGrid( split );

	const float epsilon = 0.1f;
	if (fabs(split - area->GetExtent()->lo.y) < epsilon ||
		fabs(split - area->GetExtent()->hi.y) < epsilon)
	{
		// too small to subdivide
		return;
	}

	CNavArea *alpha, *beta;
	if (area->SplitEdit( true, split, &alpha, &beta ))
	{
		// split each new area until square
		splitY( alpha );
		splitY( beta );
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Split any long, thin, areas into roughly square chunks.
 */
void CNavMesh::SquareUpAreas( void )
{
	int it = TheNavAreaList.Head();

	while( it != TheNavAreaList.InvalidIndex() )
	{
		CNavArea *area = TheNavAreaList[ it ];

		// move the iterator in case the current area is split and deleted
		it = TheNavAreaList.Next( it );

		if (!area->IsRoughlySquare())
		{
			// chop this area into square pieces
			if (area->GetSizeX() > area->GetSizeY())
				splitX( area );
			else
				splitY( area );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Start at given position and find first area in given direction
 */
inline CNavArea *findFirstAreaInDirection( const Vector *start, NavDirType dir, float range, float beneathLimit, CBaseEntity *traceIgnore = NULL, Vector *closePos = NULL )
{
	CNavArea *area = NULL;

	Vector pos = *start;

	int end = (int)((range / GenerationStepSize) + 0.5f);

	for( int i=1; i<=end; i++ )
	{
		AddDirectionVector( &pos, dir, GenerationStepSize );

		// make sure we dont look thru the wall
		trace_t result;

		UTIL_TraceLine( *start, pos, MASK_PLAYERSOLID_BRUSHONLY, traceIgnore, COLLISION_GROUP_NONE, &result );

		if (result.fraction != 1.0f)
			break;

		area = TheNavMesh->GetNavArea( &pos, beneathLimit );
		if (area)
		{
			if (closePos)
			{
				closePos->x = pos.x;
				closePos->y = pos.y;
				closePos->z = area->GetZ( pos.x, pos.y );
			}

			break;
		}
	}

	return area;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Determine if we can "jump down" from given point
 */
inline bool testJumpDown( const Vector *fromPos, const Vector *toPos )
{
	float dz = fromPos->z - toPos->z;

	// drop can't be too far, or too short (or nonexistant)
	if (dz <= JumpCrouchHeight || dz >= DeathDrop)
		return false;

	//
	// Check LOS out and down
	//
	// ------+
	//       |
	// F     |
	//       |
	//       T 
	//

	Vector from( fromPos->x, fromPos->y, fromPos->z + HumanHeight );
	Vector to( toPos->x, toPos->y, from.z );

	trace_t result;
	UTIL_TraceLine( from, to, MASK_PLAYERSOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &result );
	if (result.fraction != 1.0f || result.startsolid)
		return false;

	from = to;
	to.z = toPos->z + 2.0f;
	UTIL_TraceLine( from, to, MASK_PLAYERSOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &result );
	if (result.fraction != 1.0f || result.startsolid)
		return false;

	return true;
}


//--------------------------------------------------------------------------------------------------------------
inline CNavArea *findJumpDownArea( const Vector *fromPos, NavDirType dir )
{
	Vector start( fromPos->x, fromPos->y, fromPos->z + HalfHumanHeight );
	AddDirectionVector( &start, dir, GenerationStepSize/2.0f );

	Vector toPos;
	CNavArea *downArea = findFirstAreaInDirection( &start, dir, 4.0f * GenerationStepSize, DeathDrop, NULL, &toPos );

	if (downArea && testJumpDown( fromPos, &toPos ))
		return downArea;

	return NULL;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Define connections between adjacent generated areas
 */
void CNavMesh::ConnectGeneratedAreas( void )
{
	Msg( "Connecting navigation areas...\n" );

	FOR_EACH_LL( TheNavAreaList, it )
	{
		CNavArea *area = TheNavAreaList[ it ];

		// scan along edge nodes, stepping one node over into the next area
		// for now, only use bi-directional connections

		// north edge
		CNavNode *node;
		for( node = area->m_node[ NORTH_WEST ]; node != area->m_node[ NORTH_EAST ]; node = node->GetConnectedNode( EAST ) )
		{
			CNavNode *adj = node->GetConnectedNode( NORTH );

			if (adj && adj->GetArea() && adj->GetConnectedNode( SOUTH ) == node)
			{
				area->ConnectTo( adj->GetArea(), NORTH );
			}
			else
			{
				CNavArea *downArea = findJumpDownArea( node->GetPosition(), NORTH );
				if (downArea && downArea != area)
					area->ConnectTo( downArea, NORTH );
			}
		}

		// west edge
		for( node = area->m_node[ NORTH_WEST ]; node != area->m_node[ SOUTH_WEST ]; node = node->GetConnectedNode( SOUTH ) )
		{
			CNavNode *adj = node->GetConnectedNode( WEST );
			
			if (adj && adj->GetArea() && adj->GetConnectedNode( EAST ) == node)
			{
				area->ConnectTo( adj->GetArea(), WEST );
			}
			else
			{
				CNavArea *downArea = findJumpDownArea( node->GetPosition(), WEST );
				if (downArea && downArea != area)
					area->ConnectTo( downArea, WEST );
			}
		}

		// south edge - this edge's nodes are actually part of adjacent areas
		// move one node north, and scan west to east
		/// @todo This allows one-node-wide areas - do we want this?
		node = area->m_node[ SOUTH_WEST ];
		node = node->GetConnectedNode( NORTH );
		if (node)
		{
			CNavNode *end = area->m_node[ SOUTH_EAST ]->GetConnectedNode( NORTH );
			/// @todo Figure out why cs_backalley gets a NULL node in here...
			for( ; node && node != end; node = node->GetConnectedNode( EAST ) )
			{
				CNavNode *adj = node->GetConnectedNode( SOUTH );
				
				if (adj && adj->GetArea() && adj->GetConnectedNode( NORTH ) == node)
				{
					area->ConnectTo( adj->GetArea(), SOUTH );
				}
				else
				{
					CNavArea *downArea = findJumpDownArea( node->GetPosition(), SOUTH );
					if (downArea && downArea != area)
						area->ConnectTo( downArea, SOUTH );
				}
			}
		}

		// east edge - this edge's nodes are actually part of adjacent areas
		node = area->m_node[ NORTH_EAST ];
		node = node->GetConnectedNode( WEST );
		if (node)
		{
			CNavNode *end = area->m_node[ SOUTH_EAST ]->GetConnectedNode( WEST );
			for( ; node && node != end; node = node->GetConnectedNode( SOUTH ) )
			{
				CNavNode *adj = node->GetConnectedNode( EAST );
				
				if (adj && adj->GetArea() && adj->GetConnectedNode( WEST ) == node)
				{
					area->ConnectTo( adj->GetArea(), EAST );
				}
				else
				{
					CNavArea *downArea = findJumpDownArea( node->GetPosition(), EAST );
					if (downArea && downArea != area)
						area->ConnectTo( downArea, EAST );
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Merge areas together to make larger ones (must remain rectangular - convex).
 * Areas can only be merged if their attributes match.
 */
void CNavMesh::MergeGeneratedAreas( void )
{
	Msg( "Merging navigation areas...\n" );

	bool merged;

	do
	{
		merged = false;

		FOR_EACH_LL( TheNavAreaList, it )
		{
			CNavArea *area = TheNavAreaList[ it ];

			// north edge
			FOR_EACH_LL( area->m_connect[ NORTH ], nit )
			{
				CNavArea *adjArea = area->m_connect[ NORTH ][ nit ].area;

				if (area->m_node[ NORTH_WEST ] == adjArea->m_node[ SOUTH_WEST ] &&
					area->m_node[ NORTH_EAST ] == adjArea->m_node[ SOUTH_EAST ] &&
					area->GetAttributes() == adjArea->GetAttributes() &&
					area->IsCoplanar( adjArea ))
				{
					// merge vertical
					area->m_node[ NORTH_WEST ] = adjArea->m_node[ NORTH_WEST ];
					area->m_node[ NORTH_EAST ] = adjArea->m_node[ NORTH_EAST ];

					merged = true;
					//CONSOLE_ECHO( "  Merged (north) areas #%d and #%d\n", area->m_id, adjArea->m_id );

					area->FinishMerge( adjArea );

					// restart scan - iterator is invalidated
					break;
				}
			}

			if (merged)
				break;

			// south edge
			FOR_EACH_LL( area->m_connect[ SOUTH ], sit )
			{
				CNavArea *adjArea = area->m_connect[ SOUTH ][ sit ].area;

				if (adjArea->m_node[ NORTH_WEST ] == area->m_node[ SOUTH_WEST ] &&
					adjArea->m_node[ NORTH_EAST ] == area->m_node[ SOUTH_EAST ] &&
					area->GetAttributes() == adjArea->GetAttributes() &&
					area->IsCoplanar( adjArea ))
				{
					// merge vertical
					area->m_node[ SOUTH_WEST ] = adjArea->m_node[ SOUTH_WEST ];
					area->m_node[ SOUTH_EAST ] = adjArea->m_node[ SOUTH_EAST ];

					merged = true;
					//CONSOLE_ECHO( "  Merged (south) areas #%d and #%d\n", area->m_id, adjArea->m_id );

					area->FinishMerge( adjArea );

					// restart scan - iterator is invalidated
					break;
				}

			}

			if (merged)
				break;


			// west edge
			FOR_EACH_LL( area->m_connect[ WEST ], wit )
			{
				CNavArea *adjArea = area->m_connect[ WEST ][ wit ].area;

				if (area->m_node[ NORTH_WEST ] == adjArea->m_node[ NORTH_EAST ] &&
						area->m_node[ SOUTH_WEST ] == adjArea->m_node[ SOUTH_EAST ] &&
						area->GetAttributes() == adjArea->GetAttributes() &&
						area->IsCoplanar( adjArea ))
				{
					// merge horizontal
					area->m_node[ NORTH_WEST ] = adjArea->m_node[ NORTH_WEST ];
					area->m_node[ SOUTH_WEST ] = adjArea->m_node[ SOUTH_WEST ];

					merged = true;
					//CONSOLE_ECHO( "  Merged (west) areas #%d and #%d\n", area->m_id, adjArea->m_id );

					area->FinishMerge( adjArea );

					// restart scan - iterator is invalidated
					break;
				}

			}

			if (merged)
				break;

			// east edge
			FOR_EACH_LL( area->m_connect[ EAST ], eit )
			{
				CNavArea *adjArea = area->m_connect[ EAST ][ eit ].area;
				
				if (adjArea->m_node[ NORTH_WEST ] == area->m_node[ NORTH_EAST ] &&
					adjArea->m_node[ SOUTH_WEST ] == area->m_node[ SOUTH_EAST ] &&
					area->GetAttributes() == adjArea->GetAttributes() &&
					area->IsCoplanar( adjArea ))
				{
					// merge horizontal
					area->m_node[ NORTH_EAST ] = adjArea->m_node[ NORTH_EAST ];
					area->m_node[ SOUTH_EAST ] = adjArea->m_node[ SOUTH_EAST ];

					merged = true;
					//CONSOLE_ECHO( "  Merged (east) areas #%d and #%d\n", area->m_id, adjArea->m_id );

					area->FinishMerge( adjArea );

					// restart scan - iterator is invalidated
					break;
				}
			}

			if (merged)
				break;
		}
	}
	while( merged );
}


//--------------------------------------------------------------------------------------------------------------
/** 
 * Check if an rectangular area of the given size can be
 * made starting from the given node as the NW corner.
 * Only consider fully connected nodes for this check.
 * All of the nodes within the test area must have the same attributes.
 * All of the nodes must be approximately co-planar w.r.t the NW node's normal, with the
 * exception of 1x1 areas which can be any angle.
 */
bool CNavMesh::TestArea( CNavNode *node, int width, int height )
{
	Vector normal = *node->GetNormal();
	float d = -DotProduct( normal, *node->GetPosition() );

	const float offPlaneTolerance = 5.0f;

	CNavNode *vertNode, *horizNode;

	vertNode = node;
	for( int y=0; y<height; y++ )
	{
		horizNode = vertNode;

		for( int x=0; x<width; x++ )
		{
			// all nodes must have the same attributes
			if (horizNode->GetAttributes() != node->GetAttributes())
				return false;

			if (horizNode->IsCovered())
				return false;

			if (!horizNode->IsClosedCell())
				return false;

			horizNode = horizNode->GetConnectedNode( EAST );
			if (horizNode == NULL)
				return false;

			// nodes must lie on/near the plane
			if (width > 1 || height > 1)
			{
				float dist = (float)fabs( DotProduct( *horizNode->GetPosition(), normal ) + d );
				if (dist > offPlaneTolerance)
					return false;
			}					
		}

		vertNode = vertNode->GetConnectedNode( SOUTH );
		if (vertNode == NULL)
			return false;

		// nodes must lie on/near the plane
		if (width > 1 || height > 1)
		{
			float dist = (float)fabs( DotProduct( *vertNode->GetPosition(), normal ) + d );
			if (dist > offPlaneTolerance)
				return false;
		}					
	}

	// check planarity of southern edge
	if (width > 1 || height > 1)
	{
		horizNode = vertNode;

		for( int x=0; x<width; x++ )
		{
			horizNode = horizNode->GetConnectedNode( EAST );
			if (horizNode == NULL)
				return false;

			// nodes must lie on/near the plane
			float dist = (float)fabs( DotProduct( *horizNode->GetPosition(), normal ) + d );
			if (dist > offPlaneTolerance)
				return false;
		}
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------
/** 
 * Create a nav area, and mark all nodes it overlaps as "covered"
 * NOTE: Nodes on the east and south edges are not included.
 * Returns number of nodes covered by this area, or -1 for error;
 */
int CNavMesh::BuildArea( CNavNode *node, int width, int height )
{
	CNavNode *nwNode = node;
	CNavNode *neNode = NULL;
	CNavNode *swNode = NULL;
	CNavNode *seNode = NULL;

	CNavNode *vertNode = node;
	CNavNode *horizNode;

	int coveredNodes = 0;

	for( int y=0; y<height; y++ )
	{
		horizNode = vertNode;

		for( int x=0; x<width; x++ )
		{
			horizNode->Cover();
			++coveredNodes;
			
			horizNode = horizNode->GetConnectedNode( EAST );
		}

		if (y == 0)
			neNode = horizNode;

		vertNode = vertNode->GetConnectedNode( SOUTH );
	}

	swNode = vertNode;

	horizNode = vertNode;		
	for( int x=0; x<width; x++ )
	{
		horizNode = horizNode->GetConnectedNode( EAST );
	}
	seNode = horizNode;

	if (!nwNode || !neNode || !seNode || !swNode)
	{
		Error( "BuildArea - NULL node.\n" );
		return -1;
	}

	CNavArea *area = new CNavArea( nwNode, neNode, seNode, swNode );
	TheNavAreaList.AddToTail( area );

	// since all internal nodes have the same attributes, set this area's attributes
	area->SetAttributes( node->GetAttributes() );

	return coveredNodes;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * This function uses the CNavNodes that have been sampled from the map to
 * generate CNavAreas - rectangular areas of "walkable" space. These areas
 * are connected to each other, proving information on know how to move from
 * area to area.
 *
 * This is a "greedy" algorithm that attempts to cover the walkable area 
 * with the fewest, largest, rectangles.
 */
void CNavMesh::CreateNavAreasFromNodes( void )
{
	// haven't yet seen a map use larger than 30...
	int tryWidth = 50;
	int tryHeight = 50;
	int uncoveredNodes = CNavNode::GetListLength();

	while( uncoveredNodes > 0 )
	{
		for( CNavNode *node = CNavNode::GetFirst(); node; node = node->GetNext() )
		{
			if (node->IsCovered())
				continue;

			if (TestArea( node, tryWidth, tryHeight ))
			{
				int covered = BuildArea( node, tryWidth, tryHeight );
				if (covered < 0)
				{
					Error( "Generate: Error - Data corrupt.\n" );
					return;
				}

				uncoveredNodes -= covered;
			}
		}

		if (tryWidth >= tryHeight)
			--tryWidth;
		else
			--tryHeight;

		if (tryWidth <= 0 || tryHeight <= 0)
			break;
	}

	Extent extent;
	extent.lo.x = 9999999999.9f;
	extent.lo.y = 9999999999.9f;
	extent.hi.x = -9999999999.9f;
	extent.hi.y = -9999999999.9f;

	// compute total extent
	FOR_EACH_LL( TheNavAreaList, it )
	{
		CNavArea *area = TheNavAreaList[ it ];
		const Extent *areaExtent = area->GetExtent();

		if (areaExtent->lo.x < extent.lo.x)
			extent.lo.x = areaExtent->lo.x;
		if (areaExtent->lo.y < extent.lo.y)
			extent.lo.y = areaExtent->lo.y;
		if (areaExtent->hi.x > extent.hi.x)
			extent.hi.x = areaExtent->hi.x;
		if (areaExtent->hi.y > extent.hi.y)
			extent.hi.y = areaExtent->hi.y;
	}

	// add the areas to the grid
	AllocateGrid( extent.lo.x, extent.hi.x, extent.lo.y, extent.hi.y );

	FOR_EACH_LL( TheNavAreaList, git )
	{
		AddNavArea( TheNavAreaList[ git ] );
	}


	ConnectGeneratedAreas();
	MergeGeneratedAreas();
	SquareUpAreas();
	MarkJumpAreas();
}



//--------------------------------------------------------------------------------------------------------------
/**
 * Initiate the generation process
 */
void CNavMesh::BeginGeneration( void )
{
	m_generationState = SAMPLE_WALKABLE_SPACE;
	m_isGenerating = true;

	// clear any previous mesh
	DestroyNavigationMesh();

	// build internal representations of ladders, which are used to find new walkable areas
	BuildLadders();

	// start sampling from a spawn point
	CBaseEntity *spawn = gEntList.FindEntityByClassname( NULL, GetPlayerSpawnName() );

	if (spawn)
	{
		// snap it to the sampling grid
		Vector pos = spawn->GetAbsOrigin();
		pos.x = TheNavMesh->SnapToGrid( pos.x );
		pos.y = TheNavMesh->SnapToGrid( pos.y );

		Vector normal;
		if (GetGroundHeight( &pos, &pos.z, &normal ))
		{
			m_currentNode = new CNavNode( &pos, &normal, NULL );
		}
	}
	else
	{
		// the system will see this NULL and select the next walkable seed
		m_currentNode = NULL;
	}

	// if there are no seed points, we can't generate
	if (m_walkableSeedList.Count() == 0 && m_currentNode == NULL)
	{
		m_isGenerating = false;
		Msg( "No valid walkable seed positions.  Cannot generate Navigation Mesh.\n" );
		return;
	}

	// initialize seed list index
	m_seedIdx = m_walkableSeedList.Head();

	Msg( "Generating Navigation Mesh...\n" );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Process the auto-generation for 'maxTime' seconds. return false if generation is complete.
 */
bool CNavMesh::UpdateGeneration( float maxTime )
{
	double startTime = Plat_FloatTime();

	switch( m_generationState )
	{
		//---------------------------------------------------------------------------
		case SAMPLE_WALKABLE_SPACE:
		{
			Msg( "Sampling walkable space...\n" );

			while( Plat_FloatTime() - startTime < maxTime )
			{
				if (!SampleStep())
				{
					// sampling is complete, now wbuild nav areas
					m_generationState = CREATE_AREAS_FROM_SAMPLES;
					break;
				}
			}

			return true;
		}

		//---------------------------------------------------------------------------
		case CREATE_AREAS_FROM_SAMPLES:
		{
			Msg( "Creating navigation area from sampled data...\n" );

			CreateNavAreasFromNodes();
			DestroyHidingSpots();

			m_generationState = FIND_HIDING_SPOTS;
			return true;
		}

		//---------------------------------------------------------------------------
		case FIND_HIDING_SPOTS:
		{
			Msg( "Finding hiding spots...\n" );

			FOR_EACH_LL( TheNavAreaList, it )
			{
				CNavArea *area = TheNavAreaList[ it ];

				area->ComputeHidingSpots();

				// don't go over our time allotment
				if( Plat_FloatTime() - startTime > maxTime )
				{
					return true;
				}
			}

			m_generationState = FIND_APPROACH_AREAS;
			return true;
		}

		//---------------------------------------------------------------------------
		case FIND_APPROACH_AREAS:
		{
			Msg( "Finding approach areas...\n" );

			FOR_EACH_LL( TheNavAreaList, it )
			{
				CNavArea *area = TheNavAreaList[ it ];

				area->ComputeApproachAreas();

				// don't go over our time allotment
				if( Plat_FloatTime() - startTime > maxTime )
				{
					return true;
				}
			}

			m_generationState = FIND_ENCOUNTER_SPOTS;
			return true;
		}

		//---------------------------------------------------------------------------
		case FIND_ENCOUNTER_SPOTS:
		{
			Msg( "Finding encounter spots...\n" );

			FOR_EACH_LL( TheNavAreaList, it )
			{
				CNavArea *area = TheNavAreaList[ it ];

				area->ComputeSpotEncounters();

				// don't go over our time allotment
				if( Plat_FloatTime() - startTime > maxTime )
				{
					return true;
				}
			}

			m_generationState = FIND_SNIPER_SPOTS;
			return true;
		}

		//---------------------------------------------------------------------------
		case FIND_SNIPER_SPOTS:
		{
			Msg( "Finding sniper spots...\n" );

			FOR_EACH_LL( TheNavAreaList, it )
			{
				CNavArea *area = TheNavAreaList[ it ];

				area->ComputeSniperSpots();

				// don't go over our time allotment
				if( Plat_FloatTime() - startTime > maxTime )
				{
					return true;
				}
			}

			// generation complete!
			Msg( "Generation complete!\n" );
			m_isGenerating = false;

			// save the mesh
			if (Save())
			{
				Msg( "Navigation map '%s' saved.\n", GetFilename() );
			}
			else
			{
				const char *filename = GetFilename();
				Msg( "ERROR: Cannot save navigation map '%s'.\n", (filename) ? filename : "(null)" );
			}

			return false;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Define the name of player spawn entities
 */
void CNavMesh::SetPlayerSpawnName( const char *name )
{
	if (m_spawnName)
	{
		delete [] m_spawnName;
	}

	m_spawnName = new char [ strlen(name) + 1 ];
	strcpy( m_spawnName, name );
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Return name of player spawn entity
 */
const char *CNavMesh::GetPlayerSpawnName( void ) const
{
	if (m_spawnName)
		return m_spawnName;

	// default value
	return "info_player_start";
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Add a nav node and connect it.
 * Node Z positions are ground level.
 */
CNavNode *CNavMesh::AddNode( const Vector *destPos, const Vector *normal, NavDirType dir, CNavNode *source )
{
	// check if a node exists at this location
	CNavNode *node = CNavNode::GetNode( destPos );
	
	// if no node exists, create one
	bool useNew = false;
	if (node == NULL)
	{
		node = new CNavNode( destPos, normal, source );
		useNew = true;
	}

	// connect source node to new node
	source->ConnectTo( node, dir );

	// optimization: if deltaZ changes very little, assume connection is commutative
	const float zTolerance = 10.0f;
	if (fabs( source->GetPosition()->z - destPos->z ) < zTolerance)
	{
		node->ConnectTo( source, OppositeDirection( dir ) );
		node->MarkAsVisited( OppositeDirection( dir ) );
	}

	if (useNew)
	{
		// new node becomes current node
		m_currentNode = node;
	}

	// check if there is enough room to stand at this point, or if we need to crouch
	// check an area, not just a point
	trace_t result;
	Vector floor, ceiling;
	bool crouch = false;
	for( float y = -HalfHumanWidth; !crouch && y <= HalfHumanWidth + 0.1f; y += HalfHumanWidth )
	{
		for( float x = -HalfHumanWidth; x <= HalfHumanWidth + 0.1f; x += HalfHumanWidth )
		{
			floor.x = destPos->x + x;
			floor.y = destPos->y + y;
			floor.z = destPos->z + 5.0f;

			ceiling.x = floor.x;
			ceiling.y = floor.y;
			ceiling.z = destPos->z + HumanHeight - 0.1f;

			UTIL_TraceLine( floor, ceiling, MASK_PLAYERSOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &result );

			if (!result.startsolid && result.fraction != 1.0f)
			{
				crouch = true;
				break;
			}
		}
	}

	if (crouch)
	{
		node->SetAttributes( NAV_MESH_CROUCH );
	}

	return node;
}

//--------------------------------------------------------------------------------------------------------------
inline CNavNode *LadderEndSearch( CBaseEntity *entity, const Vector *pos, NavDirType mountDir )
{
	Vector center = *pos;
	AddDirectionVector( &center, mountDir, HalfHumanWidth );

	//
	// Test the ladder dismount point first, then each cardinal direction one and two steps away
	//
	for( int d=(-1); d<2*NUM_DIRECTIONS; ++d )
	{
		Vector tryPos = center;

		if (d >= NUM_DIRECTIONS)
			AddDirectionVector( &tryPos, (NavDirType)(d - NUM_DIRECTIONS), 2.0f*GenerationStepSize );
		else if (d >= 0)
			AddDirectionVector( &tryPos, (NavDirType)d, GenerationStepSize );

		// step up a rung, to ensure adjacent floors are below us
		tryPos.z += GenerationStepSize;

		tryPos.x = TheNavMesh->SnapToGrid( tryPos.x );
		tryPos.y = TheNavMesh->SnapToGrid( tryPos.y );

		// adjust height to account for sloping areas
		Vector tryNormal;
		if (TheNavMesh->GetGroundHeight( &tryPos, &tryPos.z, &tryNormal ) == false)
			continue;

		// make sure this point is not on the other side of a wall
		const float fudge = 2.0f;
		trace_t result;
		UTIL_TraceLine( center + Vector( 0, 0, fudge ), tryPos + Vector( 0, 0, fudge ), MASK_PLAYERSOLID_BRUSHONLY, entity, COLLISION_GROUP_NONE, &result );
		if (result.fraction != 1.0f)
			continue;

		// if no node exists here, create one and continue the search
		if (CNavNode::GetNode( &tryPos ) == NULL)
		{
			return new CNavNode( &tryPos, &tryNormal, NULL );
		}
	}

	return NULL;
}



//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

static int navAreaCount = 0;
static int currentIndex = 0;
const float updateTimesliceDuration = 0.5f;
extern int gmsgBotProgress;

// cheap-o progress meter
#define PROGRESS_TICKS 40
static void drawProgressMeter( char *title, float progress, char *vguiLabelText )
{
	char progressBuffer[ 256 ];

	int cursor = (int)(progress * (PROGRESS_TICKS-1));

	// don't redraw if cursor hasn't moved
	static int prevCursor = 99999;
	if (cursor == prevCursor)
		return;
	prevCursor = cursor;

	char *c = progressBuffer;

	if (title)
	{
		while(true)
		{
			if (*title == '\000')
				break;

			*c++ = *title++;
		}
		*c++ = '\n';
	}

	*c++ = '[';
	for( int i=0; i<PROGRESS_TICKS; ++i )
		*c++ = (i < cursor) ? '#' : '_';
	*c++ = ']';
	*c = '\000';

	//HintMessageToAllPlayers( progressBuffer );

	/* BOTPORT: Wire up learning progress bar

	MESSAGE_BEGIN( MSG_ALL, gmsgBotProgress );
		WRITE_BYTE( 0 ); // don't create/hide progress bar
		WRITE_BYTE( (int)(progress * 100) );
		WRITE_STRING( vguiLabelText );
	MESSAGE_END();
	*/
}

static void startProgressMeter( const char *title )
{
/*
	MESSAGE_BEGIN( MSG_ALL, gmsgBotProgress );
		WRITE_BYTE( 1 ); // create progress bar
		WRITE_STRING( title );
	MESSAGE_END();
*/
}

static void hideProgressMeter()
{
/*
	MESSAGE_BEGIN( MSG_ALL, gmsgBotProgress );
		WRITE_BYTE( 2 ); // hide progress bar
	MESSAGE_END();
*/
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Search the world and build a map of possible movements.
 * The algorithm begins at the bot's current location, and does a recursive search
 * outwards, tracking all valid steps and generating a directed graph of CNavNodes.
 *
 * Sample the map one "step" in a cardinal direction to learn the map.
 *
 * Returns true if sampling needs to continue, or false if done.
 */
bool CNavMesh::SampleStep( void )
{
	// take a step
	while( true )
	{
		if (m_currentNode == NULL)
		{
			// sampling is complete from current seed, try next one
			m_currentNode = GetNextWalkableSeedNode();

			if (m_currentNode == NULL)
			{
				// search is exhausted - continue search from ends of ladders
				FOR_EACH_LL( m_ladderList, it )
				{
					CNavLadder *ladder = m_ladderList[ it ];

					// check ladder bottom
					if ((m_currentNode = LadderEndSearch( ladder->m_entity, &ladder->m_bottom, ladder->m_dir )) != 0)
						break;

					// check ladder top
					if ((m_currentNode = LadderEndSearch( ladder->m_entity, &ladder->m_top, ladder->m_dir )) != 0)
						break;
				}

				if (m_currentNode == NULL)
				{
					// all seeds exhausted, sampling complete
					return false;
				}
			}
		}

		//
		// Take a step from this node
		//
		for( int dir = NORTH; dir < NUM_DIRECTIONS; dir++ )
		{
			if (!m_currentNode->HasVisited( (NavDirType)dir ))
			{
				// have not searched in this direction yet

				// start at current node position
				Vector pos = *m_currentNode->GetPosition();

				// snap to grid
				int cx = pos.x / GenerationStepSize;
				int cy = pos.y / GenerationStepSize;

				// attempt to move to adjacent node
				switch( dir )
				{
					case NORTH:		--cy; break;
					case SOUTH:		++cy; break;
					case EAST:		++cx; break;
					case WEST:		--cx; break;
				}

				pos.x = cx * GenerationStepSize;
				pos.y = cy * GenerationStepSize;

				m_generationDir = (NavDirType)dir;

				// mark direction as visited
				m_currentNode->MarkAsVisited( m_generationDir );

				// test if we can move to new position
				trace_t result;
				Vector from, to;

				// modify position to account for change in ground level during step
				to.x = pos.x;
				to.y = pos.y;
				Vector toNormal;
				if (GetGroundHeight( &pos, &to.z, &toNormal ) == false)
					return true;

				from = *m_currentNode->GetPosition();

				Vector fromOrigin = from + Vector( 0, 0, HalfHumanHeight );
				Vector toOrigin = to + Vector( 0, 0, HalfHumanHeight );

				UTIL_TraceLine( fromOrigin, toOrigin, MASK_PLAYERSOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &result );

				bool walkable;

				if (result.fraction == 1.0f && !result.startsolid)
				{
					// the trace didnt hit anything - clear

					float toGround = to.z;
					float fromGround = from.z;

					float epsilon = 0.1f;

					// check if ledge is too high to reach or will cause us to fall to our death
					if (toGround - fromGround > JumpCrouchHeight + epsilon || fromGround - toGround > DeathDrop)
					{
						walkable = false;
					}
					else
					{
						// check surface normals along this step to see if we would cross any impassable slopes
						Vector delta = to - from;
						const float inc = 2.0f;
						float along = inc;
						bool done = false;
						float ground;
						Vector normal;

						walkable = true;

						while( !done )
						{
							Vector p;

							// need to guarantee that we test the exact edges
							if (along >= GenerationStepSize)
							{
								p = to;
								done = true;
							}
							else
							{
								p = from + delta * (along/GenerationStepSize);
							}

							if (GetGroundHeight( &p, &ground, &normal ) == false)
							{
								walkable = false;
								break;
							}

							// check for maximum allowed slope
							// BOTPORT: Figure out why all traceline normals are zero
							if (normal.z < MaxUnitZSlope)
								walkable = false;

							along += inc;					
						}
					}
				}
				else	// TraceLine hit something...
				{
					if (IsEntityWalkable( result.m_pEnt, WALK_THRU_DOORS | WALK_THRU_BREAKABLES ))
					{
						walkable = true;
					}
					else
					{
						walkable = false;
					}
				}

				if (walkable)
				{
					// we can move here
					// create a new navigation node, and update current node pointer
					AddNode( &to, &toNormal, m_generationDir, m_currentNode );
				}

				return true;
			}
		}

		// all directions have been searched from this node - pop back to its parent and continue
		m_currentNode = m_currentNode->GetParent();
	}
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Add given walkable position to list of seed positions for map sampling
 */
void CNavMesh::AddWalkableSeed( const Vector &pos, const Vector &normal )
{
	WalkableSeedSpot seed;

	seed.pos = pos;
	seed.normal = normal;

	m_walkableSeedList.AddToTail( seed );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return the next walkable seed as a node
 */
CNavNode *CNavMesh::GetNextWalkableSeedNode( void )
{	
	if (!m_walkableSeedList.IsValidIndex( m_seedIdx ))
		return NULL;

	WalkableSeedSpot spot = m_walkableSeedList.Element( m_seedIdx );

	m_seedIdx = m_walkableSeedList.Next( m_seedIdx );

	return new CNavNode( &spot.pos, &spot.normal, NULL );
}
