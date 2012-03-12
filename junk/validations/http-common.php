<?

function http_get($varname, $regex, $default = null)
{
  $r = null;
  
  if (isset($_GET{$varname}))
    $r = http_vars_aux('GET', $varname, $regex);

  if ($r == null and $default == null)
    die ('wanted _GET ' . $varname);
  else if ($r == null)
    $r = $default;

  return $r;
}

function http_post($varname, $regex, $default = null)
{
  $r = null;

  if (isset($_POST{$varname}))
    $r = http_vars_aux('POST', $varname, $regex);


  if ($r == null && $default == null)
    die ('wanted _POST ' . $varname);
  else if ($r == null)
    $r = $default;

  return $r;
}

function http_any ($varname, $regex, $default = null)
{
  $r = null;

  if (isset($_GET{$varname}))
    $r = http_vars_aux('GET', $varname, $regex);
  else if (isset($_POST{$varname}))
    $r = http_vars_aux('POST', $varname, $regex);

  if ($r == null && $default == null)
    die ('wanted _POST or _GET ' . $varname);
  else if ($r == null)
    $r = $default;

  return $r;
}

function http_vars_aux ($method, $http_varname, $regex)
{
  if (eval("return isset (\$_$method {'$http_varname'});"))
  {
    $val = eval("return \$_$method {'$http_varname'};");
  }
  else
  {
    die("I wanted to get : " . $http_varname);
  }

  if (!preg_match("/$regex/", $val))
    die("Variable '$http_varname' ($method) is poisoned : $val");

  return $val;
}

?>
