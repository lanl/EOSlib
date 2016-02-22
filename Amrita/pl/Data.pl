fold::amrita {
  utilize CFD:EOSlib:Amrita::Build
  perl> \$root = "$Build::root"
}
require "$amrita::root/pl/amrita3.pl";
&amrita3();

$prog = "$bin/$AMRBIN/Data";
$Data = "$EOSlib/DATA/EOS.data";
  $ldpath = $ENV{'LD_LIBRARY_PATH'};
  $ldpath .= ':' if $ldpath;
  $ldpath .=  "$lib/$AMRSO";
$ENV{'LD_LIBRARY_PATH'}= $ldpath;

$base = 'EOS';
$type = 'Hayes';
$name = '*';
# process argument list
while( $arg=shift(@ARGV) )
{
    if( $arg =~ /^\+data=(.*)/ )
    {
        $Data .= ":$1";
    }
    elsif( $arg =~ /^base=(.*)/ )
    {
        $base = $1;
    }
    elsif( $arg =~ /^type=(.*)/ )
    {
        $type = $1;
    }
    elsif( $arg =~ /^name=(.*)/ )
    {
        $name = $1;
    }
    elsif( $arg =~ /^plain$/ )
    {
        $parg = $arg;
    }
    elsif( $arg =~ /^parameters$/ )
    {
        $parg = $arg;
    }
    elsif( $arg =~ /^use$/ )
    {
        $parg = $arg;
    }
	else
	{
	    die "unknown argument `$arg'";
	}
}
system("$prog files $Data base '$base' type '$type' name '$name' $parg");
