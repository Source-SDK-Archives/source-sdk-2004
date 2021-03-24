//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// nav_mesh.h
// The Navigation Mesh interface
// Author: Michael S. Booth (mike@turtlerockstudios.com), January 2003

#ifndef _NAV_MESH_H_
#define _NAV_MESH_H_

#include "filesystem.h"

#include "nav.h"
#include "nav_area.h"


class CNavArea;
class CBaseEntity; 

extern ConVar nav_edit;
extern ConVar nav_quicksave;
extern ConVar nav_show_approach_points;
extern ConVar nav_show_danger;

enum NavEditCmdType
{
	EDIT_NONE,
	EDIT_DELETE,				///< delete current area
	EDIT_SPLIT,					///< split current area
	EDIT_MERGE,					///< merge adjacent areas
	EDIT_JOIN,					///< define connection between areas
	EDIT_BREAK,					///< break connection between areas
	EDIT_MARK,					///< mark an area for further operations
	EDIT_ATTRIB_CROUCH,			///< toggle crouch attribute on current area
	EDIT_ATTRIB_JUMP,			///< toggle jump attribute on current area
	EDIT_ATTRIB_PRECISE,		///< toggle precise attribute on current area
	EDIT_ATTRIB_NO_JUMP,		///< toggle inhibiting discontinuity jumping in current area
	EDIT_BEGIN_AREA,			///< begin creating a new nav area
	EDIT_END_AREA,				///< end creation of the new nav area
	EDIT_CONNECT,				///< connect marked area to selected area
	EDIT_DISCONNECT,			///< disconnect marked area from selected area
	EDIT_SPLICE,				///< create new area in between marked and selected areas
	EDIT_TOGGLE_PLACE_MODE,		///< switch between normal and place editing
	EDIT_TOGGLE_PLACE_PAINTING,	///< switch between "painting" places onto areas
	EDIT_PLACE_FLOODFILL,		///< floodfill areas out from current area
	EDIT_PLACE_PICK,			///< "pick up" the place at the current area
	EDIT_MARK_UNNAMED,			///< mark an unnamed area for further operations
	EDIT_WARP_TO_MARK,			///< warp a spectating local player to the selected mark
	EDIT_SELECT_CORNER,			///< select a corner on the current area
	EDIT_RAISE_CORNER,			///< raise a corner on the current area
	EDIT_LOWER_CORNER,			///< lower a corner on the current area
};

//--------------------------------------------------------------------------------------------------------
/**
 * The CNavMesh is the global interface to the Navigation Mesh.
 * @todo Make this an abstract base class interface, and derive mod-specific implementations.
 */
class CNavMesh
{
public:
	CNavMesh( void );
	virtual ~CNavMesh();

	void Reset( void );													///< destroy Navigation Mesh data and revert to initial state
	void Update( void );												///< invoked on each game frame

	NavErrorType Load( void );											///< load navigation data from a file
	bool IsLoaded( void ) const		{ return m_isLoaded; }				///< return true if a Navigation Mesh has been loaded
	bool Save( void ) const;											///< store Navigation Mesh to a file
	
	unsigned int GetNavAreaCount( void ) const	{ return m_areaCount; }	///< return total number of nav areas

	CNavArea *GetNavArea( const Vector *pos, float beneathLimt = 120.0f ) const;	///< given a position, return the nav area that IsOverlapping and is *immediately* beneath it
	CNavArea *GetNavAreaByID( unsigned int id ) const;
	CNavArea *GetNearestNavArea( const Vector *pos, bool anyZ = false ) const;

	Place GetPlace( const Vector *pos ) const;							///< return Place at given coordinate
	const char *PlaceToName( Place place ) const;						///< given a place, return its name
	Place NameToPlace( const char *name ) const;						///< given a place name, return a place ID or zero if no place is defined
	Place PartialNameToPlace( const char *name ) const;					///< given the first part of a place name, return a place ID or zero if no place is defined, or the partial match is ambiguous
	void PrintAllPlaces( void ) const;									///< output a list of names to the console

