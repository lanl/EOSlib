package StateReadyTie;

sub TIESCALAR
{
    my ($class, $value, $riemann) = @_; 
    my $state = [$value, $riemann];
    bless $state, $class;
    return $state;
}

# Method to handle reads of the tied variable
# simply return it's value.
sub FETCH {
    my $state = shift;
    return $state->[0];
}

# Method to handle writes to the tied variable:
# 	store it's value and call StateChanged
sub STORE {
   my ($state, $value) = @_;
   $state->[0] = $value;
   $state->[1]->StateChanged;
   return $value;
}

# class destructor (unused)
sub DESTROY {
}

1;

