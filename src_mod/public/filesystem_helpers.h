//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef FILESYSTEM_HELPERS_H
#define FILESYSTEM_HELPERS_H
#ifdef _WIN32
#pragma once
#endif


// Call until it returns NULL. Each time you call it, it will parse out a token.
const char* ParseFile( const char* pFileBytes, char* pToken, bool* pWasQuoted );
char* ParseFile( char* pFileBytes, char* pToken, bool* pWasQuoted );	// (same exact thing as the const version)


#endif // FILESYSTEM_HELPERS_H
