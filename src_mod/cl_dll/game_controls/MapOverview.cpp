//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: MiniMap.cpp: implementation of the CMiniMap class.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "mapoverview.h"
#include <vgui/isurface.h>
#include <vgui/ILocalize.h>
#include <filesystem.h>
#include <keyvalues.h>
#include <convar.h>
#include <mathlib.h>
#include <cl_dll/iviewport.h>
#include <igameresources.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar overview_health( "overview_health", "1", 0, "Show player's health in map overview.\n" );
static ConVar overview_names ( "overview_names",  "1", 0, "Show player's names in map overview.\n" );
static ConVar overview_tracks( "overview_tracks", "1", 0, "Show player's tracks in map overview.\n" );
static ConVar overview_locked( "overview_locked", "1", 0, "Locks map angle, doesn't follow view angle.\n" );

static CMapOverview *s_pMapOverview = NULL; // we assume only one overview is created

CON_COMMAND( overview_zoomabs, "Sets overview map zoom: <zoom>" )
{
	if ( !s_pMapOverview || engine->Cmd_Argc() < 2 )
		return;
	
	s_pMapOverview->SetZoom( atof(engine->Cmd_Argv( 1 )) );
}

CON_COMMAND( overview_zoomrel, "Changes overview map zoom: <factor>" )
{
	if ( !s_pMapOverview || engine->Cmd_Argc() < 2 )
		return;
	
	float factor = Q_atof( engine->Cmd_Argv( 1 ) ); 
	float zoom = s_pMapOverview->GetZoom();
	s_pMapOverview->SetZoom( zoom*factor );
}

CON_COMMAND( overview_mode, "Sets overview map mode off,small,large: <0|1|2>" )
{
	if ( !s_pMapOverview || engine->Cmd_Argc() < 2 )
		return;

	int mode = Q_atoi(engine->Cmd_Argv( 1 ));

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( pPlayer && !pPlayer->IsObserver() )
		mode = 0; // if not in observer mode, disable overview

	int w, h; vgui::surface()->GetScreenSize( w, h );

	if ( mode == 0 )
	{
		gViewPortInterface->ShowPanel( s_pMapOverview, false );		
	}
	else if ( mode == 1)
	{
		h=h/2.2360679774f;	// "golden section" always works
		s_pMapOverview->SetBounds( 0,0, h, h );
		s_pMapOverview->SetZoom( 3.0f );
		
		gViewPortInterface->ShowPanel( s_pMapOverview, true );		
	}
	else if ( mode == 2 )
	{
		h = h-YRES(52*2)+1;	// 52 - height of black spectator bars 
		s_pMapOverview->SetBounds( 0, YRES(52), w, h );
		s_pMapOverview->SetZoom( -1.0f ); // zoom -1 means best full zoom
		gViewPortInterface->ShowPanel( s_pMapOverview, true );		
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


using namespace vgui;

CMapOverview::CMapOverview( IViewPort *pViewPort ) : BaseClass( NULL, PANEL_OVERVIEW )
{
	m_pViewPort = pViewPort;

	SetBounds( 0,0, 256, 256 );
	SetBgColor( Color( 0,0,0,100 ) );
	SetPaintBackgroundEnabled( true );
	SetVisible( false );
	
	m_nMapTextureID = -1;
	m_MapKeyValues = NULL;

	m_MapOrigin = Vector( 0, 0, 0 );
	m_fMapScale = 1.0f;
	m_bFollowAngle = false;

	m_fZoom = 3.0f;
	m_MapCenter = Vector2D( 512, 512 );
	m_ViewOrigin = Vector2D( 512, 512 );
	m_fViewAngle = 0;
	m_fTrailUpdateInterval = 1.0f;

	m_bShowNames = true;
	m_bShowHealth = true;
	m_bShowHurts = true;
	m_bShowTrails = true;

	Reset();
	
	Q_memset( m_Players, 0, sizeof(m_Players) );
	Q_memset( m_TeamIcons, 0, sizeof(m_TeamIcons) );
	Q_memset( m_TeamColors, 0, sizeof(m_TeamColors) );

	//setup team red
	m_TeamColors[2] = COLOR_RED;
	m_TeamIcons[2] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_TeamIcons[2] , ( const char * )"sprites/player_red", true, false);

	// setup team blue
	m_TeamColors[3] = COLOR_BLUE;
	m_TeamIcons[3] = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_TeamIcons[3], ( const char * )"sprites/player_blue", true, false);

	gameeventmanager->AddListener( this, false );	// register for events as client listener
	
	s_pMapOverview = this;  // for cvars access etc
}

