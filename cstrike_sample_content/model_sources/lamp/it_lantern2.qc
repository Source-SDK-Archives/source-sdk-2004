// Output .MDL
$modelname lamp/it_lantern2.mdl

// Directory of .BMP
$cdmaterials models/cs_italy
$staticprop

// Write textures into a ???T.mdl
//$externaltextures

$scale 1.0

// whole body
$body studio "./it_lantern2"

// sequences: all sequences are in $cd
$sequence idle "it_lantern2" loop fps 15

$collisionmodel "it_lantern2_phys.smd" {
     // Mass in kilograms
     $Mass 100
   $concave
}