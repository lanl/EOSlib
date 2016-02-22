fold::amrita {
  utilize CFD:EOSlib:Amrita::Build
  perl> \$root = "$Build::root"
}
require "$amrita::root/pl/amrita3.pl";
&amrita3();

$prog = "$bin/$AMRBIN/Convert";
$Data = "$EOSlib/DATA/EOS.data";
  $ldpath = $ENV{'LD_LIBRARY_PATH'};
  $ldpath .= ':' if $ldpath;
  $ldpath .=  "$lib/$AMRSO";
$ENV{'LD_LIBRARY_PATH'}= $ldpath;

$from = 'std';
$to = 'MKS';
# process argument list
while( $arg=shift(@ARGV) )
{
    if( $arg =~ /^\+data=(.*)/ )
    {
        $Data .= ":$1";
    }
    elsif( $arg =~ /^from=(.*)/ )
    {
        $from = $1;
    }
    elsif( $arg =~ /^to=(.*)/ )
    {
        $to = $1;
    }
	else
	{
	    unshift @ARGV, $arg;
        last;
	}
}
$args = join(" ",@ARGV);
system("$prog files $Data from $from to $to $args");
