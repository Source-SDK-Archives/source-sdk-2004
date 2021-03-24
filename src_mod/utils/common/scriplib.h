//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef SCRIPLIB_H
#define SCRIPLIB_H
#pragma once

#ifndef CMDLIB_H
#include "cmdlib.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

// scriplib.h

#define	MAXTOKEN	1024

extern	char	token[MAXTOKEN];
extern	char	*scriptbuffer,*script_p,*scriptend_p;
extern	int		grabbed;
extern	int		scriptline;
extern	qboolean	endofscript;


void LoadScriptFile (char *filename);
void ParseFromMemory (char *buffer, int size);

qboolean GetToken (qboolean crossline);
qboolean GetExprToken (qboolean crossline);
void UnGetToken (void);
qboolean TokenAvailable (void);

#ifdef __cplusplus
}
#endif

#endif // SCRIPLIB_H
