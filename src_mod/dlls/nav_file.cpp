//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// nav_file.cpp
// Reading and writing nav files
// Author: Michael S. Booth (mike@turtlerockstudios.com), January-September 2003

#include "cbase.h"
#include "nav_mesh.h"

//--------------------------------------------------------------------------------------------------------------
//
// The 'place directory' is used to save and load places from
// nav files in a size-efficient manner that also allows for the 
// order of the place ID's to change without invalidating the
// nav files.
//
// The place directory is stored in the nav file as a list of 
// place name strings.  Each nav area then contains an index
// into that directory, or zero if no place has been assigned to 
// that area.
//
class PlaceDirectory
{
public:

	typedef unsigned short IndexType;

	void Reset( void )
	{
		m_directory.RemoveAll();
	}

	/// return true if this place is already in the directory
	bool IsKnown( Place place ) const
	{
		return m_directory.HasElement( place );
	}

	/// return the directory index corresponding to this Place (0 = no entry)
	IndexType GetIndex( Place place ) const
	{
		if (place == UNDEFINED_PLACE)
			return 0;

		int i = m_directory.Find( place );

		if (i < 0)
		{
			assert( false && "PlaceDirectory::GetIndex failure" );
			return 0;
		}

		return (IndexType)i;
	}

	/// add the place to the directory if not already known
	void AddPlace( Place place )
	{
		if (place == UNDEFINED_PLACE)
			return;

		assert( place < 1000 );

		if (IsKnown( place ))
			return;

		m_directory.AddToTail( place );
	}

	/// given an index, return the Place
	Place IndexToPlace( IndexType entry ) const
	{
		if (entry == 0)
			return UNDEFINED_PLACE;

		int i = entry-1;

		if (i > m_directory.Count())
		{
			assert( false && "PlaceDirectory::IndexToPlace: Invalid entry" );
			return UNDEFINED_PLACE;
		}

		return m_directory[ i ];
	}

	/// store the directory
	void Save( FileHandle_t file )
	{
		// store number of entries in directory
		IndexType count = (IndexType)m_directory.Count();
		filesystem->Write( &count, sizeof(IndexType), file );

		// store entries		
		for( int i=0; i<m_directory.Count(); ++i )
		{
			const char *placeName = TheNavMesh->PlaceToName( m_directory[i] );

			// store string length followed by string itself
			unsigned short len = (unsigned short)(strlen( placeName ) + 1);
			filesystem->Write( &len, sizeof(unsigned short), file );
			filesystem->Write( placeName, len, file );
		}
	}

	/// load the directory
	void Load( FileHandle_t file )
	{
		// read number of entries
		IndexType count;
		filesystem->Read( &count, sizeof(IndexType), file );

		m_directory.EnsureCount( count );

		// read each entry
		char placeName[256];
		unsigned short len;
		for( int i=0; i<count; ++i )
		{
			filesystem->Read( &len, sizeof(unsigned short), file );
			filesystem->Read( placeName, len, file );

			AddPlace( TheNavMesh->NameToPlace( placeName ) );
		}
	}

private:
	CUtlVector< Place > m_directory;
};

static PlaceDirectory placeDirectory;


//--------------------------------------------------------------------------------------------------------------
/**
 * Replace extension with "bsp"
 */
char *GetBspFilename( const char *navFilename )
{
	static char bspFilename[256];

	Q_snprintf( bspFilename, sizeof( bspFilename ), "maps\\%s.bsp", STRING( gpGlobals->mapname ) );

	int len = strlen( bspFilename );
	if (len < 3)
		return NULL;

	bspFilename[ len-3 ] = 'b';
	bspFilename[ len-2 ] = 's';
	bspFilename[ len-1 ] = 'p';

	return bspFilename;
}

//--------------------------------------------------------------------------------------------------------------
/*
void CNavArea::Save( FILE *fp ) const
{
	fprintf( fp, "v  %f %f %f\n", m_extent.lo.x, m_extent.lo.y, m_extent.lo.z );
	fprintf( fp, "v  %f %f %f\n", m_extent.hi.x, m_extent.lo.y, m_neZ );
	fprintf( fp, "v  %f %f %f\n", m_extent.hi.x, m_extent.hi.y, m_extent.hi.z );
	fprintf( fp, "v  %f %f %f\n", m_extent.lo.x, m_extent.hi.y, m_swZ );

	static int base = 1;
	fprintf( fp, "\n\ng %04dArea%s%s%s%s\n", m_id, 
				(GetAttributes() & BOT_NAV_CROUCH) ? "CROUCH" : "",
				(GetAttributes() & BOT_NAV_JUMP) ? "JUMP" : "",
				(GetAttributes() & BOT_NAV_PRECISE) ? "PRECISE" : "",
				(GetAttributes() & BOT_NAV_NO_JUMP) ? "NO_JUMP" : "" );
	fprintf( fp, "f %d %d %d %d\n\n", base, base+1, base+2, base+3 );
	base += 4;
}
*/

