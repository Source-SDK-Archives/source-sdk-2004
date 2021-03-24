//  events on movements use the attachments lfoot and rfoot on the players feet to create water effects as the player walks
//  formatting for this is { event, generic event number, frame number of the animation to play the event on, name of the event to play, which attachment to play the event from

$modelname player/cs_player_shared.mdl
$include "../urban/bones1.qci"
$include "ruleshierarchy.qci"


//====================================================
//
//	LOWER BODY MOVEMENT ANIMATION
//
//====================================================

	//-----------------
	//	Idles
	//-----------------

		$sequence Idle_lower "Idle01" loop FPS 30 activity ACT_IDLE 1
		$sequence Crouch_Idle_Lower "Crouch_idle01" loop FPS 30 activity ACT_CROUCHIDLE 1
		$sequence menu_pose "menu_pose" loop fps 30 origin 0 0 -115 activity ACT_MENUPOSE 1

	//-----------------
	//	Run
	//-----------------

		$animation a_RunS RunS startloop 0 LX LY alignto Idle_lower rotateto -180
		$animation a_RunSE RunSE startloop 0 LX LY alignto Idle_lower rotateto -135
		$animation a_RunE RunE startloop 0 LX LY alignto Idle_lower rotateto -90
		$animation a_RunNE RunNE startloop 0 LX LY alignto Idle_lower rotateto -45
		$animation a_RunN RunN startloop 0 LX LY alignto Idle_lower rotateto 0
		$animation a_RunNW RunNW startloop 0 LX LY alignto Idle_lower rotateto 45
		$animation a_RunW RunW startloop 0 LX LY alignto Idle_lower rotateto 90
		$animation a_RunSW RunSW startloop 0 LX LY alignto Idle_lower rotateto 135
		$animation a_RunZero "9run_idle" loop

/*		$sequence Run_lower {
			a_RunS a_RunSE a_RunE
			a_RunNE a_RunN a_RunNW
			a_RunW a_RunSW a_RunS
			blendwidth 9 blend move_yaw -180 180
			loop 
			ACT_RUN 1
		}
*/
		$sequence Run_lower {
			a_RunNW a_RunN a_RunNE 
			a_RunW a_RunZero a_RunE 
			a_RunSW a_RunS a_RunSE 
			blendwidth 3 blend move_y -1 1 blend move_x 1 -1 loop
			ACT_RUN 1
			{ event 7001 14 "lfoot" }
			{ event 7001 5 "rfoot" }
		}

	
	//-----------------
	//	Walk
	//-----------------

		$animation a_WalkS WalkS startloop 0 LX LY loop alignto Idle_lower rotateto -180
		$animation a_WalkSE WalkSE startloop 0 LX LY loop alignto Idle_lower rotateto -135
		$animation a_WalkE WalkE startloop 0 LX LY loop alignto Idle_lower rotateto -90
		$animation a_WalkNE WalkNE startloop 0 LX LY loop alignto Idle_lower rotateto -45
		$animation a_WalkN WalkN startloop 0 LX LY loop alignto Idle_lower rotateto 0
		$animation a_WalkNW WalkNW startloop 0 LX LY loop alignto Idle_lower rotateto 45
		$animation a_WalkW WalkW startloop 0 LX LY loop alignto Idle_lower rotateto 90
		$animation a_WalkSW WalkSW startloop 0 LX LY loop alignto Idle_lower rotateto 135
		$animation a_walkZero "9walk_idle" loop

