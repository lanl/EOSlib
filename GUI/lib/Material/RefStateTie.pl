package RefStateTie;

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
       $mat->{OldMat} = $mat->{Material} = undef;
       $mat->DisabledMaterialEntryFrame;
       $mat->DisabledRefStateFrame;
       $mat->{SetInitState} = 'disabled';
   }
   elsif( $value eq 'active' )
   {
       $mat->NormalMaterialEntryFrame;
       if( $mat->{Material} )
       {
           $mat->NormalRefStateFrame;
       }
       else
       {
           $mat->DisabledRefStateFrame;
       }
       $mat->{SetInitState} = 'disabled';
   }
   elsif( $value eq 'set' )
   {
       $mat->NormalMaterialEntryFrame;
       $mat->NormalRefStateFrame;       
       $mat->{SetInitState} = 'active';
   }
   else
   {
       print "program error: unknown value for SetRefState(= $value)\n";
   }
   return $value;
}

# class destructor (unused)
sub DESTROY {
}

1;

