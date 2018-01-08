<?php
function mod_row(
  $tbl,
  $field,
  $val,
  $where_clause 
)
{
  $dbh = dbconn();
  if ( !$dbh ) { go_BYE(""); }

  $sql  = "update $tbl set $field = :$field ";
  $sql .= " where $where_clause ";
  $X[$field] = $val;
  $stmt = $dbh->prepare($sql);
  $rslt = $stmt->execute($X); assert($rslt );
  $pk = $dbh->lastInsertId();
  return $pk;
}
?>
