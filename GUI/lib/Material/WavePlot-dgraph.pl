sub WavePlot
{
	my $mat = shift;
	my $frame = $mat->{WavePlotFrame};

	my $line1 = $frame->Frame()->pack( -side=>'top', -anchor=>'w' );
	my $WavePlot_rb = $mat->{WavePlot_rb}
		= $line1->Radiobutton(
		-text => "Plot",
		-variable => \$mat->{wave_out},
		-command => sub{$mat->ComputeLocus;},
		-value => 'plot',
		-justify=>'left', -anchor=>'w',
		) ->pack(-side=>'left');
	&::StatusMessage($WavePlot_rb, "Plot wave locus");

	$mat->{x_var} = 'V';
	my $x_varEntry = $mat->{x_varEntry}
	   = $line1->BrowseEntry(
		-label => "x-variable", -labelWidth => 10,
		-width => 5, -listwidth=>20,
	  	-textvariable => \$mat->{x_var},
		-state => 'readonly',
		) ->pack(-side=>'left');
	$mat->{xminLabel} = $line1->Label( -text=>"minimum", -width=>10)
		->pack(-side=>'left');
	$mat->{x_min} = undef;
	my $x_minEntry = $mat->{x_minEntry} = $line1->Entry(
		-width => 10,
	  	-textvariable => \$mat->{x_min},
		) ->pack(-side=>'left');
	$x_minEntry->bind('<Leave>',
	   		sub{ return if( (! defined $mat->{x_min}) or
					&::IsNum($mat->{x_min}) );
	   			 $::mw->bell; $mat->{x_min}=undef; }
		);
	$mat->{xmaxLabel} = $line1->Label( -text=>"maximum", -width=>10)
		->pack(-side=>'left');
	$mat->{x_max} = undef;
	my $x_maxEntry = $mat->{x_maxEntry} = $line1->Entry(
		-width => 10,
	  	-textvariable => \$mat->{x_max},
		) ->pack(-side=>'left');
	$x_maxEntry->bind('<Leave>',
	   		sub{ return if( (! defined $mat->{x_max}) or
					&::IsNum($mat->{x_max}) );
	   			 $::mw->bell; $mat->{x_max}=undef; }
		);
	$x_varEntry->configure(
		-browsecmd => sub{$mat->{x_min} = $mat->{x_max} = undef;} );
			
	my $line2 = $frame->Frame()->pack( -side=>'top', -anchor=>'w');
	$line2->Frame( )->pack(-side=>'left', -padx=>38);
	$mat->{y_var} = 'P';
	my $y_varEntry = $mat->{y_varEntry}
	   = $line2->BrowseEntry(
		-label => "y-variable", -labelWidth => 10,
		-width => 5, -listwidth=>20,
	  	-textvariable => \$mat->{y_var},
		-state => 'readonly',
		#-browsecmd => sub{ $data->UnitsReset;},
		) ->pack(-side=>'left');
	$mat->{yminLabel} = $line2->Label( -text=>"minimum", -width=>10)
		->pack(-side=>'left');
	$mat->{y_min} = undef;
	my $y_minEntry = $mat->{y_minEntry} = $line2->Entry(
		-width => 10,
	  	-textvariable => \$mat->{y_min},
		) ->pack(-side=>'left');
	$y_minEntry->bind('<Leave>',
	   		sub{ return if( (! defined $mat->{y_min}) or
					&::IsNum($mat->{y_min}) );
	   			 $::mw->bell; $mat->{y_min}=undef; }
		);
	$mat->{ymaxLabel} = $line2->Label( -text=>"maximum", -width=>10)
		->pack(-side=>'left');
	$mat->{y_max} = undef;
	my $y_maxEntry = $mat->{y_maxEntry} = $line2->Entry(
		-width => 10,
	  	-textvariable => \$mat->{y_max},
		) ->pack(-side=>'left');
	$y_maxEntry->bind('<Leave>',
	   		sub{ return if( (! defined $mat->{y_max}) or
					&::IsNum($mat->{y_max}) );
	   			 $::mw->bell; $mat->{y_max}=undef; }
		);
	$y_varEntry->configure(
		-browsecmd => sub{ $mat->{y_min} = $mat->{y_max} = undef; } );
	
	my $var;
	for $var ( qw( V e u_p P u_s c T ) )
	{
		$x_varEntry->insert('end',$var);
		$y_varEntry->insert('end',$var);
	}
}

