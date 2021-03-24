//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
// Implements local hooks into named renderable textures.
// See matrendertexture.cpp in material system for list of available RT's
//
//=============================================================================//

#include "materialsystem/imesh.h"
#include "materialsystem/ITexture.h"
#include "vstdlib/strtools.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================
// 
//=============================================================================
static ITexture *s_pPowerOfTwoFrameBufferTexture = NULL;

static void PowerOfTwoFrameBufferTextureRestoreFunc( int nChangeFlags )
{
	s_pPowerOfTwoFrameBufferTexture = NULL;
}

ITexture *GetPowerOfTwoFrameBufferTexture( void )
{
	if( !s_pPowerOfTwoFrameBufferTexture )
	{
		s_pPowerOfTwoFrameBufferTexture = materials->FindTexture( "_rt_PowerOfTwoFB", TEXTURE_GROUP_RENDER_TARGET );
		Assert( !IsErrorTexture( s_pPowerOfTwoFrameBufferTexture ) );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( PowerOfTwoFrameBufferTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pPowerOfTwoFrameBufferTexture;
}

//=============================================================================
// 
//=============================================================================
static ITexture *s_pCameraTexture = NULL;

static void CameraTextureRestoreFunc( int nChangeFlags )
{
	s_pCameraTexture = NULL;
}

ITexture *GetCameraTexture( void )
{
	if( !s_pCameraTexture )
	{
		s_pCameraTexture = materials->FindTexture( "_rt_Camera", TEXTURE_GROUP_RENDER_TARGET );
		Assert( !IsErrorTexture( s_pCameraTexture ) );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( CameraTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pCameraTexture;
}

//=============================================================================
// 
//=============================================================================
static ITexture *s_pFullFrameFrameBufferTexture[MAX_FB_TEXTURES] = { NULL, NULL, NULL, NULL };

static void FullFrameFrameBufferTextureRestoreFunc( int nChangeFlags )
{
	int i;
	for( i = 0; i < MAX_FB_TEXTURES; i++ )
	{
		s_pFullFrameFrameBufferTexture[i] = NULL;
	}
}

ITexture *GetFullFrameFrameBufferTexture( int textureIndex )
{
	if( !s_pFullFrameFrameBufferTexture[textureIndex] )
	{
		char name[256];
		if( textureIndex != 0 )
		{
			sprintf( name, "_rt_FullFrameFB%d", textureIndex );
		}
		else
		{
			Q_strcpy( name, "_rt_FullFrameFB" );
		}
		s_pFullFrameFrameBufferTexture[textureIndex] = materials->FindTexture( name, TEXTURE_GROUP_RENDER_TARGET );
		Assert( !IsErrorTexture( s_pFullFrameFrameBufferTexture[textureIndex] ) );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( FullFrameFrameBufferTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pFullFrameFrameBufferTexture[textureIndex];
}


//=============================================================================
// Water reflection
//=============================================================================
static ITexture *s_pWaterReflectionTexture = NULL;

static void WaterReflectionTextureRestoreFunc( int nChangeFlags )
{
	s_pWaterReflectionTexture = NULL;
}

ITexture *GetWaterReflectionTexture( void )
{
	if( !s_pWaterReflectionTexture )
	{
		s_pWaterReflectionTexture = materials->FindTexture( "_rt_WaterReflection", TEXTURE_GROUP_RENDER_TARGET );
		Assert( !IsErrorTexture( s_pWaterReflectionTexture ) );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( WaterReflectionTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pWaterReflectionTexture;
}

//=============================================================================
// Water refraction
//=============================================================================
static ITexture *s_pWaterRefractionTexture = NULL;

static void WaterRefractionTextureRestoreFunc( int nChangeFlags )
{
	s_pWaterRefractionTexture = NULL;
}

ITexture *GetWaterRefractionTexture( void )
{
	if( !s_pWaterRefractionTexture )
	{
		s_pWaterRefractionTexture = materials->FindTexture( "_rt_WaterRefraction", TEXTURE_GROUP_RENDER_TARGET );
		Assert( !IsErrorTexture( s_pWaterRefractionTexture ) );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( WaterRefractionTextureRestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pWaterRefractionTexture;
}


//=============================================================================
// 
//=============================================================================
static ITexture *s_pSmallBufferHDR0 = NULL;

static void SmallBufferHDR0RestoreFunc( int nChangeFlags )
{
	s_pSmallBufferHDR0 = NULL;
}

ITexture *GetSmallBufferHDR0( void )
{
	if( !s_pSmallBufferHDR0 )
	{
		s_pSmallBufferHDR0 = materials->FindTexture( "_rt_SmallHDR0", TEXTURE_GROUP_RENDER_TARGET );
		Assert( !IsErrorTexture( s_pSmallBufferHDR0 ) );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( SmallBufferHDR0RestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pSmallBufferHDR0;
}

//=============================================================================
// 
//=============================================================================
static ITexture *s_pSmallBufferHDR1 = NULL;

static void SmallBufferHDR1RestoreFunc( int nChangeFlags )
{
	s_pSmallBufferHDR0 = NULL;
}

ITexture *GetSmallBufferHDR1( void )
{
	if( !s_pSmallBufferHDR1 )
	{
		s_pSmallBufferHDR1 = materials->FindTexture( "_rt_SmallHDR1", TEXTURE_GROUP_RENDER_TARGET );
		Assert( !IsErrorTexture( s_pSmallBufferHDR1 ) );

		static bool bAddedRestoreFunc = false;
		if( !bAddedRestoreFunc )
		{
			materials->AddRestoreFunc( SmallBufferHDR1RestoreFunc );
			bAddedRestoreFunc = true;
		}
	}
	
	return s_pSmallBufferHDR1;
}