void CMapOverview::ApplySchemeSettings(vgui::IScheme *scheme)
{
	BaseClass::ApplySchemeSettings( scheme );

	SetBgColor( Color( 0,0,0,100 ) );
	SetPaintBackgroundEnabled( true );
}

CMapOverview::~CMapOverview()
{
	if ( m_MapKeyValues )
		m_MapKeyValues->deleteThis();

	s_pMapOverview = NULL;
	gameeventmanager->RemoveListener(this);

	//TODO release Textures ? clear lists
}

void CMapOverview::UpdatePlayerTrails()
{
	if ( m_fNextTrailUpdate > m_fWorldTime )
		return;

	m_fNextTrailUpdate = m_fWorldTime + 1.0f; // update once a second

	for (int i=0; i<MAX_PLAYERS; i++)
	{
		MapPlayer_t *p = &m_Players[i];
		
		// no trails for spectators or dead players
		if ( (p->team <= TEAM_SPECTATOR) || (p->health <= 0) )
		{
			continue;
		}

		// move old trail points 
		for ( int i=MAX_TRAIL_LENGTH-1; i>0; i--)
		{
			p->trail[i]=p->trail[i-1];
		}

		p->trail[0] = WorldToMap ( p->position );
	}
}

void CMapOverview::Paint()
{
	if ( m_nFollowEntity != 0 )
	{
		C_BaseEntity *ent = ClientEntityList().GetEnt( m_nFollowEntity );
		
		if ( ent )
		{
			Vector position = ent->EyePosition();
			QAngle angle = ent->EyeAngles();

			if ( m_nFollowEntity <= MAX_PLAYERS )
			{
				SetPlayerPositions( m_nFollowEntity-1, position, angle );
			}

			SetCenter( WorldToMap(position) );
			SetAngle( angle[YAW] );
		}
	}

	UpdatePlayerTrails();

	DrawMapTexture();

	DrawMapPlayerTrails();

	DrawMapPlayers();

	BaseClass::Paint();
}

void CMapOverview::SetPlayerPositions(int index, const Vector &position, const QAngle &angle)
{
	MapPlayer_t *p = &m_Players[index];

	p->angle = angle;
	p->position = position;
}

//-----------------------------------------------------------------------------
// Purpose: shows/hides the buy menu
//-----------------------------------------------------------------------------
void CMapOverview::ShowPanel(bool bShow)
{
	SetVisible( bShow );
}

bool CMapOverview::NeedsUpdate( void )
{
	return m_fNextUpdateTime < gpGlobals->curtime;
}

void CMapOverview::Update( void )
{
	// update settings
	m_bShowNames = overview_names.GetBool();
	m_bShowHealth = overview_health.GetBool();
	m_bFollowAngle = !overview_locked.GetBool();
	m_fTrailUpdateInterval = overview_tracks.GetInt();

	m_fWorldTime = gpGlobals->curtime;

	m_fNextUpdateTime = gpGlobals->curtime + 0.2f; // update 5 times a second

	for ( int i = 1; i<= MAX_PLAYERS; i++)
	{
		C_BasePlayer *p = UTIL_PlayerByIndex( i );

		if ( !p )
		{
			// dont' draw this guy
			m_Players[i-1].health = -1;
			continue;
		}

		// update position of active players in our PVS
		Vector position = p->EyePosition();
		QAngle angles = p->EyeAngles();

		if ( position.x == 0.0f && position.y == 0.0f )
			continue;

		SetPlayerPositions( i-1, position, angles );
	}
}

