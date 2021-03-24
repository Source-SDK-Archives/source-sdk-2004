//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//=============================================================================

// No spaces in event names, max length 32
// All strings are case sensitive
//
// valid data key types are:
//   string : a zero terminated string
//   bool   : unsigned int, 1 bit
//   byte   : unsigned int, 8 bit
//   short  : signed int, 16 bit
//   long   : signed int, 32 bit
//   float  : float, 32 bit
//   local  : any data, but not networked to clients
//
// following key names are reserved:
//   local      : if set to 1, event is not networked to clients
//   unreliable : networked, but unreliable
//   suppress   : never fire this event
//   time	: firing server time
//   eventid	: holds the event ID

"cstrikeevents"
{
	"player_death"				// a game event, name may be 32 charaters long
	{
		// this extents the original player_death by a new 
		// field "headshot", all other fields remains the same
		"weapon"	"string" 	// weapon name killed used 
		"headshot"	"bool"		// singals a headshot
	}
	
	"bomb_planted"
	{
		"userid"	"short"		// player who planted the bomb
		"site"		"short"		// bombsite index
		"posx"		"short"		// position x
		"posy"		"short"		// position y
	}
	
	"bomb_defused"
	{
		"userid"	"short"		// player who defused the bomb
		"site"		"short"		// bombsite index
	}
	
	"bomb_exploded"
	{
		"userid"	"short"		// player who planted the bomb
		"site"		"short"		// bombsite index
	}
	
	"bomb_dropped"
	{
		"userid"	"short"		// player who dropped the bomb
	}
	
	"bomb_pickup"
	{
		"userid"	"short"		// player who picked up the bomb
	}

	"bomb_begindefuse"
	{
		"userid"	"short"		// player who picked up the bomb
		"haskit"	"bool"
	}
	
	"hostage_follows"
	{
		"userid"	"short"		// player who touched the hostage
		"hostage"	"short"		// hostage index
	}
	
	"hostage_hurt"
	{
		"userid"	"short"		// player who hurt the hostage
		"hostage"	"short"		// hostage index
	}
	
	"hostage_killed"
	{
		"userid"	"short"		// player who killed the hostage
		"hostage"	"short"		// hostage index
	}
	
	"hostage_rescued"
	{
		"userid"	"short"		// player who rescued the hostage
		"hostage"	"short"		// hostage index
		"site"		"short"		// rescue site index
	}
}
