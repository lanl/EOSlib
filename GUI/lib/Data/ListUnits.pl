sub ListUnits
{
	my $data = shift;
	my $units = $data->{units};
	my $DataFile = $data->{DataFile};
	
	return if not &::InList($units, $data->{Units}{$DataFile});
	my $command = "$::Run Data files $DataFile " .
                  "base Units type hydro name $units use";
	if( not open(DATABASE, "$command|") )
	{
		&::ErrorMessage(
		   "DataBase failed for units `$units'",
		   "Error with command, $command",
		   "Select new units or . . .",
		   );
		$data->{units} = undef;
		return;
	}
	
	my $top = $::mw->Toplevel();
	$top->transient($::mw);	
	$top->withdraw;
	$top->title("$units in DataBase file `$DataFile'");
	my $Parameters = $top->Scrolled("Text")
                   ->pack(-expand=>1, -fill=>'both', -anchor=>'w');
	while( <DATABASE> )
	{
		$Parameters->insert('end', $_);
	}
	close(DATABASE);
	if( $? != 0 )
	{
		&::ErrorMessage(
		   "DataBase failed for units `$units'",
		   "Error with command, $command",
		   "Select new units or . . .",
		   );
		$data->{units} = undef;
		$top->destroy();
		return;
	}
	
	$Parameters->configure( -state => 'disabled' );
	$top->geometry("+350+200");
	$top->raise;
	$top->deiconify;	
	# user to destroy window $top
}

1;