void CMapOverview::Reset( void )
{
	m_fNextUpdateTime = 0;
}

void CMapOverview::SetData(KeyValues *data)
{
	m_fZoom =  data->GetFloat( "zoom", m_fZoom );
	m_nFollowEntity =  data->GetInt( "entity", m_nFollowEntity );
}


CMapOverview::MapPlayer_t* CMapOverview::GetPlayerByUserID( int userID )
{
	for (int i=0; i<MAX_PLAYERS; i++)
	{
		MapPlayer_t *player = &m_Players[i];

		if ( player->userid == userID )
			return player;
	}

	return NULL;
}

bool CMapOverview::IsInPanel(Vector2D &pos)
{
	int x,y,w,t;

	GetBounds( x,y,w,t );

	return ( pos.x >= 0 && pos.x < w && pos.y >= 0 && pos.y < t );
}

void CMapOverview::DrawMapTexture()
{
	// now draw a box around the outside of this panel
	int x0, y0, x1, y1;
	int wide, tall;

	GetSize(wide, tall);
	x0 = 0; y0 = 0; x1 = wide - 2; y1 = tall - 2 ;

	if ( m_nMapTextureID < 0 )
		return;
	
	Vertex_t points[4] =
	{
		Vertex_t( MapToPanel ( Vector2D(0,0) ), Vector2D(0,0) ),
		Vertex_t( MapToPanel ( Vector2D(OVERVIEW_MAP_SIZE-1,0) ), Vector2D(1,0) ),
		Vertex_t( MapToPanel ( Vector2D(OVERVIEW_MAP_SIZE-1,OVERVIEW_MAP_SIZE-1) ), Vector2D(1,1) ),
		Vertex_t( MapToPanel ( Vector2D(0,OVERVIEW_MAP_SIZE-1) ), Vector2D(0,1) )
	};

	surface()->DrawSetTexture( m_nMapTextureID );
	surface()->DrawTexturedPolygon( 4, points );

	// draw red center point
	surface()->DrawSetColor( 255,0,0,255 );
	Vector2D center = MapToPanel( m_ViewOrigin );
	surface()->DrawFilledRect( center.x-2, center.y-2, center.x+2, center.y+2);
}

void CMapOverview::DrawMapPlayerTrails()
{
	if ( m_fTrailUpdateInterval <= 0 )
		return; // truned off

	for (int i=0; i<MAX_PLAYERS; i++)
	{
		MapPlayer_t *p = &m_Players[i];
		
		if ( p->team <= TEAM_SPECTATOR )
		{
			continue;
		}

		p->trail[0] = WorldToMap ( p->position );

		for ( int i=0; i<(MAX_TRAIL_LENGTH-1); i++)
		{
			if ( p->trail[i+1].x == 0 && p->trail[i+1].y == 0 )
				break;

			Vector2D pos1 = MapToPanel( p->trail[i] );
			Vector2D pos2 = MapToPanel( p->trail[i+1] );

			int intensity = 255 - float(255.0f * i) / MAX_TRAIL_LENGTH;

			Vector2D dist = pos1 - pos2;
			
			// don't draw too long lines, player probaly teleported
			if ( dist.LengthSqr() < (128*128) )
			{	
				surface()->DrawSetColor( p->color[0], p->color[1], p->color[2], intensity );
				surface()->DrawLine( pos1.x, pos1.y, pos2.x, pos2.y );
			}
		}
	}
}

