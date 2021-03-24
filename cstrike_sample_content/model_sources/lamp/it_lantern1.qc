$modelname lamp/it_lantern1.mdl

$cdmaterials models/cs_italy
$staticprop
$surfaceprop "glass"
$scale 1.0

$body studio "./it_lantern1"

$sequence idle "it_lantern1" loop fps 15

$collisionmodel "it_lantern1_phys.smd" {
     $Mass 100
   $concave
}