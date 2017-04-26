package Riemann;

require Exporter;
@ISA = qw(Exporter);
# @EXPORT = qw( InsertFiles FileSelected Reset );


#use lib 'lib/Riemann';
require 'StateReadyTie.pl';
require 'RiemannStateTie.pl';
require 'RiemannFrame.pl';

=class Riemann
LeftMaterial
LeftState

RightMaterial
RightState

RiemannLabel
SolveButton
PlotRP_cb
plot
lockRP_cb
lock
FileLabel
PlotFile
SaveFile
FileEntry


LeftLabel
LeftMaterialLabel

RightLabel
RightMaterialLabel
=cut

sub new
{
	my $riemann = {};
	my $frame = $riemann->{frame} = $_[1];
	bless $riemann;

	$riemann->{HeaderFrame} = $frame->Frame
		->pack( -side=>'top', -expand=>1, -anchor=>'w', -pady=>10 );
	$riemann->HeaderFrame;

	$riemann->{LeftMaterialFrame} = $frame->Frame()
		->pack( -side => 'top', -expand=>1, -anchor=>'w', -pady=>5 );
	$riemann->LeftMaterialFrame;

	$riemann->{RightMaterialFrame} = $frame->Frame()
		->pack( -side => 'top', -expand=>1, -anchor=>'w', -pady=>5 );
	$riemann->RightMaterialFrame;

	return $riemann;
}

sub StateChanged
{
	my ($riemann, $flag) = @_;
	return if $riemann->{lock} eq 'on';
	
	my $StateSet = 0;
	my $side;
	foreach $side ( qw(Left Right) )
	{
	    my $mat = $riemann->{$side . 'Material'};
	    next unless $mat;
	    my $Label = $riemann->{$side . 'MaterialLabel'};
	    if( $mat->{SetInitState} eq 'set' )
	    {
	    	$Label->configure(-foreground=>$::foreground);
	    	$StateSet++;
	    }
	    else
	    {
		    $Label->configure(-foreground=>$::disabledforeground);
	    }
	}

	$riemann->{lockRP_cb}->configure(
		        	-takefocus=>0, -state=>'disabled',
				-foreground => $::disabledforeground,
				-highlightcolor=>$::highlightbackground,
				-selectcolor=>$::disabledforeground,
			);	
	$riemann->{FileLabel}->configure(-foreground=>$::disabledforeground);
	$riemann->{FileEntry}->configure(
		        	-takefocus=>0, -state=>'disabled',
				-highlightcolor => $::highlightbackground,
				-foreground => $::disabledforeground,
		);	

	if( $StateSet == 2 and not defined $flag  )
	{
		$riemann->{SolveButton}->configure(
		        	-takefocus=>1, -state=>'normal',
				-foreground => $::foreground,
				-highlightcolor=>$::highlightcolor,
			);			
		$riemann->{PlotRP_cb}->configure(
		        	-takefocus=>1, -state=>'normal',
				-foreground => $::foreground,
				-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::indicatorcolor,
			);
	}
	else
	{
		$riemann->{SolveButton}->configure(
		        	-takefocus=>0, -state=>'disabled',
				-foreground => $::disabledforeground,
				-highlightcolor=>$::highlightbackground,
			);				
		$riemann->{PlotRP_cb}->configure(
		        	-takefocus=>0, -state=>'disabled',
				-foreground => $::disabledforeground,
				-highlightcolor=>$::highlightbackground,
				-selectcolor=>$::disabledforeground,
			);
	    foreach $side ( qw(Left Right) )
		{
		    my $state = $riemann->{$side . 'State'};
	            $state->SetStateNull;
	            $state->DisabledVariables;
		    my $mat = $riemann->{$side . 'Material'};
		    next unless $mat;
		    $mat->SetRiemannState_rb('disabled');
		}
	}
}

1;