void CMapOverview::DrawMapPlayers()
{
	// Make sure we actually have the font...
	vgui::IScheme *pScheme = vgui::scheme()->GetIScheme( GetScheme() );
	vgui::HFont hPlayerFont = pScheme->GetFont( "DefaultSmall" );
	
	surface()->DrawSetTextFont( hPlayerFont );

	for (int i=0; i<MAX_PLAYERS; i++)
	{
		MapPlayer_t *p = &m_Players[i];
		
		if ( (p->team <= TEAM_SPECTATOR) || (p->health <= 0) )
		{
			// don't draw dead players / spectators
			continue;
		}

		float viewAngle = p->angle[YAW];
		float scale = 64;
		Vector offset;	offset.z = 0;
		Vector pos = p->position;

		Vector2D pospanel = WorldToMap( pos );
		pospanel = MapToPanel( pospanel );

		if ( !IsInPanel( pospanel ) )
			continue; // player is not within overview panel

		offset.x = -scale;	offset.y = scale;
		VectorYawRotate( offset, viewAngle, offset );
		Vector2D pos1 = WorldToMap( pos + offset );

		offset.x = scale;	offset.y = scale;
		VectorYawRotate( offset, viewAngle, offset );
		Vector2D pos2 = WorldToMap( pos + offset );

		offset.x = scale;	offset.y = -scale;
		VectorYawRotate( offset, viewAngle, offset );
		Vector2D pos3 = WorldToMap( pos + offset );

		offset.x = -scale;	offset.y = -scale;
		VectorYawRotate( offset, viewAngle, offset );
		Vector2D pos4 = WorldToMap( pos + offset );

		Vertex_t points[4] =
		{
			Vertex_t( MapToPanel ( pos1 ), Vector2D(0,0) ),
			Vertex_t( MapToPanel ( pos2 ), Vector2D(1,0) ),
			Vertex_t( MapToPanel ( pos3 ), Vector2D(1,1) ),
			Vertex_t( MapToPanel ( pos4 ), Vector2D(0,1) )
		};

		surface()->DrawSetTexture( p->icon );
		surface()->DrawTexturedPolygon( 4, points );

		offset.x = 0; offset.y = scale;
		pos2 = WorldToMap( offset );
		pos2 = MapToPanel( pos2 );

		offset.x = 0; offset.y = 0;
		pos3 = WorldToMap( offset );
		pos3 = MapToPanel( pos3 );

		int a = pos2.y-pos3.y; 
		int b = pos2.x-pos3.x;
		int d = sqrt((float)(a*a+b*b)); // d = number of panel pixels for "scale" units in world

		pospanel.y += d+4;
		
		if ( m_bShowHealth )
		{
			// helath bar is 50x3 pixels
			surface()->DrawSetColor( 0,0,0,255 );
			surface()->DrawFilledRect( pospanel.x-d, pospanel.y-1, pospanel.x+d, pospanel.y+1 );

			int length = ((d*2)*p->health) / 100;
			surface()->DrawSetColor( 0,255,0,255 );
			surface()->DrawFilledRect( pospanel.x-d, pospanel.y-1, pospanel.x-d+length, pospanel.y+1 );

			pospanel.y += 3;
		}

		if ( m_bShowNames )
		{
			wchar_t playerName[ 64 ];
						
			localize()->ConvertANSIToUnicode( p->name, playerName, sizeof( playerName ) );

			int wide, tall;
			surface()->GetTextSize( hPlayerFont, playerName, wide, tall );

			int x = pospanel.x-(wide/2);
			int y = pospanel.y;

			// draw shadow text
			surface()->DrawSetTextColor( 0, 0, 0, 255 );
			surface()->DrawSetTextPos( x+1, y );
			surface()->DrawPrintText( playerName, wcslen( playerName) );

			// draw text
			surface()->DrawSetTextColor( p->color[0], p->color[1], p->color[2], 255 );
			surface()->DrawSetTextPos( x, y );
			surface()->DrawPrintText( playerName, wcslen( playerName) );
		}
	}
}

// map coordinates are always [0,1023]x[0,1023]

Vector2D CMapOverview::WorldToMap( const Vector &worldpos )
{
	Vector2D offset( worldpos.x - m_MapOrigin.x, worldpos.y - m_MapOrigin.y);

	offset.x /=  m_fMapScale;
	offset.y /= -m_fMapScale;

	return offset;
}


