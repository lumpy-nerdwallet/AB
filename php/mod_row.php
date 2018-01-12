<?php
require_once "dbconn.php";
function mod_row(
  $tbl,
  $kvs,
  $where_clause
)
{
  $dbh = dbconn();
  if ( !$dbh ) { go_BYE(""); }

  $sql  = "update $tbl set ";
  $first = true;
  $save_kvs = $kvs;
  foreach ( $kvs as $key => $val ) {
    if ( is_null($val) ) { continue; }
    if ( $first ) { 
      $first = false;
    }
    else {
      $sql .= ", ";
    }
    $sql .= " $key = :$key ";
    $X["$key"] = $val;
  }
  $sql  .= $where_clause;
  $stmt = $dbh->prepare($sql);
  var_dump($sql);
  var_dump($X);
  $rslt = $stmt->execute($X); if ( !$rslt ) { go_BYE(""); }
  return true;
}
$tbl = "variant";
$X['custom_data'] = "some custom data";
$X['description'] = "some description data";
mod_row($tbl, $X, "where id = 1");
?>