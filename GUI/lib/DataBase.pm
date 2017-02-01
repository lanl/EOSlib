package DataBase;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw( InsertFiles FileSelected );

require Tk::BrowseEntry;

#use lib 'lib/Data';
	require 'DataEntry.pl';
	require 'FileSelected.pl';
	require 'ListDataFile.pl';
	require 'ListUnits.pl';
	require 'NewMaterialWindow.pl';

=class DataBase
frame = (Frame widget)

DataFile (string) = selected file in DataFiles list
DateFiles [list of files]

DataBase  (hash) = material data base corresponding to DataFile
                keys are 'materials' (material=type::name)
               	values are 'units'
DataBases (hash) = "index" for available files
		keys are files
		values are DataBase
		
units (string) = selected units
Units (hash)   = available units in database file
		keys are files
		values are [list of units]

Riemann		= class Riemann
EntryFrame (Frame widget)
  DataButton (Button Widget)
  FileEntry  (BrowseEntry widget)
  UnitsEntry (BrowseEntry widget)
  NewMatreialButton (Button Widget)
    
Materials [list]  = class Material
=cut


#local subroutines
sub DataEntry;
sub UnitsList;
sub ReadFile;
sub UnitsSelected;
sub UnitsReset;
sub FileEntryIndex;

sub NewWindow;
sub RemoveMaterial;

sub new
{
	my $data = {};
	my $frame = $data->{frame} = $_[1];
	
	$data->{DataFiles} = [];
	$data->{DataBases} = {};
	$data->{Units}     = {};
	$data->{Materials} = [];
	$data->{DataBase}  = {};
	bless $data;
	
	$data->{EntryFrame} = $frame->Frame()
		 ->pack( -side=>'top',-anchor=>'w',-fill=>'x',-expand=>1);		
	$data->DataEntry;
	
	my $canvas = $frame->Scrolled('Canvas', -scrollbars=>'osow')
		->pack( -side=>'top',-anchor=>'w',-fill=>'x',-expand=>1);
	my $MaterialFrame = $canvas->Frame();
	my $RiemannFrame = $canvas->Frame();
	
	$data->{Riemann} = Riemann->new($RiemannFrame);		
	my $mat = {};
	$mat->{'Parent'}   = $data;
	$mat->{'DataBase'} = $data->{DataBase};
	$mat->{'DataFile'} = \$data->{DataFile};
	$mat->{'units'}    = \$data->{units};
	$mat->{'Riemann'}  = $data->{Riemann};
	$mat->{'frame'}    = $MaterialFrame;
	$mat = Material->new($mat);
	push( @{$data->{Materials}}, $mat );
	$data->{Riemann}->StateChanged;

	#my $MframeWidth = 630;
	#my $MframeHeight= 450;
	#my $RframeWidth = 630;
	#my $RframeHeight= 270;
	my $MframeWidth = 900+10;
	my $MframeHeight= 500;
	my $RframeWidth = 900;
	my $RframeHeight= 350;
	my $width = $MframeWidth > $RframeWidth ? $MframeWidth : $RframeWidth;
	my $height = $MframeHeight + $RframeHeight;
	$canvas->configure( -width=>$width, -height=>$height,
	                   -scrollregion=>[0,0, $width, $height] );
	$canvas->createWindow(0,0, -window=>$MaterialFrame,
		-anchor=>'nw', -width=>$MframeWidth, -height=>$MframeHeight );
	$canvas->createWindow(0,$MframeHeight, -window=>$RiemannFrame,
		-anchor=>'nw', -width=>$RframeWidth, -height=>$RframeHeight );
		
	return $data;
}

sub DeleteMaterial
{
	my ($data, $mat) = @_;
 	my $Materials = $data->{Materials};
	my $index = &::ListIndex($mat,$Materials);
	splice( @{$Materials}, $index, 1) if $index >= 0;
	# undef $mat; # does not deallocate Material class
	$mat->DESTROY;	# empty storage from hash
	# Upon leaving this routine,
	# perl deallocates $mat and calls DESTROY again
}

sub DESTROY
{
    my ($data) = @_;
    my $riemann = $data->{Riemann};
    #$riemann->{LeftMaterial} = undef;
    #$riemann->{RightMaterial} = undef;
    untie $riemann->{LeftMaterial};
    untie $riemann->{RightMaterial};
    #
 	my $Materials = $data->{Materials};
    while( $mat = pop @{$Materials} )
    {
        $mat->DESTROY;
    }
}


1;

__END__



