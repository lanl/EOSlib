#!/usr/bin/env amrperl

fold::perl { fetch EOSlib variables
   use Cwd qw( chdir getcwd realpath );
   use Amrita::Interface;

   $cwd = &getcwd();
   $amrEOSpl = &realpath($0);
   chdir($amrEOSpl);
      fold::amrita {
        utilize EOSlib:Amrita::Build
      }
   chdir($cwd);
   
   $bin    = $amrita->get_token("Build::bin");
   $lib    = $amrita->get_token("Build::lib");
   $EOSlib = $amrita->get_token("Build::EOSlib");
   $AMRBIN = $amrita->get_token("amrita::AMRBIN");
   $AMRSO  = $amrita->get_token("amrita::AMRSO");
}
$prog = shift(@ARGV);
die "amrEOS.pl: ERROR, no arguments" unless $prog;
$Prog = "$bin/$AMRBIN/$prog";
die "amrEOS.pl: ERROR, no executable $prog" unless -x $Prog;
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
system("$Prog file $file $args");
