$input_string = $ARGV[0];

$i = index($input_string,"\\n");
if ( $i == -1 )
{
  print $input_string;
}
else
{
  while ( length($input_string) != 0)
  {
    $i = index($input_string,"\\n");
    $line = substr($input_string,0,$i);
    print "$line\n";
    $input_string = substr($input_string,$i+2,length($input_string));
  }
}
