package RiemannStateTie;

sub TIESCALAR
{
    my ($class, $riemann, $side) = @_; 
    my $state = [0, $riemann, $side];
    bless $state, $class;
    return $state;
}

# Method to handle reads of the tied variable
# simply return it's mat.
sub FETCH {
    my $state = shift;
    return $state->[0];
}

# Method to handle writes to the tied variable:
# 	store it's mat and set Material Name
sub STORE {
   my ($state, $mat) = @_;
   my $riemann = $state->[1];
   my $side = $state->[2];
   
   if( ! defined $mat )
   {
        $state->[0] = 0;
   }

   my $old = $state->[0];
   if( $old )
   {
    $old->NormalRPFrame;
   	my $InitState = tied $old->{SetInitState};
	if( defined $InitState )
	{
		my $tmp = $InitState->[0];
		untie $InitState->[0];
		$InitState->[0] = $tmp;
		
		# $state->[0] = 0; # doesn't take effect until return
		$riemann->StateChanged(1); # force DisableState
	}
   }

   unless( $mat )
   {
       my $name=undef;
       $riemann->{$side . "MaterialLabel"}->configure(-textvariable=>\$name);
       $state->[0] = 0;
       return $mat;
   }

   if( $state->[2] eq 'Left' )
   {
       $riemann->{RightMaterial} = 0 if $riemann->{RightMaterial} == $mat;
       $riemann->{LeftMaterialLabel}->configure(
       			-textvariable=>\$mat->{Material} );       
   }
   elsif( $state->[2] eq 'Right' )
   {
       $riemann->{LeftMaterial} = 0 if $riemann->{LeftMaterial} == $mat;
       $riemann->{RightMaterialLabel}->configure(
       			-textvariable=>\$mat->{Material} );       
   }
    
   $state->[0] = $mat;
   my $InitState = tied $mat->{SetInitState};
   my $tmp = $InitState->[0];
   tie $InitState->[0], 'StateReadyTie', $tmp, $riemann;
   #
   $riemann->StateChanged;
   $riemann->{mat_left_rb} = "$riemann->{LeftMaterial}";
   $riemann->{mat_right_rb} = "$riemann->{RightMaterial}";
   $mat->NormalRPFrame;

   return $mat;
}

# class destructor (unused)
sub DESTROY {
}

1;

