sub FileSelected
{
	my $data = shift;
	$data->{DataFile} = $_[0] if defined $_[0];
	my $DataFile = $data->{DataFile};
	return unless defined $DataFile and $DataFile;

	my $mat;
	for $mat ( @{$data->{Materials}} )
	{
		$mat->{SetRefState}='disabled';
	}
	$data->{UnitsEntry}->delete(0,'end');
	
	if( not $data->{DataBases}{$DataFile} )
	{
		if( $data->ReadFile )
		{
			$data->{DataBase} = {};
			return 1;
		}
		$data->{FileEntry}->insert('end', $DataFile)
			if not &::InList($DataFile,$data->{DataFiles});
	}

	my $DataBase = $data->{DataBase} = $data->{DataBases}{$DataFile};
	for $mat ( @{$data->{Materials}} )
	{
		$mat->MaterialList($DataBase);
	}
	$data->UnitsList;
	$data->{units} = "";

	$mat = ${$data->{Materials}}->[0]
	{$mat->{MaterialEntry}}->focus() if $mat;

	return 0;
}

sub ReadFile
{
	my $data = shift;
	my $DataFile = $data->{DataFile};
=comment	
	if( not -r $DataFile )
	{
		$data->Reset(
		   "Selected file `$DataFile' does not exist",
		   "Error, file does not exist",
		   "Select new data file");
		return 1;
	}
=cut
    my $line;
    my $command = "$::Run Data files $DataFile base EOS type '*' name '*'";
    
	if( not open (DATABASE, "$command|") )
	{
		$data->Reset(
		   "ReadFile failed on file `$DataFile'",
		   "Error reading EOS Data Base",
		   "Select new data file");
		return 1;
	}
	my %db;
	while( <DATABASE> )
	{
		chop;
		my ($material, $units) = split /\s+/;
        $material =~ s/^EOS://;
        if( $units =~ /^Units:hydro::/ )
        {
            $units = $';
        }
	    $db{$material} = $units;
	}
	close(DATABASE);
	if( $? != 0 )
	{
		$data->Reset(
		   "ReadFile failed on file `$DataFile'",
		   "Error reading EOS from Data Base",
		   "Select new data file");
		return 1;
	}

    $command = "$::Run Data files $DataFile base Units type hydro name '*'";
	if( not open (DATABASE, "$command|") )
	{
		$data->Reset(
		   "ReadFile failed on file `$DataFile'",
		   "Error reading Units from Data Base",
		   "Select new data file");
		return 1;
	}
	my @units;
	while( ($line=<DATABASE>) )
	{
		chop $line;
		$line =~ s/^Units:hydro:://;
        push(@units, $line);
	}
	close(DATABASE);
	if( $? != 0 )
	{
		$data->Reset(
		   "ReadFile failed on file `$DataFile'",
		   "Error reading Units from Data Base",
		   "Select new data file");
		return 1;
	}
	
	$data->{DataBases}{$DataFile} = \%db;
	$data->{Units}{$DataFile} = \@units;

	$::OldStatus = $::status_message = "Select Material";
	return 0;
}

sub Reset
{
	my $data = shift;

	$data->{units} = undef;		
	$data->{UnitsEntry}->delete(0,'end');
		
	$data->{DataBase} = {};
	my $mat;
 	for $mat ( @{$data->{Materials}} )
	{
		$mat->{SetRefState}='disabled';
	}				
	$data->{DataFile} = undef;

	&::ErrorMessage(@_) if( $#_ >= 0 );
}

1;
