//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <assert.h>

#include "studio.h"

extern studiohdr_t *FindOrLoadGroupFile( char const *modelname );

virtualmodel_t *studiohdr_t::GetVirtualModel( void ) const
{
	if (numincludemodels == 0)
	{
		return NULL;
	}

	virtualmodel_t *pVModel = (virtualmodel_t *)virtualModel;

	if (pVModel == NULL)
	{
		pVModel = new virtualmodel_t;

		// !!! Set cache handle?  Set pointer to local virtual model??
		virtualModel = (void *)pVModel;

		int group = pVModel->m_group.AddToTail( );
		pVModel->m_group[ group ].cache = (void *)this;
		pVModel->AppendModels( 0, this );
	}

	return pVModel;
}


const studiohdr_t *studiohdr_t::FindModel( void **cache, char const *modelname ) const
{
	studiohdr_t *hdr = (studiohdr_t *)(*cache);

	if (hdr)
	{
		return hdr;
	}

	hdr = FindOrLoadGroupFile( modelname );

	*cache = (void *)hdr;

	return hdr;
}

const studiohdr_t *virtualgroup_t::GetStudioHdr( void ) const
{
	return (studiohdr_t *)cache;
}


byte *studiohdr_t::GetAnimBlock( int i ) const
{
	byte *hdr = (byte *)animblockModel;

	if (!hdr)
	{
		hdr = (byte *)FindOrLoadGroupFile( pszAnimBlockName() );
		animblockModel = hdr;
	}

	return hdr + pAnimBlock( i )->datastart;
}