sub ComputeLocus
{
	my $mat = shift;
	my $wave_out = $mat->{wave_out}; # 1-List, 2-Plot
	$mat->{wave_out} = 0;
	my $variable = $mat->{variable};
	my $var_min  = $mat->{var_min};
	my $var_max  = $mat->{var_max};
	my $locus    = $mat->{locus};  # Shock or Isentrope
	my $family   = $mat->{family}; # right or left

	my $DataFile = ${$mat->{DataFile}};
	my $units    = ${$mat->{units}};
	my $Material = $mat->{Material};

	my $InitState = $mat->{InitState};
	my %State0 = split(/ /,$InitState->EvaluateState( qw(V e P u c) ));

	if( not defined $var_min )
	{
		if( $variable eq 'P' )
		{
			$var_min = $State0{P0};
		}
		elsif( $variable eq 'u_p' )
		{
			$var_min = $State0{u0};
		}
		elsif( $variable eq 'u_s' )
		{
			$var_min = $State0{c0};
		}
		else
		{
			$var_min = $State0{V0};
		}
	}

# TODO add error checking on variable range
	if( not defined $var_max )
	{
		$::OldStatus = $::status_message = "Error: var_max not defined";
		$::mw->bell;
		return;
	}

	my $command = "$::Run Wave file $DataFile material $Material";
	$command .= " units hydro::$units" if defined $units and $units;
	$command .= ' ' . $InitState->EvaluateState( '0', qw(V e u) );
	if( $variable eq 'u_p' )
	{
		$command .= ' u';
	}
	elsif( $variable eq 'u_s' )
	{
		$command .= ' us';
	}
	else
	{
		$command .= " $variable";
	}
	$command .= " range $var_min $var_max";
	$command .= " $locus $family";
	$command .= " nsteps $::nsteps";
	
	if( not open(WAVE, "$command|") )
	{
	   print "\nERROR with command\n$command\n";
	   $mat->MaterialError(
		["Failed computing wave locus",
		"Error with command, $command",
		"Reset locus parameters"],
		);
	}

	if( $wave_out eq 'list' )
	{
		$mat->ListLocus($command);
	}
	else
	{
		$mat->PlotLocus($command);
	}
}

sub ListLocus
{
	my ($mat,$command) = @_;
	my $DataFile = ${$mat->{DataFile}};
	my $Material = $mat->{Material};

	my $top = $::mw->Toplevel();
	$top->withdraw;
	$top->title("Locus for $Material in DataBase file `$DataFile'");
	my $frame =$top->Frame()
		->pack( -side=>'top', -anchor=>'w');
	$frame->Frame(-width=>19)->pack(-side=>'left');				
	my $i=3;
	my $Header = $frame->Text( -width=>95, -height=>$i, -wrap=>'none' )
		->pack( -side=>'top');
	my $Locus = $top->Scrolled("Text", -scrollbars=>'ow')
		->pack( -side=>'top', -fill=>'y',-expand=>1, -anchor=>'w');
	$Locus->configure( -width=> 95, -wrap=>'none' );
	my $sf=$top->Frame()
		->pack(-side=>'top', -anchor=>'w', -fill=>'x');
	$sf->Frame(-width=>19)->pack(-side=>'left');
	my $Scrollbar = $sf->Scrollbar(-orient=>'horizontal')
		->pack( -side=>'left', -fill=>'x', -expand=>1);
	
	$Header->configure( -xscrollcommand => ['set'=>$Scrollbar] );
	$Locus->configure(  -xscrollcommand => ['set'=>$Scrollbar] );
	$Scrollbar->configure(
		-command=>sub{$Header->xview(@_);$Locus->xview(@_);}
	);
	
	while( $i-- )
	{
		$Header->insert('end', scalar <WAVE>);
	}
	<WAVE>;
	while( <WAVE> )
	{
		$Locus->insert('end', $_);
	}
	close(WAVE);
	if( $? != 0 )
	{
		$top->destroy() if Exists $top;
	   	print "\nERROR with command\n$command\n";
	   	$mat->MaterialError(
			["Failed computing wave locus",
			"Error with command: $command",
			"Reset locus parameters"],
		);
		return;
	}
	
	$Locus->configure( -state => 'disabled' );
	$top->geometry("+50+200");
	$top->raise;
	$top->deiconify;
	# use to destroy window $top
}

