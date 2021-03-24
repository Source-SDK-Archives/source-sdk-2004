//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// NavMesh.cpp
// Implementation of Navigation Mesh interface
// Author: Michael S. Booth, 2003-2004

#include "cbase.h"
#include "filesystem.h"
#include "nav_mesh.h"
#include "nav_node.h"

#define DrawLine( from, to, duration, red, green, blue )		NDebugOverlay::Line( from, to, red, green, blue, true, 0.1f )


/**
 * The singleton for accessing the navigation mesh
 */
CNavMesh *TheNavMesh = NULL;

ConVar nav_edit( "nav_edit", "0", 0, "Set to one to interactively edit the Navigation Mesh. Set to zero to leave edit mode." );
ConVar nav_quicksave( "nav_quicksave", "1", 0, "Set to one to skip the time consuming phases of the analysis.  Useful for data collection and testing." );
ConVar nav_show_approach_points( "nav_show_approach_points", "0", 0, "Show Approach Points in the Navigation Mesh." );
ConVar nav_show_danger( "nav_show_danger", "0", 0, "Show current 'danger' levels." );



//--------------------------------------------------------------------------------------------------------------
CNavMesh::CNavMesh( void )
{
	m_grid = NULL;
	m_spawnName = NULL;
	m_gridCellSize = 300.0f;

	LoadPlaceDatabase();

	Reset();
}