/*		$sequence Walk_lower {
			a_WalkS a_WalkSE a_WalkE a_WalkNE a_WalkN a_WalkNW a_WalkW a_WalkSW a_WalkS
			blendwidth 9 blend move_yaw -180 180
			ACT_WALK 1
		}
*/

		$sequence walk_lower {
			a_walkNW a_walkN a_walkNE 
			a_walkW a_walkZero a_walkE 
			a_walkSW a_walkS a_walkSE 
			blendwidth 3 blend move_y -1 1 blend move_x 1 -1 loop
			ACT_WALK 1
			{ event 7002 15 "lfoot" }
			{ event 7002 4 "lfoot" }
		}


	//-----------------------
	//	Crouch_Walk
	//-----------------------

		$animation a_Crouch_walkS Crouch_walkS startloop 0 LX LY alignto Idle_lower rotateto -180
		$animation a_Crouch_walkSE Crouch_walkSE startloop 0 LX LY alignto Idle_lower rotateto -135
		$animation a_Crouch_walkE Crouch_walkE startloop 0 LX LY alignto Idle_lower rotateto -90
		$animation a_Crouch_walkNE Crouch_walkNE startloop 0 LX LY alignto Idle_lower rotateto -45
		$animation a_Crouch_walkN Crouch_walkN startloop 0 LX LY alignto Idle_lower rotateto 0
		$animation a_Crouch_walkNW Crouch_walkNW startloop 0 LX LY alignto Idle_lower rotateto 45
		$animation a_Crouch_walkW Crouch_walkW startloop 0 LX LY alignto Idle_lower rotateto 90
		$animation a_Crouch_walkSW Crouch_walkSW startloop 0 LX LY alignto Idle_lower rotateto 135
		$animation a_Crouch_walkZero "9crouchwalk_idle" loop

/*		$sequence Crouch_Walk_lower {
			a_Crouch_walkS a_Crouch_walkSE a_Crouch_walkE
			a_Crouch_walkNE a_Crouch_walkN a_Crouch_walkNW 
			a_Crouch_walkW a_Crouch_walkSW a_Crouch_walkS
			blendwidth 9 blend move_yaw -180 180
			ACT_RUN_CROUCH 1
		}
*/
		$sequence Crouch_walk_lower {
			a_Crouch_walkNW a_Crouch_walkN a_Crouch_walkNE 
			a_Crouch_walkW a_Crouch_walkZero a_Crouch_walkE 
			a_Crouch_walkSW a_Crouch_walkS a_Crouch_walkSE 
			blendwidth 3 blend move_y -1 1 blend move_x 1 -1 loop
			ACT_RUN_CROUCH 1
			{ event 7002 17 "lfoot" }
			{ event 7002 5 "rfoot" }
		}


	//---------------
	//	jump
	//---------------

		$sequence Jump "jump" fps 30 alignto Idle_lower activity ACT_HOP 1 { event 7001 23 "lfoot" } { event 7001 23 "rfoot" }

//====================================================
//
//		Weapon Specific Animation
//
//====================================================


	//------------------
	//	Pistols
	//------------------

