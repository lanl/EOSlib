require 'LocusSelect.pl';
require 'WavePlot.pl';

sub LocusSelect;

sub LocusFrame
{
	my $mat = shift;
	my $frame = $mat->{LocusFrame};

	my $LocusSelectFrame = $mat->{LocusSelectFrame}
	                = $frame->Frame
		->pack( -side=>'top', -expand=>1, -anchor=>'sw' );
	$mat->LocusSelect;

	my $WaveFamilyFrame = $mat->{WaveFamilyFrame}
			= $frame->Frame
		->pack( -side=>'top', -expand=>1, -anchor=> 'w' );
	$mat->WaveFamily;			

	my $RangeFrame = $mat->{RangeFrame}
			= $frame->Frame
		->pack( -side=>'top', -expand=>1, -anchor=>'w' );
	$mat->Range;

	$mat->{wave_out} = 0;
	my $WaveList_rb = $mat->{WaveList_rb}
		= $frame->Radiobutton(
		-text => "List",
		-variable => \$mat->{wave_out},
		-value => 'list',
		-command => sub{$mat->ComputeLocus;},
		-width => 8, -justify=>'left', -anchor=>'w',
		) ->pack( -side=>'top', -expand=>1, -anchor=>'w' );
	&::StatusMessage($WaveList_rb, "List wave locus");
	
	my $WavePlotFrame = $mat->{WavePlotFrame}
		= $frame->Frame
		->pack( -side=>'top', -expand=>1, -anchor=>'w' );
	$mat->WavePlot;
}


sub DisabledLocusFrame
{
	my ($mat) = @_;
	
	my $widget;
	for $widget ( qw( LocusHugoniot_rb LocusIsentrope_rb
	                  RightFacing_rb   LeftFacing_rb
			  WaveList_rb      WavePlot_rb
		    )   )
	{
		$mat->{$widget}->configure(
				-state => 'disabled',
				-highlightcolor=>$::highlightbackground,
				-selectcolor => $::disabledforeground,
			);
	}
	for $widget ( qw( VariableEntry    VarMinEntry       VarMaxEntry
			  x_varEntry	   x_minEntry        x_maxEntry
			  y_varEntry	   y_minEntry        y_maxEntry
		    )   )
	{
		$mat->{$widget}->configure(
		        	-takefocus=>0, -state=>'disabled',
				-highlightcolor => $::highlightbackground,
				-foreground => $::disabledforeground,
			);
	}
	for $widget ( qw( LocusLabel 	WaveFamilyLabel	RangeLabel
			  VarMinLabel 	VarMaxLabel
			  xminLabel     xmaxLabel
			  yminLabel     ymaxLabel
		    )   )
	{
		$mat->{$widget}->configure(
				-foreground=>$::disabledforeground);
	}
			
}

sub NormalLocusFrame
{
	my ($mat) = @_;

	my $widget;
	for $widget ( qw( LocusHugoniot_rb LocusIsentrope_rb
	                  RightFacing_rb   LeftFacing_rb
			  WaveList_rb      WavePlot_rb
		    )   )
	{
		$mat->{$widget}->configure(
				-state => 'normal',
		                -highlightcolor=>$::highlightcolor,
				-selectcolor => $::indicatorcolor,
			);
	}
	for $widget ( qw( VariableEntry    VarMinEntry       VarMaxEntry
			  x_varEntry	   x_minEntry        x_maxEntry
			  y_varEntry	   y_minEntry        y_maxEntry
		    )   )
	{
		$mat->{$widget}->configure(
		        	-takefocus=>1, -state=>'normal',
				-highlightcolor => $::highlightcolor,
				-foreground => $::foreground,
			);
	}
	for $widget ( qw( LocusLabel 	WaveFamilyLabel	RangeLabel
			  VarMinLabel 	VarMaxLabel
			  xminLabel     xmaxLabel
			  yminLabel     ymaxLabel
		    )   )
	{
		$mat->{$widget}->configure(-foreground=>$::foreground);
	}	
}

1;
