package State;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw( SetStateValues    SetValues       SetStateUnits	  SetUnits
              CopyState         EvaluateState   SetStateNull
	      DisabledVariables NormalVariables DisabledVariables
	    );


=state keys
############################################
# usage:
#	$state = State->new($frame, var=>label, ... );
frame (Frame widget)
var = {hash}: var_label  => Label widget
	      unit_label => Label widget
	      Entry      => Entry widget
              unit       => $string
	      value      => $string
	      fvalue       => formated string (%.6g)
# example of usage of State
print "Keys ", keys(%$State), "\n";
print            "state: ", $State,
             " state{V}: ", $State->{V},
                " value: ", $State->{V}{value}, "\n";
############################################
=cut

#local subroutines
sub StateVariable;
sub LeaveState;

sub new
{
	my $class = shift;
	my $state = {};
	bless $state, $class;

	$state->{frame} = shift;
	while( scalar @_ > 0 )
	{
		$state->StateVariable( shift, shift );
	}

	return $state;
}

sub StateVariable
{
	my ($state,$var,$label) = @_;
	my $frame = $state->{frame};
	my $f = $frame->Frame()->pack( -side=>'left', -expand=>1 );

	my %hash;
	$hash{var_label} = $f->Label( -text=>$label, -width=>12 )
	                ->pack( -side=>'top' );
	$hash{unit} = undef;
	$hash{unit_label}  = $f->Label(
				-textvar=>\$hash{unit},
				-width=>12,
			)->pack( -side=>'top' );
	$hash{value}  = undef;
	$hash{fvalue} = undef;
	$hash{Entry}  = $f->Entry(
				-textvar=>\$hash{fvalue},
				-width=>12,
			)->pack( -side=>'top' );
	$hash{Entry}->bind('<Leave>', [\&LeaveState, \$hash{fvalue}] );

	$state->{$var} = \%hash;
}

sub LeaveState
{
	my ($widget, $value) = @_;
	return if $widget->cget( -state ) eq 'disabled';

	return if( (! defined $$value) or &::IsNum($$value) );
	$::mw->bell;
	$$value=undef;
}

# $state->SetStateUnits( var=>unit, ... )
sub SetStateUnits
{
	my $state = shift;
	
	while( scalar @_ > 0 )
	{
		my $var  = shift;
		my $unit = shift;
		$state->{$var}{unit} = '(' . $unit . ')';
	}
}

# $state->SetUnits( unit, var, ... )
sub SetUnits
{
	my ($state,$unit,@Vars) = @_;
	@Vars = keys %$state if scalar @Vars == 0;
	
	my $var;
	for $var ( keys %$state)
	{
		next if $var eq 'frame';
		$state->{$var}{unit} = $unit;
	}
}

sub SetStateNull
{
	my $state = shift;
	my $var;
	for $var ( keys %$state)
	{
		next if $var eq 'frame';
		$state->{$var}{value} = undef;
		$state->{$var}{fvalue} = undef;
		$state->{$var}{unit}  = undef;
	}
}


# $state->SetStateValues( var=>value, ... )
sub SetStateValues
{
	my $state = shift;

	if( scalar @_ == 0 )
	{
	    my $var;
	    for $var ( keys %$state )
	    {
		next if $var eq 'frame';
		$state->{$var}{value} = undef;
		$state->{$var}{fvalue} = undef;
	    }
	}
	else
	{	
	    while( scalar @_ > 0 )
	    {
	    	my $var   = shift;
		my $value = shift;
		$state->{$var}{value} = $value;
		$state->{$var}{fvalue} = &::IsNum($value) ?
		                         sprintf("%.6g",$value) : $value;
	    }
	}
}
# $state->SetValues( value, var, ... )
sub SetValues
{
	my ($state,$value,@Vars) = @_;
	@Vars = keys %$state if scalar @Vars == 0;
	
	my $var;
	my $fvalue = &::IsNum($value) ? sprintf("%.6g",$value) : $value;
	for $var ( @Vars )
	{
            next if $var eq 'frame';
	    $state->{$var}{value}  = $value;
	    $state->{$var}{fvalue} = $fvalue;
	}
}


sub CopyState
{
	my $x = shift;
	my $y = shift;

	my $var;
	for $var ( keys %$y )
	{
		next if $var eq 'frame';
		next unless exists $x->{$var};
		$x->{$var}{value}  = $y->{$var}{value};
		$x->{$var}{fvalue} = $y->{$var}{fvalue};
		$x->{$var}{unit}   = $y->{$var}{unit};
	}
}

# $Values = $state->EvaluateState( $tag, var, ... );
sub EvaluateState
{
	my $state = shift;
	my $tag   = shift;
	
	my @Values;
	my ($var,$val);
	for $var ( @_ )
	{
		$val = $state->{$var}{value};
		push(@Values, $var . $tag, $val) if defined $val and $val;
	}
	return join(' ',@Values);
}
sub EvaluateFstate
{
	my $state = shift;
	my $tag   = shift;
	
	my @Values;
	my ($var,$val);
	for $var ( @_ )
	{
		$val = $state->{$var}{fvalue};
		push(@Values, $var . $tag, $val) if defined $val and $val;
	}
	return join(' ',@Values);
}

# $state->DisabledVariables( var, ... )
sub DisabledVariables
{
	my ($state, @Vars ) = @_;
	@Vars = keys %$state if scalar @Vars == 0;

	my $var;	
	for $var ( @Vars )
	{
		next if $var eq 'frame';
		$state->{$var}{Entry}->configure(
		        	-takefocus=>0, -state=>'disabled',
				-highlightcolor => $::highlightbackground,
				-foreground => $::disabledforeground,
			);
		$state->{$var}{var_label}->configure(
				-foreground=>$::disabledforeground);
		$state->{$var}{unit_label}->configure(
				-foreground=>$::disabledforeground);
	}
}


# $state->NormalVariables( var, ... )
sub NormalVariables
{
	my ($state, @Vars ) = @_;
	@Vars = keys %$state if scalar @Vars == 0;

	my $var;	
	for $var ( @Vars )
	{
		next if $var eq 'frame';
		$state->{$var}{Entry}->configure(
		        	-takefocus=>1, -state=>'normal',
				-highlightcolor => $::highlightcolor,
				-foreground => $::foreground,
			);
		$state->{$var}{var_label}->configure(
				-foreground=>$::foreground);
		$state->{$var}{unit_label}->configure(
				-foreground=>$::foreground);
	}
}

sub DeselectVariables
{
	my ($state, @Vars ) = @_;
	@Vars = keys %$state if scalar @Vars == 0;

	my $var;	
	for $var ( @Vars )
	{
		next if $var eq 'frame';
		$state->{$var}{Entry}->configure(
		        	-takefocus=>0, -state=>'readonly',
				-highlightcolor => $::highlightbackground,
				-foreground => $::foreground,
			);
		$state->{$var}{var_label}->configure(
				-foreground=>$::foreground);
		$state->{$var}{unit_label}->configure(
				-foreground=>$::foreground);
	}
}

1;
