sub Parameters
{
	my $mat = shift;
	my $Material = $mat->{Material};
	return if not $Material;

	my $DataFile = ${$mat->{DataFile}};
	my $units = ${$mat->{units}};

	my $arg = "material $Material";
	$arg .= " units hydro::$units" if $units;
	my $command ="$::Run EOSdata file $DataFile $arg";
	if( not open(DATABASE, "$command|") )
	{
		$mat->MaterialError(
		          ["DataBase failed for material `$Material'",
		   	   "Error with command, $command",
			   "Enter new Material"],
			  'RefState');
		return;
	}
	
	my $top = $::mw->Toplevel();
	$top->transient($::mw);
	#$top->group($::mw);
	$top->withdraw;
	$top->title("$Material in DataBase file `$DataFile'");
	my $Parameters = $top->Scrolled("Text")->pack();
	while( <DATABASE> )
	{
		$Parameters->insert('end', $_);
	}
	close(DATABASE);
	if( $? != 0 )
	{
		$mat->MaterialError(
	           ["DataBase failed for material `$Material'",
		    "Error with command, $command",
		    "Enter new Material"],
		   'RefState');
		$top->destroy();
		return;
	}
	
	$Parameters->configure( -state => 'disabled' );
	$top->geometry("+350+200");
	$top->raise;
	$top->deiconify;
	# use to destroy window $top
}


1;
