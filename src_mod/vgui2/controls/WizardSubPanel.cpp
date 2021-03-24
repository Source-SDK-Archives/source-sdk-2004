//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <vgui_controls/WizardPanel.h>
#include <vgui_controls/WizardSubPanel.h>
#include <vgui_controls/BuildGroup.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
WizardSubPanel::WizardSubPanel(Panel *parent, const char *panelName) : EditablePanel(parent, panelName), _wizardPanel(NULL)
{
	SetVisible(false);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
WizardSubPanel::~WizardSubPanel()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void WizardSubPanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("WizardSubPanel.BgColor",pScheme));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
KeyValues *WizardSubPanel::GetWizardData()
{
	return GetWizardPanel()->GetWizardData();
}

WizardSubPanel *WizardSubPanel::GetSiblingSubPanelByName(const char *pageName)
{
	return GetWizardPanel()->GetSubPanelByName(pageName);
}