sub PlotLocus
{
	my ($mat,$command) = @_;

	my $DataFile = ${$mat->{DataFile}};
	my $Material = $mat->{Material};
	my $locus    = $mat->{locus};
			
	my $x_var    = $mat->{x_var};
	my $y_var    = $mat->{y_var};

	&skip_lines(2); # skip header
	my $line = <WAVE>;
	goto error if not defined $line;
	$line =~ s/^\s*//;
	my %s;
	($s{'V'},$s{'e'},$s{'u_p'}, $s{'P'},$s{'u_s'},$s{'c'},$s{'T'})
		= split(/\s+/,$line);
	<WAVE>;

	my $x_label = &label( "${x_var}($s{$x_var})" );
	my $y_label = &label( "${y_var}($s{$y_var})" );

	my $xlimits = '';
	my $xmin = $mat->{x_min};
	my $xmax = $mat->{x_max};
	my $Xrange = &::IsNum($xmin) and &::IsNum($xmax);
	$xlimits = ".limits x [ $xmin, $xmax ]" if $Xrange;

	my $ylimits = '';
	my $ymin = $mat->{y_min};
	my $ymax = $mat->{y_max};
	my $Yrange = &::IsNum($ymin) and &::IsNum($ymax);
	$ylimits = ".limits y [ $ymin, $ymax ]" if $Yrange;
		
	
	$::plot++;
	my $base = $::tag . "_" . $::plot;
	open(OUT,"> $base.dg") || die "can't create plot file";

print OUT<<EOF;
#!/bin/sh
# ColorDgraph.pl skips next line \\
exec perl $::draw -f \$0 "\$@"

# generated with command:
# $command
	
.view 0,0,1,1;
.pointsize 16;
.penwidth 2;
.ticlength 0.8;

.label x-axis "\\s-6$x_label"
.label y-axis "\\s-6$y_label"
.label screen 0.5 1.10 "$locus for material: $Material"
.label screen 0.5 1.05 "From data file: $DataFile"

$xlimits
$ylimits

.line
EOF

	while( 1 )
	{
		my @vars = &get_vars($x_var,$y_var);
		last if $#vars < 1;
		
		next if $Xrange and ( $vars[0] < $xmin or $xmax < $vars[0]);
		next if $Yrange and ( $vars[1] < $ymin or $ymax < $vars[1]);
		print OUT "$vars[0] $vars[1]\n";
	}
	close(WAVE);
	goto error if( $? != 0 );

print OUT<<EOF;
.black	
.pointsize 16;
.penwidth 2;
EOF
	close(OUT);
	chmod 0777, "$base.dg";
	system("$base.dg gid $$ out $base.eps&");
	#system("$base.dg fifo $::FIFO out $base.eps&");
	#open(FIFO,"<$::FIFO") or die "can't open FIFO: $!";
	#push @::plotid, <FIFO>;
	#close(FIFO);
			
	$::OldStatus = $::status_message = "plot file: $base.eps";
	return;
error : 
	close(OUT);
	close(WAVE);
	print "\nERROR with command\n$command\n";
	$mat->MaterialError(
		["Failed computing wave locus",
		"Error with command, $command",
		"Reset locus parameters"],
		);
}

sub get_vars
{
	my $line = <WAVE>;
	return undef unless  $line;
	chop $line;
	$line =~ s/^\s*//;
	my %s;
	($s{'V'},$s{'e'},$s{'u_p'}, $s{'P'},$s{'u_s'},$s{'c'},$s{'FD'},$s{'T'})
		= split(/\s+/,$line);

	my @out;
	my $var;	
	for $var ( @_ )	
	{

		push(@out, $s{$var});
	}
	return @out;
}

sub skip_lines
{
	for( $i=$_[0]; $i > 0 ; $i--)
	{
		$line = <WAVE>;
	}
}

sub label
{
	$_[0] =~ s/\_(.)/\\d\\s-4$1\\s+4\\u/g;
	$_[0] =~ s/\^(.)/\\u\\s-4$1\\s+4\\d/g;
	return $_[0];
}

1;
