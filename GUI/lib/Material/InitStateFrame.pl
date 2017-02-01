require 'SetInitState.pl';
sub UnsetStateBindings;

sub InitStateFrame
{
	my $mat = shift;
	my $frame = $mat->{InitStateFrame};
	
	$mat->{InitHeaderFrame} = $frame->Frame()
		->pack( -side=>'top', -expand=>1, -anchor=>'w' );
	$mat->InitHeader;

	my $InitVarFrame = $frame->Frame()
		->pack( -side=>'top' );
	$mat->{InitState} = State->new($InitVarFrame,
			qw(V V e e u u P P T T c c u_s u_s) );
	$mat->{InitVarFrame} = $InitVarFrame;
}

sub InitHeader
{
	my $mat = shift;
	my $frame = $mat->{InitHeaderFrame};
	
	$mat->{InitLabel} = $frame->Label( -text => "Initial state:" )
	               ->pack(-side=>'left');
	$mat->{init_state} = "ref_state";
	my $RefState_rb = $mat->{RefState_rb} = $frame->Radiobutton(
		-text => "reference state",
		-variable => \$mat->{init_state},
		-command => [\&InitInitState, $mat],
		-value => "ref_state",
		) ->pack(-side=>'left');
	&::StatusMessage($RefState_rb, "Set Initial state to Reference state");
	my $HugState_rb = $mat->{HugState_rb} = $frame->Radiobutton(
		-text => "Hugoniot",
		-variable => \$mat->{init_state},
		-command => [\&InitInitState, $mat],
		-value => "InitShock",
		) ->pack(-side=>'left');
	&::StatusMessage($HugState_rb, 
	  "Set Initial state to  point on Hugoniot based on Reference state");
	my $IsentropeState_rb = $mat->{IsentropeState_rb} = $frame->Radiobutton(
		-text => "Isentrope   :   ",
		-variable => \$mat->{init_state},
		-command => [\&InitInitState, $mat],
		-value => "InitIsentrope",
		) ->pack(-side=>'left');
	&::StatusMessage($IsentropeState_rb,
	  "Set Initial state to  point on isentrope based on Reference state");

	$mat->{init_state_dir} = undef;
	my $LeftInitState_rb = $mat->{LeftInitState_rb}
		= $frame->Radiobutton(
			-text => "Left",
			-variable => \$mat->{init_state_dir},
			-value => "init_left",
		) ->pack(-side=>'left');
	&::StatusMessage($LeftInitState_rb,
			"Left facing wave based on Initial State");
	my $RightInitState_rb = $mat->{RightInitState_rb}
		= $frame->Radiobutton(
			-text => "Right",
			-variable => \$mat->{init_state_dir},
			-value => "init_right",
		) ->pack(-side=>'left');
	&::StatusMessage($RightInitState_rb,
			"Right facing wave based on Initial State");
}

sub DisabledInitStateFrame
{
	my ($mat) = @_;

	&UnsetStateBindings($mat->{InitState});

	$mat->{InitLabel}->configure(-foreground=>$::disabledforeground);
	my $button;
	foreach $button ( qw/ RefState_rb HugState_rb IsentropeState_rb
	                      LeftInitState_rb RightInitState_rb / )
	{
		$mat->{$button}->configure(
				-state=>'disabled',
				-highlightcolor=>$::highlightbackground,
				-selectcolor=>$::disabledforeground,				
			);		
	}
	$mat->{InitState}->DisabledVariables;
}	

sub NormalInitStateFrame
{
	my ($mat) = @_;
	$mat->{InitStateMaterial} = ${$mat->{Material}};
	$mat->{InitLabel}->configure(-foreground=>$::foreground);
	my $button;
	foreach $button ( qw/ RefState_rb HugState_rb IsentropeState_rb / )
	{
		$mat->{$button}->configure(
				-state=>'normal',
				-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::disabledforeground,				
			);		
	}
	foreach $button ( qw/ LeftInitState_rb RightInitState_rb / )
	{
		$mat->{$button}->configure(
				-state=>'normal',
				-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::indicatorcolor,				
			);		
	}
	$mat->{InitState}->DeselectVariables;
	$mat->{init_state_dir} = "init_right"
		if not defined $mat->{init_state_dir};
}

1;


