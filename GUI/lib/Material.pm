package Material;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw( MaterialList  MaterialSelected
              MaterialError SetRiemannState_rb
	    );

#use lib 'lib/Material';	    
require 'MaterialEntryFrame.pl';	    
require 'RefStateFrame.pl';
require 'InitStateFrame.pl';
require 'LocusFrame.pl';
require 'MaterialRPFrame.pl';

require 'RefStateTie.pl';
require 'InitStateTie.pl';

#
=class Material
*DataFile (string)
DataBase (hash) keys are 'materials'
                values are 'units'
*units    (string)
Parent	  class DataBase
Riemann	  class
frame    (Frame widget)
----------------------------------------
EntryFrame  (Frame widget)
  MaterialEntry  (BrowseEntry widget)
    Material     (string)
    OldMat	 (string)
  MaterialButton (labeled 'Parameters')
----------------------------------------
RefStateFrame  (Frame widget)
  RefHeaderFrame (Frame widget)
  RefVarFrame    (Frame widget)
    default_rb   (radio button widget - default)
    Veu_rb       (radio button widget - Veu)
    PTu_rb       (radio button widget - PTu)
    InitState_rb (radio button widget - InitState)
    RiemannState_rb (radio button widget - RiemannState)
    ref_state 			(value of button ('default'))
    UpdateButton (labeled 'Update state')
  RefState	 (class State)
  SetRefState (disabled, active, set)  
----------------------------------------
InitStateFrame  (Frame widget)
  InitHeaderFrame (Frame widget)
  InitVarFrame    (Frame widget)
    RefState_rb      (radio button widget - 'ref_state')
    HugState_rb      (radio button widget - 'InitShock')
    IsentropState_rb (radio button widget - 'InitIsentrope')
    init_state  		(value of button)
    LeftInitState_rb	(radio button widget - 'init_left')
    RightInitState_rb	(radio button widget - 'init_right')
    init_state_dir		(value of button)
  InitState	(class State)
  InitStateMaterial
  SetInitState (disabled, active, set)
----------------------------------------
LocusFrame	(Frame widget)
  LocusSelectFrame 	(Frame widget)
  LocusLabel  		(Label widget)
  LocusHugoniot_rb	(radio button widget - 1)
  LocusIsentope_rb	(radio button widget - 2)
  locus			(value of button)
  ---------------------- 
  WaveFamilyFrame	(Frame widget)
  WaveFamilyLabel  	(Label widget)
  RightFacing_rb	(radio button widget - 1)
  LeftFacing_rb		(radio button widget - 2)
  family			(value of button)
  ---------------------- 
  RangeFrame		(Frame widget)
  RangeLabel		(Label widget)
  variable		(string)
  VariableEntry		(BrowseEntry widget)
  VarMinLabel		(Label widget)
  var_min		(string)
  VarMinEntry		(Entry widget)
  VarMaxLabel		(Labeel widget)
  *var_max		(string)
  VarMaxEntry		(Entry widget)
  ----------------------   
  WaveList_rb		(radio button widget - 1)
  *wave_out			(value of button)
  WavePlotFrame		(Frame widget)
  WavePlot_rb		(radio button widget - 2)
  x_var			(string)
  x_varEntry		(BrowseEntry widget)
  xminLabel		(Label widget)
  x_min			(string)
  x_minEntry		(Entry widget)
  xmaxLabel		(Label widget)
  x_max			(string)
  x_maxEntry		(Entry widget)
  y_var			(string)
  y_varEntry		(BrowseEntry widget)
  yminLabel		(Label widget)
  y_min			(string)
  y_minEntry		(Entry widget)
  ymaxLabel		(Label widget)
  y_max			(string)
  y_maxEntry		(Entry widget)
=cut

#local subroutines
sub MaterialEntryFrame;
sub RefStateFrame;
sub InitStateFrame;
sub LocusFrame;

sub Parameters;
sub MaterialList;

sub DisabledMaterial;
sub NormalMaterial;

sub new
{
	my $mat = $_[1];
	my $frame = $mat->{'frame'};
	bless $mat;
	
	$mat->{EntryFrame} = $frame->Frame()
		->pack( -side=>'top', -anchor=>'w', -expand=>1);
	$mat->MaterialEntryFrame;

	$mat->{RefStateFrame} = $frame->Frame()
		->pack( -side=>'top', -anchor=>'w', -expand=>1);
	$mat->RefStateFrame;
	
	$mat->{InitStateFrame} = $frame->Frame()
		->pack( -side=>'top', -anchor=>'w', -expand=>1);
	$mat->InitStateFrame;
	
	$mat->{RPFrame} = $frame->Frame()
		->pack( -side=>'top', -anchor=>'w', -expand=>1);
	$mat->MaterialRPFrame;
	
	$mat->{LocusFrame} = $frame->Frame()
		->pack( -side=>'top', -anchor=>'w', -expand=>1);
	$mat->LocusFrame;

	$mat->{SetRefState} = undef;
	tie $mat->{SetRefState}, 'RefStateTie', $mat;
	$mat->{SetRefState} = 'disabled';
	
	$mat->{SetInitState} = undef;
	tie $mat->{SetInitState}, 'InitStateTie', $mat;
	$mat->{SetInitState} = 'disabled';

	return $mat;
}

sub DESTROY
{
  my $mat = shift;
# print "Material DESTROY: $mat\n";
  return if scalar %$mat eq '0';

#TODO: unties and destroy states

  my $riemann = $mat->{Riemann};
  return unless defined $riemann;  # upon termination riemann destroyed first
  #$riemann->{RightMaterial} = undef if $riemann->{RightMaterial} == $mat;
  #$riemann->{LeftMaterial}  = undef if $riemann->{LeftMaterial}  == $mat;
  undef %$mat;
}

sub MaterialError
{
	my $mat = shift;
	my $msg = shift;

	::ErrorMessage(@$msg);
	return if not defined $_[0];
	if( $_[0] eq 'RefState' )
	{
	    $mat->{OldMat} = $mat->{Material} = undef;
	    $mat->{SetRefState} = 'active';
	}
	else
	{
	    $mat->{SetInitState} = 'active';
	}
}

1;
