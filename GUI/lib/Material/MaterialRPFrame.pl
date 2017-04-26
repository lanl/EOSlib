    local $mat_tmp = 0;
sub MaterialRPFrame
{
	my $mat = shift;
	my $frame = $mat->{RPFrame};
	my $riemann = $mat->{Riemann};

	$mat->{RP_Label} = $frame->Label( -text => "Set Riemann Problem" )
		->pack( -side=>'left');
	$mat->{RPleft_rb} = $frame->Radiobutton(
		-text => "left state",
		-variable => \$riemann->{mat_left_rb},
		-value => "$mat",
        -command => sub{$riemann->{LeftMaterial}=$mat;},
		) ->pack( -side=>'left' );
	$mat->{RPright_rb} = $frame->Radiobutton(
		-text => "right state",
		-variable => \$riemann->{mat_right_rb},
		-value => "$mat",
        -command => sub{$riemann->{RightMaterial}=$mat;},
		) ->pack( -side=>'left' );
}

sub DisabledRPFrame
{
	my ($mat) = @_;
				
	my $button;
	foreach $button ( qw/ RPleft_rb RPright_rb / )
	{
		$mat->{$button}->configure(
				-state=>'disabled',
	            -highlightcolor=>$::highlightbackground,
				-selectcolor=>$::disabledforeground,
			);
	}
}

sub NormalRPFrame
{
	my ($mat) = @_;
	my $riemann = $mat->{Riemann};
=skip
    if( $riemann->{LeftMaterial} == $mat )
    {
		$mat->{RPleft_rb}->configure(
				-state=>'normal',
	        	-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::indicatorcolor,
			);
    }
    else
    {
		$mat->{RPleft_rb}->configure(
				-state=>'normal',
	            -highlightcolor=>$::highlightbackground,
				-selectcolor=>$::disabledforeground,
			);
    }

    if( $riemann->{RightMaterial} == $mat )
    {
		$mat->{RPright_rb}->configure(
				-state=>'normal',
	        	-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::indicatorcolor,
			);
    }
    else
    {
		$mat->{RPright_rb}->configure(
				-state=>'normal',
	            -highlightcolor=>$::highlightbackground,
				-selectcolor=>$::disabledforeground,
			);
    }
=cut

#=skip
	my $button;
	foreach $button ( qw/ RPleft_rb RPright_rb / )
	{
		$mat->{$button}->configure(
				-state=>'normal',
	        	-highlightcolor=>$::highlightcolor,
				-selectcolor=>$::indicatorcolor,
			);
	}
#=cut
}

1;
