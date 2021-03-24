// Output .MDL
$modelname lamp/it_streetlampleg.mdl

// Directory of .BMP
$cdmaterials models/cs_italy
$staticprop

// Write textures into a ???T.mdl
//$externaltextures

$scale 1.0

// whole body
$body studio "./it_streetlampleg"

// sequences: all sequences are in $cd
$sequence idle "it_streetlampleg" loop fps 15

$collisionmodel "it_streetlampleg_phys.smd" {
     // Mass in kilograms
     $Mass 100
   $concave
}