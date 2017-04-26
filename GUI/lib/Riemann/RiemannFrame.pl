require 'SolveRP.pl';
require 'SaveRPplot.pl';

sub HeaderFrame
{
	my $riemann = shift;
	my $frame = $riemann->{HeaderFrame};
	
	$riemann->{RiemannLabel} = $frame->Label(
			-text => "Riemann Problem",
		#	-width => 15,
		) ->pack( -side=>'left' );
	my $SolveButton = $riemann->{SolveButton} = $frame->Button(
			-text => "solve",
			-command => sub{$riemann->SolveRP;},
		) ->pack( -side=>'left' );
	&::StatusMessage($SolveButton, "Solve Riemann Problem");
	
	$riemann->{plot} = 'off';
	my $PlotRP_cb = $riemann->{PlotRP_cb} = $frame->Checkbutton(
			-text => "plot u-P loci",
			-variable => \$riemann->{plot},
			-onvalue  => 'on',
			-offvalue => 'off',
		) ->pack( -side=>'left', -padx=>5 );	
	&::StatusMessage($PlotRP_cb,
			"Plot wave curves when solve button is pressed");
	$riemann->{lock} = 'off';
	my $lockRP_cb = $riemann->{lockRP_cb} = $frame->Checkbutton(
			-text => "lock",
			-variable => \$riemann->{lock},
			-onvalue  => 'on',
			-offvalue => 'off',
			-command  => sub{$riemann->lockRP_cb;},
		) ->pack( -side=>'left', -padx=>5 );	
	&::StatusMessage($lockRP_cb, "Lock/Unlock Riemann Problem display");

	$riemann->{FileLabel} = $frame->Label( -text=>"save" )
		->pack(-side=>'left');
	$riemann->{SaveFile} = undef;
	my $FileEntry = $riemann->{FileEntry} = $frame->Entry(
			-width =>15,
			-textvariable=>\$riemann->{SaveFile},
		) ->pack( -side=>'left' );
	&::StatusMessage($FileEntry, "Save dgraph file of wave curves");
	$FileEntry->bind('<Return>', sub{$riemann->SaveRPplot;});	
}

sub LeftMaterialFrame
{
	my $riemann = shift;
	my $frame = $riemann->{LeftMaterialFrame};
	
	$riemann->{LeftMaterial} = 0;
	tie $riemann->{LeftMaterial}, 'RiemannStateTie', $riemann, 'Left';
	my $LeftHeaderFrame = $frame->Frame()
		->pack( -side => 'top', -expand=>1, -anchor=>'w', -pady=>5 );
	$riemann->{LeftLabel} = $LeftHeaderFrame->Label(
			-text => "Left material",
			-width =>15,
		) ->pack( -side=>'left' );
	$riemann->{LeftMaterialLabel} = $LeftHeaderFrame->Label(
			-width => 20,
			-relief => 'sunken',
			) ->pack( -side=>'left' );
	my $LeftStateFrame = $frame->Frame()
		->pack( -side => 'top', -expand=>1, -anchor=>'w' );
	$riemann->{LeftState} = State->new($LeftStateFrame,
					qw(V V e e u u P P T T c c u_s u_s) );
}

sub RightMaterialFrame
{
	my $riemann = shift;
	my $frame = $riemann->{RightMaterialFrame};
	
	$riemann->{RightMaterial} = 0;
	tie $riemann->{RightMaterial}, 'RiemannStateTie', $riemann, 'Right';

	my $RightHeaderFrame = $frame->Frame()
		->pack( -side => 'top', -expand=>1, -anchor=>'w', -pady=>5 );
	$riemann->{RightLabel} = $RightHeaderFrame->Label(
			-text => "Right material",
			-width =>15,
		) ->pack( -side=>'left' );
	$riemann->{RightMaterialLabel} = $RightHeaderFrame->Label(
			-width => 20,
			-relief => 'sunken',
			) ->pack( -side=>'left' );
	
	my $RightStateFrame = $frame->Frame()
		->pack( -side => 'top', -expand=>1, -anchor=>'w' );
	$riemann->{RightState} = State->new($RightStateFrame,	
					qw(V V e e u u P P T T c c u_s u_s) );
}

sub CheckRPlock
{
	my ($riemann,$mat) = @_;
	return if $riemann->{lock} eq 'off';
	return if $riemann->{LeftMaterial} != $mat
		and $riemann->{RightMaterial} != $mat;
	$riemann->{lock} = 'off';
	$riemann->lockRP_cb();
}


sub lockRP_cb
{
	my $riemann = shift;
	if( $riemann->{lock} eq 'on' )
	{
		my $side;
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
		return;
	}
	$riemann->StateChanged;
	my $side;
	foreach $side ( qw(Left Right) )
	{
	    my $state = $riemann->{$side . 'State'};
	    $state->SetStateNull;
	    $state->DisabledVariables;
	}
	$::OldStatus = $::status_message = "Ready and waiting";
}

1;
