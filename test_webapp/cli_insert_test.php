<?php
error_reporting(E_ALL);
set_include_path(get_include_path() . PATH_SEPARATOR . "../php/");

require_once 'insert_test.php';

if ( $argc != 2 ) { echo "Expected 3 arguments. Got $argc"; exit(1); }

$infile = $argv[1];

assert(is_file($infile));
$str_inJ = file_get_contents($infile);
$outJ = insert_test($str_inJ);
var_dump($outJ);
?>