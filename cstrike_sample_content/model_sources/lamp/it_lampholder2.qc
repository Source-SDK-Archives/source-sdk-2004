// Output .MDL
$modelname lamp/it_lampholder2.mdl

// Directory of .BMP
$cdmaterials models/cs_italy
$staticprop

// Write textures into a ???T.mdl
//$externaltextures

$scale 1.0

// whole body
$body studio "./it_lampholder2"

// sequences: all sequences are in $cd
$sequence idle "it_lampholder2" loop fps 15

$collisionmodel "it_lampholder2_phys.smd" {
     // Mass in kilograms
     $Mass 100
   $concave
}