Vector2D CMapOverview::MapToPanel( const Vector2D &mappos )
{
	int pwidth, pheight; 
	Vector2D panelpos;
	float viewAngle = m_fViewAngle - 90.0f;

	GetSize(pwidth, pheight);

	Vector offset;
	offset.x = mappos.x - m_MapCenter.x;
	offset.y = mappos.y - m_MapCenter.y;
	offset.z = 0;

	if ( !m_bFollowAngle )
	{
		if ( m_bRotateMap )
			viewAngle = 90.0f;
		else
			viewAngle = 0.0f;
	}

	VectorYawRotate( offset, viewAngle, offset );

	offset.x *= m_fZoom/OVERVIEW_MAP_SIZE;
	offset.y *= m_fZoom/OVERVIEW_MAP_SIZE;

	panelpos.x = (pwidth * 0.5f) + (pheight * offset.x);
	panelpos.y = (pheight * 0.5f) + (pheight * offset.y);

	return panelpos;
}

void CMapOverview::SetTime( float time )
{
	m_fWorldTime = time;
}

void CMapOverview::SetMap(const char * levelname)
{
	// load new KeyValues

	if ( m_MapKeyValues && Q_strcmp( levelname, m_MapKeyValues->GetName() ) == 0 )
	{
		return;	// map didn't change
	}

	if ( m_MapKeyValues )
		m_MapKeyValues->deleteThis();

	m_MapKeyValues = new KeyValues( levelname );

	char tempfile[MAX_PATH];
	Q_snprintf( tempfile, sizeof( tempfile ), "resource/overviews/%s.txt", levelname );
	
	if ( !m_MapKeyValues->LoadFromFile( vgui::filesystem(), tempfile, "GAME" ) )
	{
		DevMsg( 1, "CMapOverview::OnNewLevel: couldn't load file %s.\n", tempfile );
		m_nMapTextureID = -1;
		return;
	}

	// TODO release old texture ?

	m_nMapTextureID = surface()->CreateNewTextureID();

	//if we have not uploaded yet, lets go ahead and do so
	// surface()->DrawSetTextureFile(m_nMapTextureID,( const char * )"sprites/shopping_cart", false, false);
	surface()->DrawSetTextureFile( m_nMapTextureID, m_MapKeyValues->GetString("material"), true, false);

	m_MapOrigin.x	= m_MapKeyValues->GetInt("pos_x");
	m_MapOrigin.y	= m_MapKeyValues->GetInt("pos_y");
	m_fMapScale		= m_MapKeyValues->GetFloat("scale", 1.0f);
	m_bRotateMap	= m_MapKeyValues->GetInt("rotate")!=0;
	m_fFullZoom		= m_MapKeyValues->GetFloat("zoom", 1.0f );

	m_fNextTrailUpdate = m_fWorldTime;
}

void CMapOverview::ResetRound()
{
	for (int i=0; i<MAX_PLAYERS; i++)
	{
		MapPlayer_t *p = &m_Players[i];
		
		if ( p->team > TEAM_SPECTATOR )
		{
			p->health = 100;
		}

		Q_memset( p->trail, 0, sizeof(p->trail) );
	}


}

void CMapOverview::OnMousePressed( MouseCode code )
{
	
}

