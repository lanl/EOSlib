require 'RPplot.pl';

sub SolveRP
{
	my $riemann = shift;
	my $LeftMaterial = $riemann->{LeftMaterial};
	my $RightMaterial = $riemann->{RightMaterial};

	my $command = "$::Run ImpedanceMatch file " . 
				${$RightMaterial->{DataFile}};
	my $mat;
	my $n = 1;
	for $mat ( $LeftMaterial, $RightMaterial )
	{
	    $command .= " mat$n " . $mat->{Material};
	    my $units = ${$mat->{units}};
	    $command .= " units hydro::$units" if $units;
	    my $state = $mat->{InitState}->EvaluateState( $n, qw(V e u) );
	    $command .= ' ' . $state;
	    $n++;
	}
	goto error unless open(WAVE, "$command|");

	&skip_lines(6);	# skip Header
	my $line = <WAVE>;		# units
	goto error unless defined $line;
	$line =~ s/^\s+//;
	my ($V_u,$e_u,$u_u,$P_u,$us_u,$c_u,$T_u) = split(/\s+/,$line);
	&skip_lines(2);			# skip incident state
	$line = <WAVE>;			# left wave
	goto error unless defined $line;
	my ($label1, $V1,$e1,$u1,$P1,$us1,$c1,$T1) = split(/\s+/,$line);
	$line = <WAVE>;			# right wave
	goto error unless defined $line;
	my ($label2, $V2,$e2,$u2,$P2,$us2,$c2,$T2) = split(/\s+/,$line);
	close(WAVE);
	goto error if $?;
	
	my $LeftState = $riemann->{LeftState};
	$LeftState->SetStateUnits(  V=>$V_u, e=>$e_u, u=>$u_u,
	                            P=>$P_u, T=>$T_u, c=>$c_u, u_s=>$us_u );
	$LeftState->SetStateValues( V=>$V1,  e=>$e1,  u=>$u1,
	                            P=>$P1,  T=>$T1,  c=>$c1,  u_s=>$us1  );
	$LeftState->DeselectVariables;
	
	my $RightState = $riemann->{RightState};
	$RightState->SetStateUnits(  V=>$V_u, e=>$e_u, u=>$u_u,
	                             P=>$P_u, T=>$T_u, c=>$c_u, u_s=>$us_u );
	$RightState->SetStateValues( V=>$V2,  e=>$e2,  u=>$u2,
	                             P=>$P2,  T=>$T2,  c=>$c2,  u_s=>$us2  );
	$RightState->DeselectVariables;
	$riemann->{lockRP_cb}->configure(
		    		-takefocus=>1, -state=>'normal',
				-foreground => $::foreground,
				-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::indicatorcolor,
				);
	
	$::OldStatus = $::status_message = "Solved Riemann problem";

	$riemann->RPplot if $riemann->{plot} eq 'on';
	$LeftMaterial->SetRiemannState_rb('set');
	$RightMaterial->SetRiemannState_rb('set');
	return;
error :
	::ErrorMessage(
		"Failed solving Riemann Problem",
		"Error executing: $command",
		"Reset states for Riemann Problem"
		);
}

sub skip_lines
{
	my $i;
	for( $i=$_[0]; $i > 0 ; $i--)
	{
		$line = <WAVE>;
	}
}


1;
