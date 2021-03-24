//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: MiniMap.h: interface for the CMiniMap class.
//
// $NoKeywords: $
//=============================================================================//

#if !defined HLTVPANEL_H
#define HLTVPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>
// #include <imapoverview.h>
#include <cl_dll/iviewport.h>
#include <vector.h>
#include <igameevents.h>
#include <shareddefs.h>
#include <const.h>

#define MAX_TRAIL_LENGTH	30
#define OVERVIEW_MAP_SIZE	1024	// an overview map is 1024x1024 pixels

class CMapOverview : public vgui::Panel, public IViewPortPanel, public IGameEventListener
{

	DECLARE_CLASS_SIMPLE( CMapOverview, vgui::Panel );

public:	

	CMapOverview( IViewPort *pViewPort );
	virtual ~CMapOverview();

private:	// private structures & types
	
	typedef vgui::Panel BaseClass;

	// list of game events the hLTV takes care of

	typedef struct {
		int		xpos;
		int		ypos;
	} FootStep_t;	

	typedef struct MapPlayer_s {
		int		index;		// player's entity index
		int		userid;		// user ID on server
		int		icon;		// players texture icon ID
		Color   color;		// players team color
		char	name[MAX_PLAYER_NAME_LENGTH];
		int		team;		// N,T,CT
		int		health;		// 0..100, 7 bit
		Vector	position;	// current x pos
		QAngle	angle;		// view origin 0..360
		Vector2D trail[MAX_TRAIL_LENGTH];	// save 1 footstep each second for 1 minute
	} MapPlayer_t;

public: // IViewPortPanel interface:

	virtual const char *GetName( void ) { return PANEL_OVERVIEW; }
	virtual void SetData(KeyValues *data);
	virtual void Reset();
	virtual void Update();
	virtual bool NeedsUpdate( void );
	virtual bool HasInputElements( void ) { return false; }
	virtual void ShowPanel( bool bShow );

	// both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
	virtual bool IsVisible() { return BaseClass::IsVisible(); }
	virtual void SetParent(vgui::VPANEL parent) { BaseClass::SetParent(parent); }

public: // IGameEventListener

	virtual void FireGameEvent( KeyValues * event);
	virtual bool IsServerSide() { return false; };

public:	// VGUI overrides

	virtual void Paint();
	virtual void OnMousePressed( vgui::MouseCode code );
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	
public:	// IMapOverview Interfaces

	virtual float GetZoom( void );
	
	virtual void ShowPlayerNames(bool state);
	virtual void ShowTracers(bool state);
	virtual void ShowExplosions(bool state);
	virtual void ShowHealth(bool state);
	virtual void ShowHurts(bool state);
	virtual void ShowTracks(float seconds); 

	virtual void SetMap(const char * map);
	virtual void SetTime( float time );
	virtual bool SetTeamColor(int team, Color color);
	virtual void SetZoom( float zoom );
	virtual void SetFollowAngle(bool state);
	virtual void SetFollowEntity(int entindex); // 0 = off
	virtual void SetCenter( const Vector2D &mappos); 
	virtual void SetAngle( float angle);
	
	virtual Vector2D WorldToMap( const Vector &worldpos );

	virtual void SetPlayerPositions(int index, const Vector &position, const QAngle &angle);
	
private:
	
	Vector2D MapToPanel( const Vector2D &mappos );

	void	DrawMapTexture();
	void	DrawMapPlayers();
	void	DrawMapPlayerTrails();
	void	UpdatePlayerTrails();
	void	ResetRound();
	bool	IsInPanel(Vector2D &pos);

	MapPlayer_t* GetPlayerByUserID( int userID );
	
		
	// IGameEventManager * m_GameEventManager;
	IViewPort *m_pViewPort;
		
	MapPlayer_t			m_Players[MAX_PLAYERS];

	Color	m_TeamColors[MAX_TEAMS];
	int		m_TeamIcons[MAX_TEAMS];
	
	bool m_bShowNames;
	bool m_bShowTracers;
	bool m_bShowTrails;
	bool m_bShowExplosions;
	bool m_bShowHealth;
	bool m_bShowHurts;
	
	int	 m_nMapTextureID;		// texture id for current overview image
	
	KeyValues * m_MapKeyValues; // keyvalues describinng overview parameters

	Vector	m_MapOrigin;	// read from KeyValues files
	float	m_fMapScale;	// origin and sacle used when screenshot was made
	bool	m_bRotateMap;	// if true roatate map around 90 degress, so it fits better to 4:3 screen ratio

	int		m_nFollowEntity;// entity number to follow, 0 = off
	float	m_fZoom;		// current zoom n = overview panel shows 1/n^2 of whole map 
	float	m_fFullZoom;	// best zoom factor for full map view (1.0 is map is a square) 
	Vector2D m_ViewOrigin;	// map coordinates that are in the center of the pverview panel
	Vector2D m_MapCenter;	// map coordinates that are in the center of the pverview panel

	float	m_fNextUpdateTime;
	float	m_fViewAngle;	// rototaion of overview map
	float	m_fWorldTime;	// current world time
	float   m_fNextTrailUpdate; // next time to update player trails
	float	m_fTrailUpdateInterval; // if -1 don't show trails
	bool	m_bFollowAngle;	// if true, map rotates with view angle
	
};

#endif //