	bool GetGroundHeight( const Vector *pos, float *height, Vector *normal = NULL ) const;		///< get the Z coordinate of the topmost ground level below the given point
	bool GetSimpleGroundHeight( const Vector *pos, float *height, Vector *normal = NULL ) const;///< get the Z coordinate of the ground level directly below the given point


	/// increase "danger" weights in the given nav area and nearby ones
	void IncreaseDangerNearby( int teamID, float amount, CNavArea *area, const Vector *pos, float maxRadius );
	void DrawDanger( void ) const;										///< draw the current danger levels


	//-------------------------------------------------------------------------------------
	// Auto-generation
	//
	void BeginGeneration( void );										///< initiate the generation process
	bool IsGenerating( void ) const		{ return m_isGenerating; }		///< return true while a Navigation Mesh is being generated
	const char *GetPlayerSpawnName( void ) const;						///< return name of player spawn entity
	void SetPlayerSpawnName( const char *name );						///< define the name of player spawn entities
	void AddWalkableSeed( const Vector &pos, const Vector &normal );	///< add given walkable position to list of seed positions for map sampling
	void ClearWalkableSeeds( void )		{ m_walkableSeedList.RemoveAll(); }	///< erase all walkable seed positions

	//-------------------------------------------------------------------------------------
	// Edit mode
	//
	unsigned int GetNavPlace( void ) const			{ return m_navPlace; }
	void SetNavPlace( unsigned int place )			{ m_navPlace = place; }

	void Edit( NavEditCmdType cmd )					{ m_editCmd = cmd; }///< edit the mesh

	void SetMarkedArea( CNavArea *area )			{ m_markedArea = area; }	///< mark area for further edit operations
	CNavArea *GetMarkedArea( void ) const			{ return m_markedArea; }	///< return area marked by user in edit mode

	float SnapToGrid( float x ) const									///< snap given coordinate to generation grid boundary
	{
		int gx = (int)( x / GenerationStepSize );
		return gx * GenerationStepSize;
	}

	const Vector &GetEditCursorPosition( void ) const	{ return m_editCursorPos; }	///< return position of edit cursor
	void StripNavigationAreas( void );
	const char *GetFilename( void ) const;								///< return the filename for this map's "nav" file
							

	//-------------------------------------------------------------------------------------
	/**
	 * Apply the functor to all navigation areas.
	 * If functor returns false, stop processing and return false.
	 */
	template < typename Functor >
	bool ForAllAreas( Functor &func )
	{
		FOR_EACH_LL( TheNavAreaList, it )
		{
			CNavArea *area = TheNavAreaList[ it ];

			if (func( area ) == false)
				return false;
		}

		return true;
	}


private:
	friend class CNavArea;

	NavAreaList *m_grid;
	float m_gridCellSize;										///< the width/height of a grid cell for spatially partitioning nav areas for fast access
	int m_gridSizeX;
	int m_gridSizeY;
	float m_minX;
	float m_minY;
	unsigned int m_areaCount;									///< total number of nav areas

	bool m_isLoaded;											///< true if a Navigation Mesh has been loaded

	enum { HASH_TABLE_SIZE = 256 };
	CNavArea *m_hashTable[ HASH_TABLE_SIZE ];					///< hash table to optimize lookup by ID
	int ComputeHashKey( unsigned int id ) const;				///< returns a hash key for the given nav area ID

	int WorldToGridX( float wx ) const;							///< given X component, return grid index
	int WorldToGridY( float wy ) const;							///< given Y component, return grid index
	void AllocateGrid( float minX, float maxX, float minY, float maxY );	///< clear and reset the grid to the given extents

	void AddNavArea( CNavArea *area );							///< add an area to the grid
	void RemoveNavArea( CNavArea *area );						///< remove an area from the grid

	void DestroyNavigationMesh( void );							///< free all resources of the mesh and reset it to empty state
	void DestroyHidingSpots( void );


