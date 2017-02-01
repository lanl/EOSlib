eval 'exec perl -w $0 ${1+"$@"}'
   if 0;

#!/usr/bin/perl -w
use Tk;
require 'xmgrace.pl';
use List::Util qw( min max );


# quantities needed to get -state=>'disabled' to behave properly
# set in &BottomFrame();
local $disabledforeground;
local $foreground;
local $highlightthickness;

# set directory paths to find perl routines

BEGIN{
    #(my $DIR = $0) =~ s!/[^/]*$!!;
    use FindBin;
    $DIR = $FindBin::Bin;
    my $p;
    foreach $p ( qw( lib lib/Data lib/Material lib/Riemann ) )
    {
         push @libs, "$DIR/$p";
    }
}
use File::Basename;
use Cwd qw( chdir getcwd abs_path );

use lib @libs;
use State;
use Material;
use Riemann;
use DataBase;

require 'StatusLine.pl';
require 'BottomFrame.pl';

#

sub cleanup;
$SIG{QUIT} = \&cleanup;
$SIG{INT}  = \&cleanup;
$SIG{TERM} = \&cleanup;

# set paths

#(local $DIR = $0) =~ s!/[^/]*$!!;
#(local $basename = $0) =~ s!^.*/([^/]*)$!$1!;
#(local $program = $basename) =~ s!\..*$!!;
#local $EOSlib=`cd $DIR/..; pwd`;
#chop $EOSlib;
local $basename = basename($0);
local $program = ($basename =~ /\.[^.]*/) ? $` : $basename;
#local $EOSlib = abs_path("$DIR/..");
#local $Run = $EOSlib . "/Run";
local $Run = 'runEOS';


local $PATH = $ENV{"PATH"};
my @dirs = Path("env", $PATH);
die "Could not find `env' not in PATH\n" if $#dirs < 0 ;
local $env = &FullPath($dirs[0]) . "/env";
undef @dirs;

# initialize variable

local ($status_message, $OldStatus) = ();
local ($DataFile, $units, $Material) = ();
local @DataFiles = ();
local $nsteps = 100;	# number of points computed for wave locus
local $plot = 0;	# index for plot
local $tag = $program;  # name for temporary plot files

my %SetArgs = (
		DataFile  => 'scalar',
		DataFiles => 'array',
		Material  => 'scalar',
		nsteps    => 'scalar',
		tag       => 'scalar',
		
	);

# process argument list

my $args;
for $arg (@ARGV)
{
	my ($var, $value) = split /=/, $arg, 2;
	die "$program, ERROR: argument \`$arg' not of form var=value\n"
			unless defined $value;
	my $type = $SetArgs{$var};
	SWITCH: {
	  $type =~ /array/  and @$var = split(/,/, $value), last SWITCH;
	  $type =~ /scalar/ and $$var = $value,                last SWITCH;
	  $type and $$var = $type,                             last SWITCH;
	  die "$program, ERROR: unknown argument \`$arg'";
	}
}

# Temporary files in tmp/$tag$$*
if( not -d 'tmp' )
{
	mkdir('tmp',0777) || die "Could not make tmp directory";
}
$tag = 'tmp/' . $tag . $$;
$pwd = `pwd`;
chop $pwd;
#$FIFO=$pwd . '/tmp/' . $$ . 'fifo';
#system('mknod', $FIFO, 'p') && die "can't mknod $FIFO: $!";
#@plotid;	# pid of plots

# defaults
unshift @DataFiles, split(/,/,$DataFile) if( $DataFile );
@DataFiles = ( $ENV{'EOSdata'} ) if( $#DataFiles < 0 );
&RemoveDuplicates(\@DataFiles);


# Start

$mw = MainWindow->new;
$mw->geometry("-10+5");
$mw->focusFollowsMouse();
$mw->fontCreate('fixed',
        -family=>'Monospace 15',
        -size=>15);
$mw->optionAdd('*font' => 'fixed');

$status_message = "Initializing";
&StatusLine;
&BottomFrame;

$DataFrame = $mw->Frame()
	->pack( -side=>'top', -anchor=>'w', -expand=>1, -fill=>'x' );
local $Data = DataBase->new($DataFrame);


if( $#DataFiles >= 0 )
{
	$Data->InsertFiles( @DataFiles );
	unless( $Data->FileSelected($DataFiles[0]) )
	{
		my $mat = $Data->{Materials}->[0];
		my $MaterialEntry = $mat->{MaterialEntry};
		$MaterialEntry->focusForce();
		$MaterialEntry->icursor('end');
		$mat->MaterialSelected($Material) if $Material;
	}
}
else
{
	$Data->{FileEntry}->focusForce();
	$Data->{FileEntry}->icursor('end');
}

MainLoop;

&cleanup;

sub cleanup
{
    print "$program terminated\n";
    $Data->DESTROY;
	$mw->destroy if Exists($mw);
	#remove temporary files
	#close FIFO;
	#unlink $FIFO;
	unlink <$tag*>;

	#print "plotid's = ", join(", ",@plotid), "\n";
        #kill 'TERM', @plotid if scalar @plotid;
        $SIG{TERM} = 'IGNORE';
	kill 'KILL', 0;	# current process group
	exit;
}


#Circumvent compiler warning messages "only used once"
undef $status_message;
undef $OldStatus;
undef $units;
undef $disabledforeground;
undef $foreground;
undef $highlightthickness;
undef $nsteps;
undef $plot;
undef $env;

#
# usage: &RemoveDuplicates(\@array)
#        Removes duplicate elements while preserving order
#
sub RemoveDuplicates
{
	my $array = shift;
	my $elem;
	my @a;
	my %list = ();
	
	for $elem (@$array)
	{
		if( not $list{$elem} )
		{
			push @a, $elem;
			$list{$elem} = 1;
		}
	}
	@$array = @a;
}

#
# usage: &InList($word, \&list)
#        Returns 1 if $word is element of @list, otherwise 0
#
sub InList
{
	my ($word, $list) = @_;
	my $w;
	for $w ( @$list )
	{
		return 1 if $word eq $w;
	}
	return 0;
}

#
# usage: &InList($word, \&list)
#        Returns index of $word in @list, otherwise -1
#
sub ListIndex
{
	my ($word, $list) = @_;
	my $i = 0;
	my $w;
	for $w ( @$list )
	{
		return $i if $word == $w;
		$i++;
	}
	return -1;
}

#
# usage: &IsNum($word)
#        Returns 1 if string $word represents a number, otherwise 0
#
sub IsNum
{
	return 0 if not defined $_[0];
	return $_[0] =~ /^[+-]?((0|[1-9]\d*)(\.\d*)?|\.\d+)([eE][+-]?\d+)?$/;
}


#
# usage: @Path($prog,$list)
#	 Returns list of paths for "prog" in `:' list of paths
#

sub Path
{
	my ($prog, $list) = @_;
	for $dir ( split(/:/,$list) )
	{
		my $path = $dir . "/" . $prog;
		push @list, $dir if -x $path;
	}
	return @list;
}

#
# usage: @FullPath($dir)
#	 Returns absolute path of directory dir
#

sub FullPath
{
	return undef unless $_[0];
	my $FullDir = `cd $_[0]; pwd`;
	chop $FullDir;
	return $FullDir;
}

1;

__END__

