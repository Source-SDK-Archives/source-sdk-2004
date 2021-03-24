$modelname player/urban_sample.mdl
$cdmaterials models\player\urban
$bbox -13 -13 0 13 13 72

$eyeposition 0 0 73
$opaque
$surfaceprop "flesh"

// whole body
$model Urban "urban_reference.smd" 

$include "bones1.qci"

$include "..\animation\ruleshierarchy.qci"
$include "..\animation\ct_attachments.qci"
$include "..\animation\hitbox.qci"

$includemodel player/cs_player_shared.mdl
$includemodel player/ak_anims_ct.mdl

$include "ragdoll.qci"



$lod 7 
{
	replacemodel "urban_reference" "urban_LOD_1" 
}


$shadowlod
{
	replacemodel "urban_reference" "urban_LOD_4" 

}