//--------------------------------------------------------------------------------------------------------------
CNavMesh::~CNavMesh()
{
	delete [] m_grid;
	m_grid = NULL;

	if (m_spawnName)
		delete [] m_spawnName;

	for( unsigned int i=0; i<m_placeCount; ++i )
	{
		delete [] m_placeName[i];
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Reset the Navigation Mesh to initial values
 */
void CNavMesh::Reset( void )
{
	DestroyNavigationMesh();

	m_isGenerating = false;
	m_currentNode = NULL;
	ClearWalkableSeeds();

	m_editCmd = EDIT_NONE;
	m_navPlace = UNDEFINED_PLACE;
	m_markedArea = NULL;
	EditReset(); // reset static vars

	if (m_spawnName)
		delete [] m_spawnName;

	m_spawnName = NULL;

	m_walkableSeedList.RemoveAll();
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Free all resources of the mesh and reset it to empty state
 */
void CNavMesh::DestroyNavigationMesh( void )
{
	// destroy all areas
	CNavArea::m_isReset = true;

		// remove each element of the list and delete them
		FOR_EACH_LL( TheNavAreaList, it )
		{
			delete TheNavAreaList[ it ];
		}

		TheNavAreaList.RemoveAll();

	CNavArea::m_isReset = false;


	// destroy ladder representations
	DestroyLadders();

	// destroy all hiding spots
	DestroyHidingSpots();

	// destroy navigation nodes created during map generation
	CNavNode *node, *next;
	for( node = CNavNode::m_list; node; node = next )
	{
		next = node->m_next;
		delete node;
	}
	CNavNode::m_list = NULL;

	// destroy the grid
	if (m_grid)
		delete [] m_grid;

	m_grid = NULL;
	m_gridSizeX = 0;
	m_gridSizeY = 0;

	// clear the hash table
	for( int i=0; i<HASH_TABLE_SIZE; ++i )
	{
		m_hashTable[i] = NULL;
	}

	m_areaCount = 0;

	m_isLoaded = false;

}


//--------------------------------------------------------------------------------------------------------------
/**
 * Invoked on each game frame
 */
void CNavMesh::Update( void )
{
	if (IsGenerating())
		UpdateGeneration();

	if (nav_edit.GetBool())
		UpdateEditMode();

	/// @todo Remove this nasty, nasty mechanism of issuing editing commands
	m_editCmd = EDIT_NONE;


	if (nav_show_danger.GetBool())
		DrawDanger();

	// draw any walkable seeds that have been marked
	FOR_EACH_LL( m_walkableSeedList, it )
	{
		WalkableSeedSpot spot = m_walkableSeedList[ it ];

		const float height = 50.0f;
		const float width = 25.0f;
		DrawLine( spot.pos, spot.pos + height * spot.normal, 3, 255, 0, 255 ); 
		DrawLine( spot.pos + Vector( width, 0, 0 ), spot.pos + height * spot.normal, 3, 255, 0, 255 ); 
		DrawLine( spot.pos + Vector( -width, 0, 0 ), spot.pos + height * spot.normal, 3, 255, 0, 255 ); 
		DrawLine( spot.pos + Vector( 0, width, 0 ), spot.pos + height * spot.normal, 3, 255, 0, 255 ); 
		DrawLine( spot.pos + Vector( 0, -width, 0 ), spot.pos + height * spot.normal, 3, 255, 0, 255 ); 
	}
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Allocate the grid and define its extents
 */
void CNavMesh::AllocateGrid( float minX, float maxX, float minY, float maxY )
{
	if (m_grid)
	{
		// destroy the grid
		if (m_grid)
			delete [] m_grid;

		m_grid = NULL;
	}

	m_minX = minX;
	m_minY = minY;

	m_gridSizeX = (int)((maxX - minX) / m_gridCellSize) + 1;
	m_gridSizeY = (int)((maxY - minY) / m_gridCellSize) + 1;

	m_grid = new NavAreaList[ m_gridSizeX * m_gridSizeY ];
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Add an area to the mesh
 */
void CNavMesh::AddNavArea( CNavArea *area )
{
	// add to grid
	const Extent *extent = area->GetExtent();

	int loX = WorldToGridX( extent->lo.x );
	int loY = WorldToGridY( extent->lo.y );
	int hiX = WorldToGridX( extent->hi.x );
	int hiY = WorldToGridY( extent->hi.y );

	for( int y = loY; y <= hiY; ++y )
	{
		for( int x = loX; x <= hiX; ++x )
		{
			m_grid[ x + y*m_gridSizeX ].AddToTail( const_cast<CNavArea *>( area ) );
		}
	}

	// add to hash table
	int key = ComputeHashKey( area->GetID() );

	if (m_hashTable[key])
	{
		// add to head of list in this slot
		area->m_prevHash = NULL;
		area->m_nextHash = m_hashTable[key];
		m_hashTable[key]->m_prevHash = area;
		m_hashTable[key] = area;
	}
	else
	{
		// first entry in this slot
		m_hashTable[key] = area;
		area->m_nextHash = NULL;
		area->m_prevHash = NULL;
	}

	++m_areaCount;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Remove an area from the mesh
 */
void CNavMesh::RemoveNavArea( CNavArea *area )
{
	// add to grid
	const Extent *extent = area->GetExtent();

	int loX = WorldToGridX( extent->lo.x );
	int loY = WorldToGridY( extent->lo.y );
	int hiX = WorldToGridX( extent->hi.x );
	int hiY = WorldToGridY( extent->hi.y );

	for( int y = loY; y <= hiY; ++y )
	{
		for( int x = loX; x <= hiX; ++x )
		{
			m_grid[ x + y*m_gridSizeX ].FindAndRemove( area );
		}
	}

	// remove from hash table
	int key = ComputeHashKey( area->GetID() );

	if (area->m_prevHash)
	{
		area->m_prevHash->m_nextHash = area->m_nextHash;
	}
	else
	{
		// area was at start of list
		m_hashTable[key] = area->m_nextHash;

		if (m_hashTable[key])
		{
			m_hashTable[key]->m_prevHash = NULL;
		}
	}

	if (area->m_nextHash)
	{
		area->m_nextHash->m_prevHash = area->m_prevHash;
	}

	--m_areaCount;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Given a position, return the nav area that IsOverlapping and is *immediately* beneath it
 */
CNavArea *CNavMesh::GetNavArea( const Vector *pos, float beneathLimit ) const
{
	if (m_grid == NULL)
		return NULL;

	// get list in cell that contains position
	int x = WorldToGridX( pos->x );
	int y = WorldToGridY( pos->y );
	NavAreaList *list = &m_grid[ x + y*m_gridSizeX ];


	// search cell list to find correct area
	CNavArea *use = NULL;
	float useZ = -99999999.9f;
	Vector testPos = *pos + Vector( 0, 0, 5 );

	FOR_EACH_LL( (*list), it )
	{
		CNavArea *area = (*list)[ it ];

		// check if position is within 2D boundaries of this area
		if (area->IsOverlapping( &testPos ))
		{
			// project position onto area to get Z
			float z = area->GetZ( &testPos );

			// if area is above us, skip it
			if (z > testPos.z)
				continue;

			// if area is too far below us, skip it
			if (z < pos->z - beneathLimit)
				continue;

			// if area is higher than the one we have, use this instead
			if (z > useZ)
			{
				use = area;
				useZ = z;
			}
		}
	}

	return use;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Given a position in the world, return the nav area that is closest
 * and at the same height, or beneath it.
 * Used to find initial area if we start off of the mesh.
 */
CNavArea *CNavMesh::GetNearestNavArea( const Vector *pos, bool anyZ ) const
{
	if (m_grid == NULL)
		return NULL;

	CNavArea *close = NULL;
	float closeDistSq = 99999999.9f;

	// quick check
	close = GetNavArea( pos );
	if (close)
	{
		return close;
	}

	// ensure source position is well behaved
	Vector source;
	source.x = pos->x;
	source.y = pos->y;
	if (GetGroundHeight( pos, &source.z ) == false)
	{
		return NULL;
	}

	source.z += HalfHumanHeight;

	/// @todo Step incrementally using grid for speed

	// find closest nav area
	FOR_EACH_LL( TheNavAreaList, it )
	{
		CNavArea *area = TheNavAreaList[ it ];

		Vector areaPos;
		area->GetClosestPointOnArea( &source, &areaPos );

		float distSq = (areaPos - source).LengthSqr();

		// keep the closest area
		if (distSq < closeDistSq)
		{
			// check LOS to area
			if (!anyZ)
			{
				trace_t result;
				UTIL_TraceLine( source, areaPos + Vector( 0, 0, HalfHumanHeight ), MASK_PLAYERSOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &result );
				if (result.fraction != 1.0f)
				{
					continue;
				}
			}
					
			closeDistSq = distSq;
			close = area;
		}
	}

	return close;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Given an ID, return the associated area
 */
CNavArea *CNavMesh::GetNavAreaByID( unsigned int id ) const
{
	if (id == 0)
		return NULL;

	int key = ComputeHashKey( id );

	for( CNavArea *area = m_hashTable[key]; area; area = area->m_nextHash )
	{
		if (area->GetID() == id)
		{
			return area;
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return radio chatter place for given coordinate
 */
unsigned int CNavMesh::GetPlace( const Vector *pos ) const
{
	CNavArea *area = GetNearestNavArea( pos, true );

	if (area)
	{
		return area->GetPlace();
	}

	return UNDEFINED_PLACE;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Load the place names from a file
 */
void CNavMesh::LoadPlaceDatabase( void )
{
	m_placeCount = 0;

	FileHandle_t file = filesystem->Open( "NavPlace.db", "r" );

	if (!file)
	{
		return;
	}

	const int maxNameLength = 128;
	char buffer[ maxNameLength ];

	// count the number of places
	while( true )
	{
		if (filesystem->ReadLine( buffer, maxNameLength, file ) == NULL)
			break;

		++m_placeCount;
	}

	// allocate place name array
	m_placeName = new char * [ m_placeCount ];


	// rewind and actually read the places
	filesystem->Seek( file, 0, FILESYSTEM_SEEK_HEAD );

	int i = 0;
	while( true )
	{
		if (filesystem->ReadLine( buffer, maxNameLength, file ) == NULL)
			break;

		int length = strlen( buffer );

		// remove trailing newline
		if (buffer[ length-1 ] == '\n')
			buffer[ length-1 ] = '\000';

		m_placeName[ i ] = new char [ strlen(buffer)+1 ];
		strcpy( m_placeName[ i ], buffer );
		++i;
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Given a place, return its name.
 * Reserve zero as invalid.
 */
const char *CNavMesh::PlaceToName( Place place ) const
{
	if (place > 1 && place <= m_placeCount)
		return m_placeName[ (int)place - 1 ];

	return NULL;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Given a place name, return a place ID or zero if no place is defined
 * Reserve zero as invalid.
 */
Place CNavMesh::NameToPlace( const char *name ) const
{
	for( unsigned int i=0; i<m_placeCount; ++i )
	{
		if (FStrEq( m_placeName[i], name ))
			return i+1;
	}

	return 0;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Given the first part of a place name, return a place ID or zero if no place is defined, or the partial match is ambiguous
 */
Place CNavMesh::PartialNameToPlace( const char *name ) const
{
	Place found = UNDEFINED_PLACE;
	bool isAmbiguous = false;
	for( unsigned int i=0; i<m_placeCount; ++i )
	{
		if (!strnicmp( m_placeName[i], name, strlen( name ) ))
		{
			// check for exact match in case of subsets of other strings
			if (!stricmp( m_placeName[i], name ))
			{
				found = NameToPlace( m_placeName[i] );
				isAmbiguous = false;
				break;
			}

			if (found != UNDEFINED_PLACE)
			{
				isAmbiguous = true;
			}
			else
			{
				found = NameToPlace( m_placeName[i] );
			}
		}
	}

	if (isAmbiguous)
		return UNDEFINED_PLACE;

	return found;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Output a list of names to the console
 */
void CNavMesh::PrintAllPlaces( void ) const
{
	if (m_placeCount == 0)
	{
		Msg( "There are no entries in the Place database.\n" );
		return;
	}

	int i=0;
	for( unsigned int it=0; it<m_placeCount; ++it )
	{
		if (NameToPlace( m_placeName[ it ] ) == GetNavPlace())
			Msg( "--> %-26s", m_placeName[ it ] );
		else
			Msg( "%-30s", m_placeName[ it ] );

		if (++i % 3 == 0)
			Msg( "\n" );
	}
	Msg( "\n" );
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Return the ground height below this point in "height".
 * Return false if position is invalid (outside of map, in a solid area, etc).
 */
bool CNavMesh::GetGroundHeight( const Vector *pos, float *height, Vector *normal ) const
{
	Vector to;
	to.x = pos->x;
	to.y = pos->y;
	to.z = pos->z - 9999.9f;

	float offset;
	Vector from;
	trace_t result;
	CBaseEntity *ignore = NULL;

	const float maxOffset = 100.0f;
	const float inc = 10.0f;

	#define MAX_GROUND_LAYERS 16
	struct GroundLayerInfo
	{
		float ground;
		Vector normal;
	}
	layer[ MAX_GROUND_LAYERS ];
	int layerCount = 0;

	for( offset = 1.0f; offset < maxOffset; offset += inc )
	{
		from = *pos + Vector( 0, 0, offset );

		UTIL_TraceLine( from, to, MASK_PLAYERSOLID_BRUSHONLY, ignore, COLLISION_GROUP_NONE, &result );

		// if the trace came down thru a door, ignore the door and try again
		// also ignore breakable floors
		if (result.DidHitNonWorldEntity() && result.m_pEnt)
		{
			if (FClassnameIs( result.m_pEnt, "prop_door" ) ||
				FClassnameIs( result.m_pEnt, "prop_door_rotating" ) ||
				FClassnameIs( result.m_pEnt, "func_breakable" ))// && VARS( result.pHit )->takedamage == DAMAGE_YES))
			{
				ignore = result.m_pEnt;
				// keep incrementing to avoid infinite loop if more than one entity is along the traceline...
				/// @todo Deal with multiple ignore entities in a single TraceLine()
				//offset -= inc;
				continue;
			}
		}

		if (result.startsolid == false)
		{
			// if we didnt start inside a solid area, the trace hit a ground layer

			// if this is a new ground layer, add it to the set
			if (layerCount == 0 || result.endpos.z > layer[ layerCount-1 ].ground)
			{
				layer[ layerCount ].ground = result.endpos.z;
				if (result.plane.normal.IsZero())
					layer[ layerCount ].normal = Vector( 0, 0, 1 );
				else
					layer[ layerCount ].normal = result.plane.normal;

				++layerCount;
						
				if (layerCount == MAX_GROUND_LAYERS)
					break;
			}
		}
	}

	if (layerCount == 0)
		return false;

	// find the lowest layer that allows a player to stand or crouch upon it
	int i;
	for( i=0; i<layerCount-1; ++i )
	{
		if (layer[i+1].ground - layer[i].ground >= HalfHumanHeight)
			break;		
	}

	*height = layer[ i ].ground;

	if (normal)
		*normal = layer[ i ].normal;

	return true;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return the "simple" ground height below this point in "height".
 * This function is much faster, but less tolerant. Make sure the give position is "well behaved".
 * Return false if position is invalid (outside of map, in a solid area, etc).
 */
bool CNavMesh::GetSimpleGroundHeight( const Vector *pos, float *height, Vector *normal ) const
{
	Vector to;
	to.x = pos->x;
	to.y = pos->y;
	to.z = pos->z - 9999.9f;

	trace_t result;

	UTIL_TraceLine( *pos, to, MASK_PLAYERSOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &result );

	if (result.startsolid)
		return false;

	*height = result.endpos.z;

	if (normal)
		*normal = result.plane.normal;

	return true;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Show danger levels for debugging
 */
void CNavMesh::DrawDanger( void ) const
{
	FOR_EACH_LL( TheNavAreaList, it )
	{
		CNavArea *area = TheNavAreaList[ it ];

		Vector center = *area->GetCenter();
		Vector top;
		center.z = area->GetZ( &center );

		float danger = area->GetDanger( 0 );
		if (danger > 0.1f)
		{
			top.x = center.x;
			top.y = center.y;
			top.z = center.z + 10.0f * danger;
			DrawLine( center, top, 3, 255, 0, 0 );
		}

		danger = area->GetDanger( 1 );
		if (danger > 0.1f)
		{
			top.x = center.x;
			top.y = center.y;
			top.z = center.z + 10.0f * danger;
			DrawLine( center, top, 3, 0, 0, 255 );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Increase the danger of nav areas containing and near the given position
 */
void CNavMesh::IncreaseDangerNearby( int teamID, float amount, CNavArea *startArea, const Vector *pos, float maxRadius )
{
	if (startArea == NULL)
		return;

	CNavArea::MakeNewMarker();
	CNavArea::ClearSearchLists();

	startArea->AddToOpenList();
	startArea->SetTotalCost( 0.0f );
	startArea->Mark();
	startArea->IncreaseDanger( teamID, amount );

	while( !CNavArea::IsOpenListEmpty() )
	{
		// get next area to check
		CNavArea *area = CNavArea::PopOpenList();
		
		// explore adjacent areas
		for( int dir=0; dir<NUM_DIRECTIONS; ++dir )
		{
			int count = area->GetAdjacentCount( (NavDirType)dir );
			for( int i=0; i<count; ++i )
			{
				CNavArea *adjArea = area->GetAdjacentArea( (NavDirType)dir, i );

				if (!adjArea->IsMarked())
				{
					// compute distance from danger source
					float cost = (*adjArea->GetCenter() - *pos).Length();
					if (cost <= maxRadius)
					{
						adjArea->AddToOpenList();
						adjArea->SetTotalCost( cost );
						adjArea->Mark();
						adjArea->IncreaseDanger( teamID, amount * cost/maxRadius );
					}
				}
			}
		}
	}
}


//--------------------------------------------------------------------------------------------------------------
void CommandNavDelete( void )
{
	TheNavMesh->Edit( EDIT_DELETE );
}
static ConCommand nav_delete( "nav_delete", CommandNavDelete, "Deletes the currently highlighted Area.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavSplit( void )
{
	TheNavMesh->Edit( EDIT_SPLIT );
}
static ConCommand nav_split( "nav_split", CommandNavSplit, "To split an Area into two, align the split line using your cursor and invoke the split command.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavMerge( void )
{
	TheNavMesh->Edit( EDIT_MERGE );
}
static ConCommand nav_merge( "nav_merge", CommandNavMerge, "To merge two Areas into one, mark the first Area, highlight the second by pointing your cursor at it, and invoke the merge command.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavMark( void )
{
	TheNavMesh->Edit( EDIT_MARK );
}
static ConCommand nav_mark( "nav_mark", CommandNavMark, "Marks the Area under the cursor for manipulation by subsequent editing commands.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavBeginArea( void )
{
	TheNavMesh->Edit( EDIT_BEGIN_AREA );
}
static ConCommand nav_begin_area( "nav_begin_area", CommandNavBeginArea, "Defines a corner of a new Area. To complete the Area, drag the opposite corner to the desired location and issue a 'nav_end_area' command.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavEndArea( void )
{
	TheNavMesh->Edit( EDIT_END_AREA );
}
static ConCommand nav_end_area( "nav_end_area", CommandNavEndArea, "Defines the second corner of a new Area and creates it.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavConnect( void )
{
	TheNavMesh->Edit( EDIT_CONNECT );
}
static ConCommand nav_connect( "nav_connect", CommandNavConnect, "To connect two Areas, mark the first Area, highlight the second Area, then invoke the connect command. Note that this creates a ONE-WAY connection from the first to the second Area. To make a two-way connection, also connect the second area to the first.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavDisconnect( void )
{
	TheNavMesh->Edit( EDIT_DISCONNECT );
}
static ConCommand nav_disconnect( "nav_disconnect", CommandNavDisconnect, "To disconnect two Areas, mark an Area, highlight a second Area, then invoke the disconnect command. This will remove all connections between the two Areas.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavSplice( void )
{
	TheNavMesh->Edit( EDIT_SPLICE );
}
static ConCommand nav_splice( "nav_splice", CommandNavSplice, "To splice, mark an area, highlight a second area, then invoke the splice command to create a new, connected area between them.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavCrouch( void )
{
	TheNavMesh->Edit( EDIT_ATTRIB_CROUCH );
}
static ConCommand nav_crouch( "nav_crouch", CommandNavCrouch, "Toggles the 'must crouch in this area' flag used by the AI system.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavPrecise( void )
{
	TheNavMesh->Edit( EDIT_ATTRIB_PRECISE );
}
static ConCommand nav_precise( "nav_precise", CommandNavPrecise, "Toggles the 'dont avoid obstacles' flag used by the AI system.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavJump( void )
{
	TheNavMesh->Edit( EDIT_ATTRIB_JUMP );
}
static ConCommand nav_jump( "nav_jump", CommandNavJump, "Toggles the 'traverse this area by jumping' flag used by the AI system.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavNoJump( void )
{
	TheNavMesh->Edit( EDIT_ATTRIB_NO_JUMP );
}
static ConCommand nav_no_jump( "nav_no_jump", CommandNavNoJump, "Toggles the 'dont jump in this area' flag used by the AI system.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavStrip( void )
{
	TheNavMesh->StripNavigationAreas();
}
static ConCommand nav_strip( "nav_strip", CommandNavStrip, "Strips all Hiding Spots, Approach Points, and Encounter Spots from the current Area.", FCVAR_GAMEDLL );


//--------------------------------------------------------------------------------------------------------------
void CommandNavSave( void )
{
	if (TheNavMesh->Save())
	{
		Msg( "Navigation map '%s' saved.\n", TheNavMesh->GetFilename() );
	}
	else
	{
		const char *filename = TheNavMesh->GetFilename();
		Msg( "ERROR: Cannot save navigation map '%s'.\n", (filename) ? filename : "(null)" );
	}
}
static ConCommand nav_save( "nav_save", CommandNavSave, "Saves the current Navigation Mesh to disk.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavLoad( void )
{
	if (TheNavMesh->Load() != NAV_OK)
	{
		Msg( "ERROR: Navigation Mesh load failed.\n" );
	}
}
static ConCommand nav_load( "nav_load", CommandNavLoad, "Loads the Navigation Mesh for the current map.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavUsePlace( void )
{
	if (engine->Cmd_Argc() == 1)
	{
		// no arguments = list all available places
		TheNavMesh->PrintAllPlaces();
	}
	else
	{
		// single argument = set current place
		Place place = TheNavMesh->PartialNameToPlace( engine->Cmd_Argv( 1 ) );

		if (place == UNDEFINED_PLACE)
		{
			Msg( "Ambiguous\n" );
		}
		else
		{
			Msg( "Current place set to '%s'\n", TheNavMesh->PlaceToName( place ) );
			TheNavMesh->SetNavPlace( place );
		}
	}
}
static ConCommand nav_use_place( "nav_use_place", CommandNavUsePlace, "If used without arguments, all available Places will be listed. If a Place argument is given, the current Place is set.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavTogglePlaceMode( void )
{
	TheNavMesh->Edit( EDIT_TOGGLE_PLACE_MODE );
}
static ConCommand nav_toggle_place_mode( "nav_toggle_place_mode", CommandNavTogglePlaceMode, "Toggle the editor into and out of Place mode. Place mode allows labelling of Area with Place names.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavPlaceFloodFill( void )
{
	TheNavMesh->Edit( EDIT_PLACE_FLOODFILL );
}
static ConCommand nav_place_floodfill( "nav_place_floodfill", CommandNavPlaceFloodFill, "Sets the Place of the Area under the cursor to the curent Place, and 'flood-fills' the Place to all adjacent Areas. Flood-filling stops when it hits an Area with the same Place, or a different Place than that of the initial Area.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavPlacePick( void )
{
	TheNavMesh->Edit( EDIT_PLACE_PICK );
}
static ConCommand nav_place_pick( "nav_place_pick", CommandNavPlacePick, "Sets the current Place to the Place of the Area under the cursor.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavTogglePlacePainting( void )
{
	TheNavMesh->Edit( EDIT_TOGGLE_PLACE_PAINTING );
}
static ConCommand nav_toggle_place_painting( "nav_toggle_place_painting", CommandNavTogglePlacePainting, "Toggles Place Painting mode. When Place Painting, pointing at an Area will 'paint' it with the current Place.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavMarkUnnamed( void )
{
	TheNavMesh->Edit( EDIT_MARK_UNNAMED );
}
static ConCommand nav_mark_unnamed( "nav_mark_unnamed", CommandNavMarkUnnamed, "Mark an Area with no Place name. Useful for finding stray areas missed when Place Painting.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavCornerSelect( void )
{
	TheNavMesh->Edit( EDIT_SELECT_CORNER );
}
static ConCommand nav_corner_select( "nav_corner_select", CommandNavCornerSelect, "Select a corner of the currently marked Area. Use multiple times to access all four corners.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavCornerRaise( void )
{
	TheNavMesh->Edit( EDIT_RAISE_CORNER );
}
static ConCommand nav_corner_raise( "nav_corner_raise", CommandNavCornerRaise, "Raise the selected corner of the currently marked Area.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavCornerLower( void )
{
	TheNavMesh->Edit( EDIT_LOWER_CORNER );
}
static ConCommand nav_corner_lower( "nav_corner_lower", CommandNavCornerLower, "Lower the selected corner of the currently marked Area.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavGenerate( void )
{
	TheNavMesh->BeginGeneration();
}
static ConCommand nav_generate( "nav_generate", CommandNavGenerate, "Generate a Navigation Mesh for the current map and save it to disk.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
void CommandNavMarkWalkable( void )
{
	Vector pos;

	if (nav_edit.GetBool())
	{
		// we are in edit mode, use the edit cursor's location
		pos = TheNavMesh->GetEditCursorPosition();
	}
	else
	{
		// we are not in edit mode, use the position of the local player
		CBasePlayer *player = UTIL_GetListenServerHost();

		if (player == NULL)
		{
			Msg( "ERROR: No local player!\n" );
			return;
		}

		pos = player->GetAbsOrigin();
	}

	// snap position to the sampling grid
	pos.x = TheNavMesh->SnapToGrid( pos.x );
	pos.y = TheNavMesh->SnapToGrid( pos.y );

	Vector normal;
	if (TheNavMesh->GetGroundHeight( &pos, &pos.z, &normal ) == false)
	{
		Msg( "ERROR: Invalid ground position.\n" );
		return;
	}

	TheNavMesh->AddWalkableSeed( pos, normal );

	Msg( "Walkable position marked.\n" );
}
static ConCommand nav_mark_walkable( "nav_mark_walkable", CommandNavMarkWalkable, "Mark the current location as a walkable position. These positions are used as seed locations when sampling the map to generate a Navigation Mesh.", FCVAR_GAMEDLL );


//--------------------------------------------------------------------------------------------------------------
void CommandNavClearWalkableMarks( void )
{
	TheNavMesh->ClearWalkableSeeds();
}
static ConCommand nav_clear_walkable_marks( "nav_clear_walkable_marks", CommandNavClearWalkableMarks, "Erase any previously placed walkable positions.", FCVAR_GAMEDLL );

//--------------------------------------------------------------------------------------------------------------
/**
 * Destroy ladder representations
 */
void CNavMesh::DestroyLadders( void )
{
	FOR_EACH_LL( m_ladderList, it )
	{
		delete m_ladderList[ it ];
	}

	m_ladderList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Strip the "analyzed" data out of all navigation areas
 */
void CNavMesh::StripNavigationAreas( void )
{
	FOR_EACH_LL( TheNavAreaList, it )
	{
		CNavArea *area = TheNavAreaList[ it ];

		area->Strip();
	}
}

//--------------------------------------------------------------------------------------------------------------

HidingSpotList TheHidingSpotList;
unsigned int HidingSpot::m_nextID = 1;
unsigned int HidingSpot::m_masterMarker = 0;

void CNavMesh::DestroyHidingSpots( void )
{
	// remove all hiding spot references from the nav areas
	FOR_EACH_LL( TheNavAreaList, it )
	{
		CNavArea *area = TheNavAreaList[ it ];

		area->m_hidingSpotList.RemoveAll();
	}

	HidingSpot::m_nextID = 0;

	// free all the HidingSpots
	FOR_EACH_LL( TheHidingSpotList, hit )
	{
		delete TheHidingSpotList[ hit ];
	}

	TheHidingSpotList.RemoveAll();
}

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
/**
 * For use when loading from a file
 */
HidingSpot::HidingSpot( void )
{
	m_pos = Vector( 0, 0, 0 );
	m_id = 0;
	m_flags = 0;

	TheHidingSpotList.AddToTail( this );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * For use when generating - assigns unique ID
 */
HidingSpot::HidingSpot( const Vector *pos, int flags )
{
	m_pos = *pos;
	m_id = m_nextID++;
	m_flags = (unsigned char)flags;

	TheHidingSpotList.AddToTail( this );
}

//--------------------------------------------------------------------------------------------------------------
void HidingSpot::Save( FileHandle_t file, unsigned int version ) const
{
	filesystem->Write( &m_id, sizeof(unsigned int), file );
	filesystem->Write( &m_pos, 3 * sizeof(float), file );
	filesystem->Write( &m_flags, sizeof(unsigned char), file );
}

//--------------------------------------------------------------------------------------------------------------
void HidingSpot::Load( FileHandle_t file, unsigned int version )
{
	filesystem->Read( &m_id, sizeof(unsigned int), file );
	filesystem->Read( &m_pos, 3 * sizeof(float), file );
	filesystem->Read( &m_flags, sizeof(unsigned char), file );

	// update next ID to avoid ID collisions by later spots
	if (m_id >= m_nextID)
		m_nextID = m_id+1;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Given a HidingSpot ID, return the associated HidingSpot
 */
HidingSpot *GetHidingSpotByID( unsigned int id )
{
	FOR_EACH_LL( TheHidingSpotList, it )
	{
		HidingSpot *spot = TheHidingSpotList[ it ];

		if (spot->GetID() == id)
			return spot;
	}	

	return NULL;
}

