fold::amrita {
  utilize CFD:EOSlib:Amrita::Build
  perl> \$root = "$Build::root"
}
require "$amrita::root/pl/amrita3.pl";
&amrita3();

$prog = "$bin/$AMRBIN/Wave";
$file = "$EOSlib/DATA/EOS.data";
$ENV{'SharedLibDirEOS'} = "$lib/$AMRSO";
  $ldpath = $ENV{'LD_LIBRARY_PATH'};
  $ldpath .= ':' if $ldpath;
  $ldpath .=  "$lib/$AMRSO";
$ENV{'LD_LIBRARY_PATH'}= $ldpath;
if( $ARGV[0] =~  /^\+file=(.*)/ )
{
   $file .= ":$1";
   shift(@ARGV);
}
$args = join(" ",@ARGV);
system("$prog file $file $args");
