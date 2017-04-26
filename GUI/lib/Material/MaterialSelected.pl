sub MaterialSelected
{
	my $mat = shift;
	$mat->{Material} = $_[0] if defined $_[0];
	my $Material = $mat->{Material};
	if( defined $Material and not $Material )
	{
		$mat->{Material} = $mat->{OldMat};
		return;
	}
	$mat->{OldMat} = $mat->{Material};
	$mat->{Riemann}->CheckRPlock($mat);

	my $DataFile = ${$mat->{DataFile}};
	my $DataBase = $mat->{DataBase};

	if( not $DataBase->{$Material} )
	{
		$mat->{SetInitState} = 'disabled';
		$mat->MaterialError(
		   ["Material `$Material' not in selected Data Base",
		   "Error selecting Material",
		   "Enter new Material"],
		   'RefState');
		return;
	}
	my $ref_state = $mat->{ref_state};
	my $RefState = $mat->{RefState};
	my $units = ${$mat->{units}};
	
	if( !$units )
	{
		my $mat_units =$mat->{DataBase}->{$Material};
		if( $mat_units )
		{
			${$mat->{units}} = $mat_units;
			$mat->{Parent}->UnitsReset($units);
			return;
		} 
	}
	
	my $command = "$::Run Wave file $DataFile material $Material";  
	$command .= " units hydro::$units" if $units;
	$command .= " V_form 17 11 e_form 17 11 u_form 17 11";

	my $estate;
	if( $ref_state eq 'Veu' )
	{
		$estate = $RefState->EvaluateFstate( '0', qw(V e u) );
	}
	elsif( $ref_state eq 'PTu' )
	{
		$estate = $RefState->EvaluateFstate( '0', qw(P T u) );	
	}
	if( $estate )
	{
		$command .= ' ' . $estate;
	}
	else
	{
		$mat->{ref_state} = 'default';
	}
	
	goto error unless open(WAVE, "$command|");
	<WAVE>;<WAVE>;	# skip Header
	my $line = <WAVE>;
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
	
	$RefState->SetStateUnits(  V=>$V_u, e=>$e_u, u=>$u_u,
	                           P=>$P_u, T=>$T_u, c=>$c_u );
	$RefState->SetStateValues( V=>$V,   e=>$e,   u=>$u,
	                           P=>$P,   T=>$T,   c=>$c   );
	$mat->{SetRefState} = 'set';
	$mat->{init_state} = 'ref_state';
	$mat->InitInitState;	
	return;	
error :
	$mat->{ref_state} = 0;	
	print "\nERROR with command\n$command\n";
	$mat->MaterialError(
		   ["Failed computing reference state for Material `$Material'",
		   "Error executing: $command",
		   "Reference State NOT set, Enter new Material"],
		   'RefState');
	$RefState->SetValues(undef);
}


1;
