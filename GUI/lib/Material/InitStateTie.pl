package InitStateTie;

sub TIESCALAR
{
    my ($class, $mat) = @_; 
    my $state = [undef, $mat];
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
# 	store it's value and set dependencies
sub STORE {
   my ($state, $value) = @_;
   $state->[0] = $value;

   my $mat = $state->[1];
   if( $value eq 'disabled')
   {
       $mat->DisabledInitStateFrame;
       $mat->DisabledLocusFrame;
       $mat->DisabledRPFrame;
   }
   elsif( $value eq 'active' )
   {
       $mat->NormalInitStateFrame;
       $mat->DisabledLocusFrame;
       $mat->DisabledRPFrame;
       
   }
   elsif( $value eq 'set' )
   {
       $mat->NormalInitStateFrame;
       $mat->NormalLocusFrame;
       $mat->NormalRPFrame;
   }
   else
   {
       print "program error: unknown value for SetInitState(= $value)\n";
   }
   $mat->SetInitState_rb;
   return $value;
}

# class destructor (unused)
sub DESTROY {
}

1;

