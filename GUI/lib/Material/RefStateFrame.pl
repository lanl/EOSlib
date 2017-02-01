#require 'LocusSelect.pl';

sub RefStateFrame
{
	my $mat = shift;
	my $frame = $mat->{RefStateFrame};
	
	$mat->{RefHeaderFrame} = $frame->Frame()
		->pack( -side=>'top', -expand=>1, -anchor=>'w' );
	$mat->RefHeader;
	
	my $RefVarFrame = $mat->{RefVarFrame} = $frame->Frame()
		->pack( -side => 'top', -expand=>1, -anchor=>'w' );
	$mat->{RefState} = State->new($RefVarFrame,
					qw( V V e e u u P P T T c c ) );	
	$mat->{Veu_rb}->configure( -command => sub{
		$mat->{SetRefState}='active';
		my $state = $mat->{RefState};
		$state->NormalVariables( qw(V e u) );
		$state->SetValues(undef, qw(V e u) );
		$state->DisabledVariables(   qw(P T c) );
		$state->SetValues('----------', qw(P T c) );
		$mat->{Veu_rb}->configure(-selectcolor=>$::indicatorcolor);
		$::OldStatus = $::status_message = 
			"Enter {V,e,u}, then press `Update State' button";}
		);
	$mat->{PTu_rb}->configure( -command => sub{
		my $state = $mat->{RefState};
		$state->NormalVariables( qw(P T u) );
		$state->SetValues(undef, qw(P T u) );
		$state->DisabledVariables(   qw(V e c) );
		$state->SetValues('----------', qw(V e c) );
		$mat->{SetRefState} = 'active';
		$mat->{PTu_rb}->configure(-selectcolor=>$::indicatorcolor);
		$::OldStatus = $::status_message =
		"Enter {P,T,u}, then press `Update State' button";}
		);
}


sub RefHeader
{
	my $mat = shift;
	my $frame = $mat->{RefHeaderFrame};
	
	$mat->{RefLabel} = $frame->Label( -text => "Reference state:" )
	               ->pack( -side=>'left' );
	$mat->{ref_state} = 'default';
	my $default_rb = $mat->{default_rb} = $frame->Radiobutton(
		-text => "default",
		-variable => \$mat->{ref_state},
		-value => 'default',
		-command => sub{$mat->MaterialSelected},
		) ->pack( -side=>'left' );
	&::StatusMessage($default_rb,
			"Set reference state to default from data file");
	my $Veu_rb = $mat->{Veu_rb} = $frame->Radiobutton(
		-text => "(V,e,u)",
		-variable => \$mat->{ref_state},
		-value => 'Veu',
		) ->pack( -side=>'left' );
	&::StatusMessage($Veu_rb, "Modify reference state");
	my $PTu_rb = $mat->{PTu_rb} = $frame->Radiobutton(
		-text => "(P,T,u)",
		-variable => \$mat->{ref_state},
		-value => 'PTu',
		) ->pack( -side=>'left' );
	&::StatusMessage($PTu_rb, "Modify reference state");
	my $InitState_rb = $mat->{InitState_rb} = $frame->Radiobutton(
		-text => "InitState",
		-variable => \$mat->{ref_state},
		-value => 'InitState',
		-command=>sub{
			$mat->{RefState}->CopyState($mat->{InitState});
			$mat->{init_state} = 'ref_state';
			$mat->{init_state_dir} = undef;
			},
		) ->pack( -side=>'left' );
	&::StatusMessage($InitState_rb, "Set reference state to Initial state");
	my $RiemannState_rb = $mat->{RiemannState_rb} = $frame->Radiobutton(
		-text => "RiemannState",
		-variable => \$mat->{ref_state},
		-value => 'RiemannState',
		-command=>sub{$mat->SetToRiemannState;},
		) ->pack( -side=>'left' );
	&::StatusMessage($RiemannState_rb,
		"Set reference state to solution of Riemann Problem");
	
	my $UpdateButton = $mat->{UpdateButton} = $frame->Button(
		-text => "Update State",
		-command => sub{$mat->MaterialSelected},
		) ->pack( -side=>'left', -padx=>0 );	
	&::StatusMessage($UpdateButton, "Update reference state");
}

sub DisabledRefStateFrame
{
	my ($mat) = @_;
	
	$mat->{RefLabel}->configure(-foreground=>$::disabledforeground);	
	my $button;
	foreach $button ( qw/ default_rb   Veu_rb          PTu_rb
	                      InitState_rb RiemannState_rb / )
	{
		$mat->{$button}->configure(
				-state=>'disabled',
				-highlightcolor=>$::highlightbackground,
				-selectcolor=>$::disabledforeground,				
			);		
	}
	$mat->{UpdateButton}->configure(
				-state=>'disabled',
				-highlightcolor=>$::highlightbackground,
			);
	$mat->{ref_state} = 'default';
	$mat->{RefState}->DisabledVariables;
}

sub NormalRefStateFrame
{
	my ($mat) = @_;

	$mat->{RefLabel}->configure(-foreground=>$::foreground);	
	my $button;
	foreach $button ( qw/ default_rb Veu_rb PTu_rb / )
	{
		$mat->{$button}->configure(
				-state=>'normal',
				-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::disabledforeground,
			);
	}
	if( $mat->{SetRefState} eq 'active' )
	{
	    $mat->{UpdateButton}->configure(
				-state=>'normal',
				-highlightcolor=>$::highlightcolor,
			);
	}
	else
	{
	    $mat->{UpdateButton}->configure(
				-state=>'disabled',
				-highlightcolor=>$::highlightbackground,
			);
	}
	$mat->{RefState}->DeselectVariables
			if $mat->{SetRefState} eq 'set';
}

sub SetInitState_rb
{
	my $mat = shift;
	if ( $mat->{SetInitState} eq 'set' )
	{
		$mat->{InitState_rb}->configure(
				-state=>'normal',
				-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::disabledforeground,
			);
	}
	else
	{
		$mat->{InitState_rb}->configure(
				-state=>'disabled',
				-highlightcolor=>$::highlightbackground,
				-selectcolor=>$::disabledforeground,				
			);		
	}
}

sub SetRiemannState_rb
{
	my ($mat, $flag) = @_;
	if ( $flag eq 'set' )
	{
		$mat->{RiemannState_rb}->configure(
				-state=>'normal',
				-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::disabledforeground,
			);
	}
	else
	{
		$mat->{RiemannState_rb}->configure(
				-state=>'disabled',
				-highlightcolor=>$::highlightbackground,
				-selectcolor=>$::disabledforeground,				
			);		
	}
}

sub SetToRiemannState
{
	my $mat = shift;
	my $riemann = $mat->{Riemann};
	my $RiemannState;
	if( $mat == $riemann->{LeftMaterial} )
	{
		$RiemannState = $riemann->{LeftState};
	}
	elsif( $mat == $riemann->{RightMaterial} )
	{
		$RiemannState = $riemann->{RightState};
	}
	else
	{
		return;
	}
	$mat->{RefState}->CopyState($RiemannState);
	$mat->{InitState}->CopyState($RiemannState);
	$mat->{init_state} = 'ref_state';
	$mat->{init_state_dir} = undef;
	$riemann->StateChanged(1);
	$::OldStatus = $::status_message = "State set";	
}

1;
