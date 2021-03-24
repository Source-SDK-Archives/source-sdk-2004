//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <stdio.h>

#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/MouseCode.h"
#include "vgui/Cursor.h"
#include "KeyValues.h"

#include "vgui_controls/URLLabel.h"
#include "vgui_controls/TextImage.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
URLLabel::URLLabel(Panel *parent, const char *panelName, const char *text, const char *pszURL) : Label(parent, panelName, text)
{
    m_pszURL = NULL;
	m_bUnderline = false;
    m_iURLSize = 0;
    if (pszURL && strlen(pszURL) > 0)
    {
        SetURL(pszURL);
    }
}

//-----------------------------------------------------------------------------
// Purpose: unicode constructor
//-----------------------------------------------------------------------------
URLLabel::URLLabel(Panel *parent, const char *panelName, const wchar_t *wszText, const char *pszURL) : Label(parent, panelName, wszText)
{
    m_pszURL = NULL;
	m_bUnderline = false;
    m_iURLSize = 0;
    if (pszURL && strlen(pszURL) > 0)
    {
        SetURL(pszURL);
    }
}

//-----------------------------------------------------------------------------
// Purpose: destructor
//-----------------------------------------------------------------------------
URLLabel::~URLLabel()
{
    if (m_pszURL)
        delete [] m_pszURL;
}

//-----------------------------------------------------------------------------
// Purpose: sets the URL
//-----------------------------------------------------------------------------
void URLLabel::SetURL(const char *pszURL)
{
	int iNewURLSize = strlen(pszURL);
	if (iNewURLSize > m_iURLSize || !m_pszURL)
	{
		delete [] m_pszURL;
		m_pszURL = new char [iNewURLSize + 1];
	}
	strcpy(m_pszURL, pszURL);
	m_iURLSize = iNewURLSize;
}

//-----------------------------------------------------------------------------
// Purpose: If we were left clicked on, launch the URL
//-----------------------------------------------------------------------------
void URLLabel::OnMousePressed(MouseCode code)
{
    if (code == MOUSE_LEFT)
    {
        if (m_pszURL)
		{
	        system()->ShellExecute("open", m_pszURL);
		}
    }
}

//-----------------------------------------------------------------------------
// Purpose: Applies resouce settings
//-----------------------------------------------------------------------------
void URLLabel::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	const char *pszURL = inResourceData->GetString("URLText", NULL);
	if (pszURL)
	{
        SetURL(pszURL);
	}
	m_bUnderline = inResourceData->GetInt( "Underline", 0 ) != 0;
}

//-----------------------------------------------------------------------------
// Purpose: saves them to disk
//-----------------------------------------------------------------------------
void URLLabel::GetSettings( KeyValues *outResourceData )
{
	BaseClass::GetSettings(outResourceData);

	if (m_pszURL)
	{
		outResourceData->SetString("URLText", m_pszURL);
	}
	outResourceData->SetInt( "Underline", m_bUnderline );
}

//-----------------------------------------------------------------------------
// Purpose: draw an underline under the label
//-----------------------------------------------------------------------------
void URLLabel::Paint()
{
	BaseClass::Paint();

	if ( m_bUnderline )
	{
		HFont font = GetFont();

		int px, py;
		GetPos(px, py);

		surface()->DrawSetTextFont(font);
		surface()->DrawSetTextColor(GetFgColor());
		
		TextImage *tx = GetTextImage();
		int maxWide;
		int fontTall = surface()->GetFontTall( font );
		int x = 0, y = fontTall - surface()->GetFontAscent( font, L'_' ) + 2;
		tx->GetSize( maxWide, x);
		x = 0;
		int charWide = surface()->GetCharacterWidth(font, '_');

		while ( x < maxWide )
		{
			if ( x + charWide < maxWide )
			{
				surface()->DrawSetTextPos(x , y );
				surface()->DrawUnicodeChar('_');
				x += charWide;
			}
			else // need to draw only part of this character, so it doesn't hang out the end of the string
			{
				surface()->DrawSetTextPos( maxWide - charWide , y );
				surface()->DrawUnicodeChar('_');
				x += charWide;				
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Returns a description of the label string
//-----------------------------------------------------------------------------
const char *URLLabel::GetDescription( void )
{
	static char buf[1024];
	_snprintf(buf, sizeof(buf), "%s, string URLText bool Underline", BaseClass::GetDescription());
	return buf;
}

//-----------------------------------------------------------------------------
// Purpose: scheme settings
//-----------------------------------------------------------------------------
void URLLabel::ApplySchemeSettings(IScheme *pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);
    SetCursor(dc_hand);
}

