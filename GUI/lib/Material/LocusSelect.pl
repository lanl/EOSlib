sub LocusSelect
{
	my $mat = shift;
	my $frame = $mat->{LocusSelectFrame};

	$mat->{LocusLabel} = $frame->Label(
			-text => "Wave locus",
			-width => 15, -height =>2.5,
			-anchor=>'s',
		)->pack( -side=>'left', -anchor=>'s');
		
	$mat->{locus} = 'Shock';	
	$mat->{LocusHugoniot_rb} = $frame->Radiobutton(
		-text => 'Hugoniot',
		-variable => \$mat->{locus},
		-value => 'Shock',
		-width => 15, -justify=>'left', -anchor=>'w',
		) ->pack( -side=>'left', -anchor=>'s' );
	$mat->{LocusIsentrope_rb} = $frame->Radiobutton(
		-text => 'Isentrope',
		-variable => \$mat->{locus},
		-value => 'Isentrope',
		-width => 15, -justify=>'left', -anchor=>'w',
		) ->pack( -side=>'left', -anchor=>'s' );
}

sub WaveFamily
{
	my $mat = shift;
	my $frame = $mat->{WaveFamilyFrame};

	$mat->{WaveFamilyLabel} = $frame->Label(
			-text => "Wave family",
			-width => 15,
		)->pack( -side=>'left');
		
	$mat->{family} = 'right';	
	$mat->{RightFacing_rb} = $frame->Radiobutton(
		-text => "right facing",
		-variable => \$mat->{family},
		-value => 'right',
		-width => 15, -justify=>'left', -anchor=>'w',
		) ->pack( -side=>'left' );
	$mat->{LeftFacing_rb} = $frame->Radiobutton(
		-text => "left facing",
		-variable => \$mat->{family},
		-value => 'left',
		-width => 15, -justify=>'left', -anchor=>'w',
		) ->pack( -side=>'left' );
}


sub Range
{
	my $mat = shift;
	my $frame = $mat->{RangeFrame};

	$mat->{RangeLabel} = $frame->Label(
			-text => "Range",
			-width => 8,
		)->pack( -side=>'left', -expand=>1 );
	
	$mat->{variable} = 'P';
	my $VariableEntry = $mat->{VariableEntry}
	   = $frame->BrowseEntry(
		-label => "Variable", -labelWidth => 10,
		-width => 5, -listwidth=>20,
	  	-textvariable => \$mat->{variable},
		-state => 'readonly',
		#-browsecmd => sub{ $data->UnitsReset;},
		) ->pack(-side=>'left');
	$VariableEntry->insert('end','P');
	$VariableEntry->insert('end','u_p');
	$VariableEntry->insert('end','u_s');
	$VariableEntry->insert('end','V');
	

	$mat->{VarMinLabel} = $frame->Label(
			-text => "minimum",
			-width => 12,
		)->pack( -side=>'left', -expand=>1 );
		
	$mat->{var_min} = undef;
	my $VarMinEntry = $mat->{VarMinEntry}
	   = $frame->Entry(
		-width => 10,
	  	-textvariable => \$mat->{var_min},
		) ->pack(-side=>'left');
	$VarMinEntry->bind('<Leave>',
	   		sub{ return if( (! defined $mat->{var_min}) or
					&::IsNum($mat->{var_min}) );
	   			 $::mw->bell; $mat->{var_min}=undef; }
		);
		
	$mat->{VarMaxLabel} = $frame->Label(
			-text => "maximum",
			-width => 12,
		)->pack( -side=>'left', -expand=>1 );
	$mat->{var_max} = undef;
	my $VarMaxEntry = $mat->{VarMaxEntry}
	   = $frame->Entry(
		-width => 10,
	  	-textvariable => \$mat->{var_max},
		) ->pack(-side=>'left');
	$VarMaxEntry->bind('<Leave>',
	   		sub{ return if( (! defined $mat->{var_max}) or
					&::IsNum($mat->{var_max}) );
	   			 $::mw->bell; $mat->{var_max}=undef; }
		);

	$VariableEntry->configure(
		-browsecmd=>sub{$mat->{var_min}=$mat->{var_max}=undef;} );
}


1;