//--------------------------------------------------------------------------------------------------------------
/**
 * Save a navigation area to the opened binary stream
 */
void CNavArea::Save( FileHandle_t file, unsigned int version ) const
{
	// save ID
	filesystem->Write( &m_id, sizeof(unsigned int), file );

	// save attribute flags
	filesystem->Write( &m_attributeFlags, sizeof(unsigned char), file );

	// save extent of area
	filesystem->Write( &m_extent, 6*sizeof(float), file );

	// save heights of implicit corners
	filesystem->Write( &m_neZ, sizeof(float), file );
	filesystem->Write( &m_swZ, sizeof(float), file );

	// save connections to adjacent areas
	// in the enum order NORTH, EAST, SOUTH, WEST
	for( int d=0; d<NUM_DIRECTIONS; d++ )
	{
		// save number of connections for this direction
		unsigned int count = m_connect[d].Count();
		filesystem->Write( &count, sizeof(unsigned int), file );

		FOR_EACH_LL( m_connect[d], it )
		{
			NavConnect connect = m_connect[d][ it ];
			filesystem->Write( &connect.area->m_id, sizeof(unsigned int), file );
		}
	}

	//
	// Store hiding spots for this area
	//
	unsigned char count;
	if (m_hidingSpotList.Count() > 255)
	{
		count = 255;
		Warning( "Warning: NavArea #%d: Truncated hiding spot list to 255\n", m_id );
	}
	else
	{
		count = (unsigned char)m_hidingSpotList.Count();
	}
	filesystem->Write( &count, sizeof(unsigned char), file );

	// store HidingSpot objects
	unsigned int saveCount = 0;
	FOR_EACH_LL( m_hidingSpotList, hit )
	{
		HidingSpot *spot = m_hidingSpotList[ hit ];
		
		spot->Save( file, version );

		// overflow check
		if (++saveCount == count)
			break;
	}

	//
	// Save the approach areas for this area
	//

	// save number of approach areas
	filesystem->Write( &m_approachCount, sizeof(unsigned char), file );

	// save approach area info
	unsigned char type;
	unsigned int zero = 0;
	for( int a=0; a<m_approachCount; ++a )
	{
		if (m_approach[a].here.area)
			filesystem->Write( &m_approach[a].here.area->m_id, sizeof(unsigned int), file );
		else
			filesystem->Write( &zero, sizeof(unsigned int), file );

		if (m_approach[a].prev.area)
			filesystem->Write( &m_approach[a].prev.area->m_id, sizeof(unsigned int), file );
		else
			filesystem->Write( &zero, sizeof(unsigned int), file );
		type = (unsigned char)m_approach[a].prevToHereHow;
		filesystem->Write( &type, sizeof(unsigned char), file );

		if (m_approach[a].next.area)
			filesystem->Write( &m_approach[a].next.area->m_id, sizeof(unsigned int), file );
		else
			filesystem->Write( &zero, sizeof(unsigned int), file );
		type = (unsigned char)m_approach[a].hereToNextHow;
		filesystem->Write( &type, sizeof(unsigned char), file );
	}

	//
	// Save encounter spots for this area
	//
	{
		// save number of encounter paths for this area
		unsigned int count = m_spotEncounterList.Count();
		filesystem->Write( &count, sizeof(unsigned int), file );

		SpotEncounter *e;
		FOR_EACH_LL( m_spotEncounterList, it )
		{
			e = m_spotEncounterList[ it ];

			if (e->from.area)
				filesystem->Write( &e->from.area->m_id, sizeof(unsigned int), file );
			else
				filesystem->Write( &zero, sizeof(unsigned int), file );

			unsigned char dir = (unsigned char)e->fromDir;
			filesystem->Write( &dir, sizeof(unsigned char), file );

			if (e->to.area)
				filesystem->Write( &e->to.area->m_id, sizeof(unsigned int), file );
			else
				filesystem->Write( &zero, sizeof(unsigned int), file );

			dir = (unsigned char)e->toDir;
			filesystem->Write( &dir, sizeof(unsigned char), file );

			// write list of spots along this path
			unsigned char spotCount;
			if (e->spotList.Count() > 255)
			{
				spotCount = 255;
				Warning( "Warning: NavArea #%d: Truncated encounter spot list to 255\n", m_id );
			}
			else
			{
				spotCount = (unsigned char)e->spotList.Count();
			}
			filesystem->Write( &spotCount, sizeof(unsigned char), file );
		
			saveCount = 0;
			FOR_EACH_LL( e->spotList, sit )
			{
				SpotOrder *order = &e->spotList[ sit ];

				// order->spot may be NULL if we've loaded a nav mesh that has been edited but not re-analyzed
				unsigned int id = (order->spot) ? order->spot->GetID() : 0;
				filesystem->Write( &id, sizeof(unsigned int), file );

				unsigned char t = (unsigned char)(255 * order->t);
				filesystem->Write( &t, sizeof(unsigned char), file );

				// overflow check
				if (++saveCount == spotCount)
					break;
			}
		}
	}

	// store place dictionary entry
	PlaceDirectory::IndexType entry = placeDirectory.GetIndex( GetPlace() );
	filesystem->Write( &entry, sizeof(entry), file );

}

