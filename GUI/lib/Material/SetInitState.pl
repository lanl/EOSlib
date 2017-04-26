sub SetInitState
{
	my ($mat, $InitState, $RefState, $var) = @_;

	my $DataFile = ${$mat->{DataFile}};
	my $units    = ${$mat->{units}};
	my $Material = $mat->{Material};	
	
	my $command = "$::Run Wave file $DataFile material $Material";
	$command .= " units hydro::$units" if $units;
	$command .= " V_form 17 11 e_form 17 11 u_form 17 11";			
	$command .= ' ' . $RefState->EvaluateState( '0', qw(V e u) );
	$command .= " $mat->{init_state}";
	$command .= " $mat->{init_state_dir}";
	
	my $value = $InitState->{$var}{fvalue};
	if( (not defined $value) or (not &::IsNum($value)) )
	{
		$::mw->bell;
		$InitState->{$var}{fvalue} = undef;
		return;
	}
	
 point: {
 		$command .= ' u_p ' . $value, last point if $var eq 'u';
 		$command .= ' u_s ' . $value, last point if $var eq 'u_s';
 		$command .= ' Ps '  . $value, last point if $var eq 'P';
 		$command .= ' Vs '  . $value, last point if $var eq 'V';
 	}

	goto error unless open(WAVE, "$command|");

	<WAVE>;	<WAVE>;
	my $line = <WAVE>;
	if( $line =~ /^---/ )
	{
	    <WAVE>;
	    $line = <WAVE>;
	}
	goto error unless defined $line;
	$line =~ s/^\s+//;
	my ($V_u,$e_u,$u_u,$P_u,$us_u,$c_u,$T_u) = split(/\s+/,$line);
	<WAVE>;
	$line = <WAVE>;
	goto error unless defined $line;
	$line =~ s/^\s+//;
	my ($V,$e,$u,$P,$us,$c,$FD,$T) = split(/\s+/,$line);
	close(WAVE);
	goto error if $?;
	
	$InitState->SetStateUnits(  V=>$V_u, e=>$e_u, u=>$u_u,
	                            P=>$P_u, T=>$T_u, c=>$c_u, u_s=>$us_u );	
	$InitState->SetStateValues( V=>$V,   e=>$e,   u=>$u,
	                            P=>$P,   T=>$T,   c=>$c,   u_s=>$us   );
	&UnsetStateBindings($InitState);
	$::OldStatus = $::status_message = "Initial state is set";
	$mat->{SetInitState} = 'set';
	return;
error :
	print "\nERROR with command\n$command\n";
	$mat->MaterialError(
		["Failed computing state for Material `$Material'",
		"Error executing: $command",
		"Initial state not set"],
		'InitState');
	$mat->{init_state} = 0;
	$InitState->SetStateValues;
	&UnsetStateBindings($InitState);
	$mat->{SetInitState} = 'active';
	$mat->{InitStateMaterial} = undef;
}

sub InitInitState
{
	my $mat = shift;
	
	my $InitState = $mat->{InitState};
	my $RefState  = $mat->{RefState};

	my $init_state = $mat->{init_state};
	if( $init_state eq 'ref_state' )
	{
		&UnsetStateBindings($InitState);	
		$::OldStatus = $::status_message = "Initial state is set";
		$InitState->SetStateNull;
		$InitState->CopyState($RefState);
		
		$::OldStatus = $::status_message = "Initial state is set";
		$mat->{SetInitState} = 'set';
		$mat->{init_state_dir} = undef;
		return;
	}

	$::OldStatus = $::status_message =
		"Enter either V, u, P or u_s then `return'";
	
	$mat->{SetInitState} = 'active';
	$mat->{init_state_dir} = 'init_right'
		unless defined $mat->{init_state_dir};
	$InitState->NormalVariables( qw(V u P u_s) );
	$InitState->SetValues(undef, qw(V u P u_s) );
	$InitState->DisabledVariables( qw(e T c) );
	$InitState->SetValues('----------', qw(e T c) );
	# SetBindings
	my $var;
	for $var ( qw( V u P u_s ) )
	{
		my $widget = $InitState->{$var}->{Entry};
		$widget->bind('<Return>',
			sub{$mat->SetInitState($InitState, $RefState, $var);} );
		$widget->bind('<Leave>',
			sub{ $InitState->{$var}{fvalue} = undef;} );
	}
	my $button;
	foreach $button ( qw/ RefState_rb HugState_rb IsentropeState_rb
	                      LeftInitState_rb RightInitState_rb / )
	{
		$mat->{$button}->configure(-selectcolor=>$::indicatorcolor);
	}
}

sub UnsetStateBindings
{
	my $state = shift;
	my $var;
	for $var ( keys %$state )
	{
		next if $var eq 'frame';
		my $widget = $state->{$var}->{Entry};
		my $fvalue = $state->{$var}->{fvalue};
		$widget->bind('<Return>', "" );
		$widget->bind('<Leave>', [\&State::LeaveState, \$fvalue] );
	}
	$state->DeselectVariables;
}


1;
