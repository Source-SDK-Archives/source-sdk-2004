// Output .MDL
$modelname lamp/it_lampholder1.mdl

// Directory of .BMP
$cdmaterials models/cs_italy
$staticprop

// Write textures into a ???T.mdl
//$externaltextures

$scale 1.0

// whole body
$body studio "./it_lampholder1"

// sequences: all sequences are in $cd
$sequence idle "it_lampholder1" loop fps 15

$collisionmodel "it_lampholder1_phys.smd" {
     // Mass in kilograms
     $Mass 100
   $concave
}