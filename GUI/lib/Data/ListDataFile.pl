sub ListDataFile
{
	my $data = shift;
	my $DataFile = $data->{DataFile};

	return if not defined $DataFile || $DataFile eq '';
=comment    
	if( not -r $DataFile )
	{
		$::OldStatus = $::status_message = "selected file not readable";
		$::mw->bell;
		return;
	}
=cut
	my $command = "$::Run Data files $DataFile "
                . "base EOS type '*' name '*' parameters";
	open(DATABASE, "$command|") || goto error;
	
	my $top = $::mw->Toplevel();
	$top->transient($::mw);
	#$top->group($::mw);
	$top->withdraw;
	$top->title("DataBase file `$DataFile'");
	my $file = $top->Scrolled("Text")
                   ->pack(-expand=>1, -fill=>'both', -anchor=>'w');
	while( <DATABASE> )
	{
		$file->insert('end', $_);
	}
	close(DATABASE);
	goto error if( $? != 0 );
	
	$file->configure( -state => 'disabled' );
	$top->geometry("+350+200");
	$top->raise;
	$top->deiconify;
	# use to destroy window $top
	return;

error :
	$mat->MaterialError(
		["DataBase failed for file `$DataFile'",
		"Error with command, $command",
		"Enter new data file"],
		1 );
	$top->destroy() if Exists($top);
	return;
}

1;