//--------------------------------------------------------------------------------------------------------------
/**
 * Load a navigation area from the file
 */
void CNavArea::Load( FileHandle_t file, unsigned int version )
{
	// load ID
	filesystem->Read( &m_id, sizeof(unsigned int), file );

	// update nextID to avoid collisions
	if (m_id >= m_nextID)
		m_nextID = m_id+1;

	// load attribute flags
	filesystem->Read( &m_attributeFlags, sizeof(unsigned char), file );

	// load extent of area
	filesystem->Read( &m_extent, 6*sizeof(float), file );

	m_center.x = (m_extent.lo.x + m_extent.hi.x)/2.0f;
	m_center.y = (m_extent.lo.y + m_extent.hi.y)/2.0f;
	m_center.z = (m_extent.lo.z + m_extent.hi.z)/2.0f;

	// load heights of implicit corners
	filesystem->Read( &m_neZ, sizeof(float), file );
	filesystem->Read( &m_swZ, sizeof(float), file );

	// load connections (IDs) to adjacent areas
	// in the enum order NORTH, EAST, SOUTH, WEST
	for( int d=0; d<NUM_DIRECTIONS; d++ )
	{
		// load number of connections for this direction
		unsigned int count;
		int result = filesystem->Read( &count, sizeof(unsigned int), file );
		Assert( result == sizeof(unsigned int) );

		for( unsigned int i=0; i<count; ++i )
		{
			NavConnect connect;
			result = filesystem->Read( &connect.id, sizeof(unsigned int), file );
			Assert( result == sizeof(unsigned int) );

			m_connect[d].AddToTail( connect );
		}
	}

	//
	// Load hiding spots
	//

	// load number of hiding spots
	unsigned char hidingSpotCount;
	filesystem->Read( &hidingSpotCount, sizeof(unsigned char), file );

	if (version == 1)
	{
		// load simple vector array
		Vector pos;
		for( int h=0; h<hidingSpotCount; ++h )
		{
			filesystem->Read( &pos, 3 * sizeof(float), file );

			// create new hiding spot and put on master list
			HidingSpot *spot = new HidingSpot( &pos, HidingSpot::IN_COVER );

			m_hidingSpotList.AddToTail( spot );
		}
	}
	else
	{
		// load HidingSpot objects for this area
		for( int h=0; h<hidingSpotCount; ++h )
		{
			// create new hiding spot and put on master list
			HidingSpot *spot = new HidingSpot;

			spot->Load( file, version );
			
			m_hidingSpotList.AddToTail( spot );
		}
	}

	//
	// Load number of approach areas
	//
	filesystem->Read( &m_approachCount, sizeof(unsigned char), file );

	// load approach area info (IDs)
	unsigned char type;
	for( int a=0; a<m_approachCount; ++a )
	{
		filesystem->Read( &m_approach[a].here.id, sizeof(unsigned int), file );

		filesystem->Read( &m_approach[a].prev.id, sizeof(unsigned int), file );
		filesystem->Read( &type, sizeof(unsigned char), file );
		m_approach[a].prevToHereHow = (NavTraverseType)type;

		filesystem->Read( &m_approach[a].next.id, sizeof(unsigned int), file );
		filesystem->Read( &type, sizeof(unsigned char), file );
		m_approach[a].hereToNextHow = (NavTraverseType)type;
	}


	//
	// Load encounter paths for this area
	//
	unsigned int count;
	filesystem->Read( &count, sizeof(unsigned int), file );

	if (version < 3)
	{
		// old data, read and discard
		for( unsigned int e=0; e<count; ++e )
		{
			SpotEncounter encounter;

			filesystem->Read( &encounter.from.id, sizeof(unsigned int), file );
			filesystem->Read( &encounter.to.id, sizeof(unsigned int), file );

			filesystem->Read( &encounter.path.from.x, 3 * sizeof(float), file );
			filesystem->Read( &encounter.path.to.x, 3 * sizeof(float), file );

			// read list of spots along this path
			unsigned char spotCount;
			filesystem->Read( &spotCount, sizeof(unsigned char), file );
		
			for( int s=0; s<spotCount; ++s )
			{
				Vector pos;
				filesystem->Read( &pos, 3*sizeof(float), file );
				filesystem->Read( &pos, sizeof(float), file );
			}
		}
		return;
	}

	for( unsigned int e=0; e<count; ++e )
	{
		SpotEncounter *encounter = new SpotEncounter;

		filesystem->Read( &encounter->from.id, sizeof(unsigned int), file );

		unsigned char dir;
		filesystem->Read( &dir, sizeof(unsigned char), file );
		encounter->fromDir = static_cast<NavDirType>( dir );

		filesystem->Read( &encounter->to.id, sizeof(unsigned int), file );

		filesystem->Read( &dir, sizeof(unsigned char), file );
		encounter->toDir = static_cast<NavDirType>( dir );

		// read list of spots along this path
		unsigned char spotCount;
		filesystem->Read( &spotCount, sizeof(unsigned char), file );
	
		SpotOrder order;
		for( int s=0; s<spotCount; ++s )
		{
			filesystem->Read( &order.id, sizeof(unsigned int), file );

			unsigned char t;
			filesystem->Read( &t, sizeof(unsigned char), file );

			order.t = (float)t/255.0f;

			encounter->spotList.AddToTail( order );
		}

		m_spotEncounterList.AddToTail( encounter );
	}

	if (version < 5)
		return;

	//
	// Load Place data
	//
	PlaceDirectory::IndexType entry;
	filesystem->Read( &entry, sizeof(entry), file );

	// convert entry to actual Place
	SetPlace( placeDirectory.IndexToPlace( entry ) );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Convert loaded IDs to pointers
 * Make sure all IDs are converted, even if corrupt data is encountered.
 */
NavErrorType CNavArea::PostLoad( void )
{
	NavErrorType error = NAV_OK;

	// connect areas together
	for( int d=0; d<NUM_DIRECTIONS; d++ )
	{
		FOR_EACH_LL( m_connect[d], it )
		{
			NavConnect *connect = &m_connect[ d ][ it ];

			unsigned int id = connect->id;
			connect->area = TheNavMesh->GetNavAreaByID( id );
			if (id && connect->area == NULL)
			{
				Error( "CNavArea::PostLoad: Corrupt navigation data. Cannot connect Navigation Areas.\n" );
				error = NAV_CORRUPT_DATA;
			}
		}		
	}

	// resolve approach area IDs
	for( int a=0; a<m_approachCount; ++a )
	{
		m_approach[a].here.area = TheNavMesh->GetNavAreaByID( m_approach[a].here.id );
		if (m_approach[a].here.id && m_approach[a].here.area == NULL)
		{
			Error( "CNavArea::PostLoad: Corrupt navigation data. Missing Approach Area (here).\n" );
			error = NAV_CORRUPT_DATA;
		}

		m_approach[a].prev.area = TheNavMesh->GetNavAreaByID( m_approach[a].prev.id );
		if (m_approach[a].prev.id && m_approach[a].prev.area == NULL)
		{
			Error( "CNavArea::PostLoad: Corrupt navigation data. Missing Approach Area (prev).\n" );
			error = NAV_CORRUPT_DATA;
		}

		m_approach[a].next.area = TheNavMesh->GetNavAreaByID( m_approach[a].next.id );
		if (m_approach[a].next.id && m_approach[a].next.area == NULL)
		{
			Error( "CNavArea::PostLoad: Corrupt navigation data. Missing Approach Area (next).\n" );
			error = NAV_CORRUPT_DATA;
		}
	}

	// resolve spot encounter IDs
	SpotEncounter *e;
	FOR_EACH_LL( m_spotEncounterList, it )
	{
		e = m_spotEncounterList[ it ];

		e->from.area = TheNavMesh->GetNavAreaByID( e->from.id );
		if (e->from.area == NULL)
		{
			Error( "CNavArea::PostLoad: Corrupt navigation data. Missing \"from\" Navigation Area for Encounter Spot.\n" );
			error = NAV_CORRUPT_DATA;
		}

		e->to.area = TheNavMesh->GetNavAreaByID( e->to.id );
		if (e->to.area == NULL)
		{
			Error( "CNavArea::PostLoad: Corrupt navigation data. Missing \"to\" Navigation Area for Encounter Spot.\n" );
			error = NAV_CORRUPT_DATA;
		}

		if (e->from.area && e->to.area)
		{
			// compute path
			float halfWidth;
			ComputePortal( e->to.area, e->toDir, &e->path.to, &halfWidth );
			ComputePortal( e->from.area, e->fromDir, &e->path.from, &halfWidth );

			const float eyeHeight = HalfHumanHeight;
			e->path.from.z = e->from.area->GetZ( &e->path.from ) + eyeHeight;
			e->path.to.z = e->to.area->GetZ( &e->path.to ) + eyeHeight;
		}

		// resolve HidingSpot IDs
		FOR_EACH_LL( e->spotList, sit )
		{
			SpotOrder *order = &e->spotList[ sit ];

			order->spot = GetHidingSpotByID( order->id );
			if (order->spot == NULL)
			{
				Error( "CNavArea::PostLoad: Corrupt navigation data. Missing Hiding Spot\n" );
				error = NAV_CORRUPT_DATA;
			}
		}
	}

	// build overlap list
	/// @todo Optimize this
	FOR_EACH_LL( TheNavAreaList, nit )
	{
		CNavArea *area = TheNavAreaList[ nit ];

		if (area == this)
			continue;

		if (IsOverlapping( area ))
			m_overlapList.AddToTail( area );
	}

	return error;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Return the filename for this map's "nav map" file
 */
const char *CNavMesh::GetFilename( void ) const
{
	// filename is local to game dir for Steam, so we need to prepend game dir for regular file save
	char gamePath[256];
	engine->GetGameDir( gamePath, 256 );

	static char filename[256];
	Q_snprintf( filename, sizeof( filename ), "%s\\maps\\%s.nav", gamePath, STRING( gpGlobals->mapname ) );

	return filename;
}

//--------------------------------------------------------------------------------------------------------------
/*
============
COM_FixSlashes

Changes all '/' characters into '\' characters, in place.
============
*/
inline void COM_FixSlashes( char *pname )
{
#ifdef _WIN32
	while ( *pname ) 
	{
		if ( *pname == '/' )
			*pname = '\\';
		pname++;
	}
#else
	while ( *pname ) 
	{
		if ( *pname == '\\' )
			*pname = '/';
		pname++;
	}
#endif
}

/**
 * Store Navigation Mesh to a file
 */
bool CNavMesh::Save( void ) const
{
	const char *filename = GetFilename();
	if (filename == NULL)
		return false;

	//
	// Store the NAV file
	//
	COM_FixSlashes( const_cast<char *>(filename) );

	FileHandle_t file = filesystem->Open( filename, "wb" );

	if (!file)
	{
		return false;
	}

	// store "magic number" to help identify this kind of file
	unsigned int magic = NAV_MAGIC_NUMBER;
	filesystem->Write( &magic, sizeof(unsigned int), file );

	// store version number of file
	// 1 = hiding spots as plain vector array
	// 2 = hiding spots as HidingSpot objects
	// 3 = Encounter spots use HidingSpot ID's instead of storing vector again
	// 4 = Includes size of source bsp file to verify nav data correlation
	// ---- Beta Release at V4 -----
	// 5 = Added Place info
	unsigned int version = 5;
	filesystem->Write( &version, sizeof(unsigned int), file );


	// get size of source bsp file and store it in the nav file
	// so we can test if the bsp changed since the nav file was made
	char *bspFilename = GetBspFilename( filename );
	if (bspFilename == NULL)
		return false;

	unsigned int bspSize = filesystem->Size( bspFilename );
	DevMsg( "Size of bsp file '%s' is %u bytes.\n", bspFilename, bspSize );

	filesystem->Write( &bspSize, sizeof(unsigned int), file );


	//
	// Build a directory of the Places in this map
	//
	placeDirectory.Reset();

	FOR_EACH_LL( TheNavAreaList, nit )
	{
		CNavArea *area = TheNavAreaList[ nit ];

		Place place = area->GetPlace();

		if (place)
		{
			placeDirectory.AddPlace( place );
		}
	}

	placeDirectory.Save( file );


	//
	// Store navigation areas
	//

	// store number of areas
	unsigned int count = TheNavAreaList.Count();
	filesystem->Write( &count, sizeof(unsigned int), file );

	// store each area
	FOR_EACH_LL( TheNavAreaList, it )
	{
		CNavArea *area = TheNavAreaList[ it ];

		area->Save( file, version );
	}

	filesystem->Flush( file );
	filesystem->Close( file );

	return true;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Load AI navigation data from a file
 */
NavErrorType CNavMesh::Load( void )
{
	// free previous navigation mesh data
	Reset();
	placeDirectory.Reset();

	CNavArea::m_nextID = 1;

	// nav filename is derived from map filename
	char filename[256];
	Q_snprintf( filename, sizeof( filename ), "maps\\%s.nav", STRING( gpGlobals->mapname ) );

	FileHandle_t file = filesystem->Open( filename, "rb" );

	if (!file)
	{
		return NAV_CANT_ACCESS_FILE;
	}

	// check magic number
	int result;
	unsigned int magic;
	result = filesystem->Read( &magic, sizeof(unsigned int), file );
	if (!result || magic != NAV_MAGIC_NUMBER)
	{
		Error( "Invalid navigation file '%s'.\n", filename );
		filesystem->Close( file );
		return NAV_INVALID_FILE;
	}

	// read file version number
	unsigned int version;
	result = filesystem->Read( &version, sizeof(unsigned int), file );
	if (!result || version > 5)
	{
		Error( "Unknown navigation file version.\n" );
		filesystem->Close( file );
		return NAV_BAD_FILE_VERSION;
	}

	if (version >= 4)
	{
		// get size of source bsp file and verify that the bsp hasn't changed
		unsigned int saveBspSize;
		filesystem->Read( &saveBspSize, sizeof(unsigned int), file );

		// verify size
		char *bspFilename = GetBspFilename( filename );
		if ( bspFilename == NULL )
		{
			filesystem->Close( file );
			return NAV_INVALID_FILE;
		}

		unsigned int bspSize = filesystem->Size( bspFilename );

		if (bspSize != saveBspSize)
		{
			Warning( "The Navigation Mesh was built using a different version of this map.\n" );
		}
	}

	// load Place directory
	if (version >= 5)
	{
		placeDirectory.Load( file );
	}

	// get number of areas
	unsigned int count;
	result = filesystem->Read( &count, sizeof(unsigned int), file );

	Extent extent;
	extent.lo.x = 9999999999.9f;
	extent.lo.y = 9999999999.9f;
	extent.hi.x = -9999999999.9f;
	extent.hi.y = -9999999999.9f;

	// load the areas and compute total extent
	for( unsigned int i=0; i<count; ++i )
	{
		CNavArea *area = new CNavArea;
		area->Load( file, version );
		TheNavAreaList.AddToTail( area );

		const Extent *areaExtent = area->GetExtent();

		// check validity of nav area
		if (areaExtent->lo.x >= areaExtent->hi.x || areaExtent->lo.y >= areaExtent->hi.y)
			Warning( "WARNING: Degenerate Navigation Area #%d at ( %g, %g, %g )\n", 
						area->GetID(), area->m_center.x, area->m_center.y, area->m_center.z );

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

	FOR_EACH_LL( TheNavAreaList, it )
	{
		AddNavArea( TheNavAreaList[ it ] );
	}


	// allow areas to connect to each other, etc
	FOR_EACH_LL( TheNavAreaList, pit )
	{
		CNavArea *area = TheNavAreaList[ pit ];
		area->PostLoad();
	}

	//
	// Set up all the ladders
	//
	BuildLadders();

	// the Navigation Mesh has been successfully loaded
	m_isLoaded = true;

	filesystem->Close( file );

	return NAV_OK;
}