/*		$sequence Crouch_Idle_Aim_Pistol  { 
			Idle_Pistol_aim_up_right Idle_Pistol_aim_up_center Idle_Pistol_aim_up_left 
			Idle_Pistol_aim_mid_right Idle_Pistol_aim_mid_center Idle_Pistol_aim_mid_left 
			Idle_Pistol_aim_down_right Idle_Pistol_aim_down_center Idle_Pistol_aim_down_left 
			weightlist nolowerbody
			blendwidth 3 blend body_yaw -90 90 blend body_pitch -65 65
			iklock lfoot 1 0 iklock rfoot 1 0
			delta
		}
*/

		$animation Run_Pistol_aim_neutral Run_9aims_Pistol frame 4 4
		$animation Run_Pistol_aim_up_left Run_9aims_Pistol frame 0 0 subtract Run_Pistol_aim_neutral 0
		$animation Run_Pistol_aim_up_center Run_9aims_Pistol frame 1 1 subtract Run_Pistol_aim_neutral 0
		$animation Run_Pistol_aim_up_right Run_9aims_Pistol frame 2 2 subtract Run_Pistol_aim_neutral 0
		$animation Run_Pistol_aim_mid_left Run_9aims_Pistol frame 3 3 subtract Run_Pistol_aim_neutral 0
		$animation Run_Pistol_aim_mid_center Run_9aims_Pistol frame 4 4 subtract Run_Pistol_aim_neutral 0
		$animation Run_Pistol_aim_mid_right Run_9aims_Pistol frame 5 5 subtract Run_Pistol_aim_neutral 0
		$animation Run_Pistol_aim_down_left Run_9aims_Pistol frame 6 6 subtract Run_Pistol_aim_neutral 0
		$animation Run_Pistol_aim_down_center Run_9aims_Pistol frame 7 7 subtract Run_Pistol_aim_neutral 0
		$animation Run_Pistol_aim_down_right Run_9aims_Pistol frame 8 8 subtract Run_Pistol_aim_neutral 0

		$sequence Run_Aim_Pistol  { 
			Run_Pistol_aim_up_right Run_Pistol_aim_up_center Run_Pistol_aim_up_left 
			Run_Pistol_aim_mid_right Run_Pistol_aim_mid_center Run_Pistol_aim_mid_left 
			Run_Pistol_aim_down_right Run_Pistol_aim_down_center Run_Pistol_aim_down_left 
			weightlist nolowerbody
			blendwidth 3 blend body_yaw -90 90 blend body_pitch -65 65
			iklock lfoot 1 0 iklock rfoot 1 0
			delta
		}


		$sequence Idle_Upper_Pistol "Idle_Counter_Pistol" FPS 30 loop weightlist upperbody addlayer Run_Aim_Pistol
		$sequence Walk_Upper_Pistol "Walk_Counter_Pistol" FPS 30 weightlist upperbody addlayer Run_Aim_Pistol
		$sequence Run_Upper_Pistol "Run_Counter_Pistol" FPS 30 loop weightlist upperbody addlayer Run_Aim_Pistol
		$sequence Crouch_Idle_Upper_Pistol "Crouch_Idle_Counter_Pistol" FPS 30 weightlist upperbody addlayer Run_Aim_Pistol
		$sequence Crouch_Walk_Upper_Pistol "Crouch_Walk_Counter_Pistol" FPS 30 weightlist upperbody addlayer Run_Aim_Pistol


		$animation Idle_Shoot_Pistol_layer "Run_Shoot_Pistol" FPS 30 subtract Idle_Shoot_Pistol_layer 0 weightlist upperbody
		$sequence Idle_Shoot_Pistol "Idle_Shoot_Pistol_layer" delta
		$sequence Walk_Shoot_Pistol "Idle_Shoot_Pistol_layer" delta
		$sequence Run_Shoot_Pistol "Idle_Shoot_Pistol_layer" delta
		$sequence Crouch_Idle_Shoot_Pistol "Idle_Shoot_Pistol_layer" delta
		$sequence Crouch_Walk_Shoot_Pistol "Idle_Shoot_Pistol_layer" delta


		$animation Reload_Pistol_layer "Idle_Reload_Pistol" FPS 30 subtract Reload_Pistol_layer 0 weightlist upperbody
		$sequence Reload_Pistol "Reload_Pistol_layer" delta


	//-----------------
	//	M4
	//-----------------

		$animation Crouch_M4_aim_neutral Crouch_9aims_m4 frame 4 4
		$animation Crouch_M4_aim_up_left Crouch_9aims_m4 frame 0 0 subtract Crouch_M4_aim_neutral 0
		$animation Crouch_M4_aim_up_center Crouch_9aims_m4 frame 1 1 subtract Crouch_M4_aim_neutral 0
		$animation Crouch_M4_aim_up_right Crouch_9aims_m4 frame 2 2 subtract Crouch_M4_aim_neutral 0
		$animation Crouch_M4_aim_mid_left Crouch_9aims_m4 frame 3 3 subtract Crouch_M4_aim_neutral 0
		$animation Crouch_M4_aim_mid_center Crouch_9aims_m4 frame 4 4 subtract Crouch_M4_aim_neutral 0
		$animation Crouch_M4_aim_mid_right Crouch_9aims_m4 frame 5 5 subtract Crouch_M4_aim_neutral 0
		$animation Crouch_M4_aim_down_left Crouch_9aims_m4 frame 6 6 subtract Crouch_M4_aim_neutral 0
		$animation Crouch_M4_aim_down_center Crouch_9aims_m4 frame 7 7 subtract Crouch_M4_aim_neutral 0
		$animation Crouch_M4_aim_down_right Crouch_9aims_m4 frame 8 8 subtract Crouch_M4_aim_neutral 0


		$sequence Crouch_Aim_M4  { 
			Crouch_M4_aim_up_right Crouch_M4_aim_up_center Crouch_M4_aim_up_left 
			Crouch_M4_aim_mid_right Crouch_M4_aim_mid_center Crouch_M4_aim_mid_left 
			Crouch_M4_aim_down_right Crouch_M4_aim_down_center Crouch_M4_aim_down_left 
			weightlist upperbody
			blendwidth 3 blend body_yaw -90 90 blend body_pitch -65 65
			iklock lfoot 1 0 iklock rfoot 1 0
			delta
		}


		$animation Run_M4_aim_neutral Run_9aims_M4 frame 4 4
		$animation Run_M4_aim_up_left Run_9aims_M4 frame 0 0 subtract Run_M4_aim_neutral 0
		$animation Run_M4_aim_up_center Run_9aims_M4 frame 1 1 subtract Run_M4_aim_neutral 0
		$animation Run_M4_aim_up_right Run_9aims_M4 frame 2 2 subtract Run_M4_aim_neutral 0
		$animation Run_M4_aim_mid_left Run_9aims_M4 frame 3 3 subtract Run_M4_aim_neutral 0
		$animation Run_M4_aim_mid_center Run_9aims_M4 frame 4 4 subtract Run_M4_aim_neutral 0
		$animation Run_M4_aim_mid_right Run_9aims_M4 frame 5 5 subtract Run_M4_aim_neutral 0
		$animation Run_M4_aim_down_left Run_9aims_M4 frame 6 6 subtract Run_M4_aim_neutral 0
		$animation Run_M4_aim_down_center Run_9aims_M4 frame 7 7 subtract Run_M4_aim_neutral 0
		$animation Run_M4_aim_down_right Run_9aims_M4 frame 8 8 subtract Run_M4_aim_neutral 0

		$sequence Run_Aim_M4  { 
			Run_M4_aim_up_right Run_M4_aim_up_center Run_M4_aim_up_left 
			Run_M4_aim_mid_right Run_M4_aim_mid_center Run_M4_aim_mid_left 
			Run_M4_aim_down_right Run_M4_aim_down_center Run_M4_aim_down_left 
			weightlist upperbody
			blendwidth 3 blend body_yaw -90 90 blend body_pitch -65 65
			iklock lfoot 1 0 iklock rfoot 1 0
			delta
		}


		$sequence Idle_Upper_M4 "Idle_Counter_M4" FPS 30 weightlist upperbody addlayer Run_Aim_M4
		$sequence Walk_Upper_M4 "Walk_Counter_M4" FPS 30 weightlist upperbody addlayer Run_Aim_m4
		$sequence Run_Upper_M4 "Run_Counter_M4" FPS 30 startloop 0 weightlist upperbody addlayer Run_Aim_M4
		$sequence Crouch_Idle_Upper_M4 "Crouch_Idle_Counter_M4" FPS 30 weightlist upperbody addlayer Crouch_Aim_m4
		$sequence Crouch_Walk_Upper_M4 "Crouch_Walk_Counter_m4" FPS 30 weightlist upperbody addlayer Crouch_Aim_M4


		$animation Idle_Shoot_M4_layer "Run_shoot_m4" FPS 30 subtract Idle_Shoot_M4_layer 0 weightlist upperbody
		$sequence Idle_Shoot_M4 "Idle_Shoot_M4_layer" delta
		$sequence Walk_Shoot_M4 "Idle_Shoot_M4_layer" delta
		$sequence Run_Shoot_M4 "Idle_Shoot_M4_layer" delta
		$sequence Crouch_Idle_Shoot_M4 "Idle_Shoot_M4_layer" delta
		$sequence Crouch_Walk_Shoot_M4 "Idle_Shoot_M4_layer" delta


		$animation Reload_M4_layer "Idle_Reload_M4" FPS 30 subtract Reload_M4_layer 0 weightlist upperbody
		$sequence Reload_M4 "Reload_M4_layer" delta