void CMapOverview::FireGameEvent( KeyValues * event)
{
	const char * type = event->GetName();

	if ( Q_strcmp(type, "game_newmap") == 0 )
	{
		SetMap( event->GetString("mapname") );
	}

	else if ( Q_strcmp(type, "round_start") == 0 )
	{
		ResetRound();
	}

	else if ( Q_strcmp(type,"player_connect") == 0 )
	{
		int index = event->GetInt("index");
		MapPlayer_t *p = &m_Players[index];

		if ( !p ) return;

		p->index = index;
		p->userid = event->GetInt("userid");
		Q_strncpy( p->name, event->GetString("name","unknown"), sizeof(p->name) );

		// Reset settings
		Q_memset( p->trail, 0, sizeof(p->trail) );
		p->team = TEAM_UNASSIGNED;
		p->health = 0;
	}

	else if ( Q_strcmp(type,"player_info") == 0 )
	{
		int index = event->GetInt("index");
		MapPlayer_t *p = &m_Players[index];

		if ( !p ) return;

		p->index = index;
		p->userid = event->GetInt("userid");
		Q_strncpy( p->name, event->GetString("name","unknown"), sizeof(p->name) );
	}

	else if ( Q_strcmp(type,"player_team") == 0 )
	{
		MapPlayer_t *p = GetPlayerByUserID( event->GetInt("userid") );

		if ( !p ) return;

		p->team = event->GetInt("team");
		p->icon = m_TeamIcons[ p->team  ];
		p->color = m_TeamColors[ p->team ];
	}

	else if ( Q_strcmp(type,"player_hurt") == 0 )
	{
		MapPlayer_t *p = GetPlayerByUserID( event->GetInt("userid") );

		if ( !p ) return;

		p->health = event->GetInt("health");
	}

	else if ( Q_strcmp(type,"player_death") == 0 )
	{
		MapPlayer_t *p = GetPlayerByUserID( event->GetInt("userid") );

		if ( !p ) return;

		p->health = 0;
		Q_memset( p->trail, 0, sizeof(p->trail) ); // clear trails
	}

	else if ( Q_strcmp(type,"player_disconnect") == 0 )
	{
		MapPlayer_t *p = GetPlayerByUserID( event->GetInt("userid") );
		
		if ( !p ) return;

		Q_memset( p, 0, sizeof(MapPlayer_t) ); // clear player field
	}
}

void CMapOverview::SetCenter(const Vector2D &mappos)
{
	int width, height;

	GetSize( width, height);

	m_ViewOrigin = mappos;
	m_MapCenter = mappos;

	width = OVERVIEW_MAP_SIZE / (m_fZoom*2);
	height = OVERVIEW_MAP_SIZE / (m_fZoom*2);

	if ( m_MapCenter.x < width )
		m_MapCenter.x = width;

	if ( m_MapCenter.x > (OVERVIEW_MAP_SIZE-width) )
		m_MapCenter.x = (OVERVIEW_MAP_SIZE-width);

	if ( m_MapCenter.y < height )
		m_MapCenter.y = height;

	if ( m_MapCenter.y > (OVERVIEW_MAP_SIZE-height) )
		m_MapCenter.y = (OVERVIEW_MAP_SIZE-height);

	//center if in full map mode
	if ( m_fZoom <= m_fFullZoom )
	{
		m_MapCenter.x = OVERVIEW_MAP_SIZE/2;
		m_MapCenter.y = OVERVIEW_MAP_SIZE/2;
	}

	
}

void CMapOverview::SetFollowAngle(bool state)
{
	m_bFollowAngle = state;
}

void CMapOverview::SetFollowEntity(int entindex)
{
	m_nFollowEntity = entindex;
}

float CMapOverview::GetZoom( void )
{
	return m_fZoom;
}

void CMapOverview::SetZoom( float zoom )
{
	m_fZoom = zoom;

	if ( m_fZoom == -1.0f )
	{
		m_fZoom = m_fFullZoom;
	}
	else if ( m_fZoom < 0.5f )
	{
		m_fZoom = 0.5f;
	}
	else if ( m_fZoom > 5.0f )
	{
		m_fZoom = 5.0f;
	}
}

void CMapOverview::SetAngle(float angle)
{
	m_fViewAngle = angle;
}

void CMapOverview::ShowPlayerNames(bool state)
{
	m_bShowNames = state;
}

void CMapOverview::ShowTracers(bool state)
{
	m_bShowTracers = state;
}

void CMapOverview::ShowExplosions(bool state)
{
	m_bShowExplosions = state;
}

void CMapOverview::ShowHealth(bool state)
{
	m_bShowHealth = state;
}

void CMapOverview::ShowHurts(bool state)
{
	m_bShowHurts = state;
}

void CMapOverview::ShowTracks(float seconds)
{
	m_fTrailUpdateInterval = seconds;
}

bool CMapOverview::SetTeamColor(int team, Color color)
{
	if ( team < 0 || team>= MAX_TEAMS )
		return false;

	m_TeamColors[team] = color;

	return true;
}