	//----------------------------------------------------------------------------------
	// Place directory
	//
	char **m_placeName;											///< master directory of place names (ie: "places")
	unsigned int m_placeCount;									///< number of "places" defined in placeName[]
	void LoadPlaceDatabase( void );								///< load the place names from a file

	//----------------------------------------------------------------------------------
	// Edit mode
	//
	NavEditCmdType m_editCmd;									///< edit command for editing the nav mesh
	unsigned int m_navPlace;									///< current navigation place for editing
	CNavArea *m_markedArea;										///< currently marked area for edit operations
	void EditReset( void );										///< REMOVE ME
	void UpdateEditMode( void );								///< draw navigation areas and edit them
	Vector m_editCursorPos;										///< current position of the cursor

	//----------------------------------------------------------------------------------
	// Auto-generation
	//
	bool UpdateGeneration( float maxTime = 0.25f );				///< process the auto-generation for 'maxTime' seconds. return false if generation is complete.

	CNavNode *m_currentNode;									///< the current node we are sampling from
	NavDirType m_generationDir;
	CNavNode *AddNode( const Vector *destPos, const Vector *destNormal, NavDirType dir, CNavNode *source );		///< add a nav node and connect it, update current node

	NavLadderList m_ladderList;									///< list of ladder navigation representations
	void BuildLadders( void );
	void DestroyLadders( void );

	bool SampleStep( void );									///< sample the walkable areas of the map
	void CreateNavAreasFromNodes( void );						///< cover all of the sampled nodes with nav areas

	bool TestArea( CNavNode *node, int width, int height );		///< check if an area of size (width, height) can fit, starting from node as upper left corner
	int BuildArea( CNavNode *node, int width, int height );		///< create a CNavArea of size (width, height) starting fom node at upper left corner

	void MarkJumpAreas( void );
	void SquareUpAreas( void );
	void MergeGeneratedAreas( void );
	void ConnectGeneratedAreas( void );

	enum GenerationStateType
	{
		SAMPLE_WALKABLE_SPACE,
		CREATE_AREAS_FROM_SAMPLES,
		FIND_HIDING_SPOTS,
		FIND_APPROACH_AREAS,
		FIND_ENCOUNTER_SPOTS,
		FIND_SNIPER_SPOTS,

		NUM_GENERATION_STATES
	}
	m_generationState;											///< the state of the generation process
	bool m_isGenerating;										///< true while a Navigation Mesh is being generated

	char *m_spawnName;											///< name of player spawn entity, used to initiate sampling

	struct WalkableSeedSpot
	{
		Vector pos;
		Vector normal;
	};
	CUtlLinkedList< WalkableSeedSpot, int > m_walkableSeedList;	///< list of walkable seed spots for sampling

	CNavNode *GetNextWalkableSeedNode( void );					///< return the next walkable seed as a node
	int m_seedIdx;
};

// the global singleton interface
extern CNavMesh *TheNavMesh;



//--------------------------------------------------------------------------------------------------------------
inline int CNavMesh::ComputeHashKey( unsigned int id ) const
{
	return id & 0xFF;
}

//--------------------------------------------------------------------------------------------------------------
inline int CNavMesh::WorldToGridX( float wx ) const
{ 
	int x = (int)( (wx - m_minX) / m_gridCellSize );

	if (x < 0)
		x = 0;
	else if (x >= m_gridSizeX)
		x = m_gridSizeX-1;
	
	return x;
}

//--------------------------------------------------------------------------------------------------------------
inline int CNavMesh::WorldToGridY( float wy ) const
{ 
	int y = (int)( (wy - m_minY) / m_gridCellSize );

	if (y < 0)
		y = 0;
	else if (y >= m_gridSizeY)
		y = m_gridSizeY-1;
	
	return y;
}


//--------------------------------------------------------------------------------------------------------------
//
// Function prototypes
//

extern void ApproachAreaAnalysisPrep( void );
extern void CleanupApproachAreaAnalysisPrep( void );

#endif // _NAV_MESH_H_
