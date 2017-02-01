sub DataEntry
{
	my $data = shift;
	my $frame = $data->{EntryFrame};
				
	my $DataButton = $data->{DataButton} = $frame->Button(
		-width => 14, -text=>"DataBase file",
		-command => sub{$data->ListDataFile},
        #-font => "lucidasans-bold",
	      )->pack( -side=>'left' );
	&::StatusMessage($DataButton, "List selected file");

	$data->{DataFile} = undef;
	my $FileEntry = $data->{FileEntry} = $frame->BrowseEntry(
            -width => 32,
			-variable => \$data->{DataFile},
			-browsecmd => sub{$data->FileSelected;}
		) ->pack( -side=>'left' );
	$FileEntry->bind('<Return>', sub{$data->FileSelected;} );
	&::StatusMessage($FileEntry, "Enter/Select file name of Data Base");
	
	my $UnitsButton = $data->{UnitsButton} = $frame->Button(
		-width => 8, -text=>"Units",
		-command => sub{$data->ListUnits},
	      )->pack( -side=>'left' );
	&::StatusMessage($UnitsButton, "List selected units");

	$data->{units} = undef;
	my $UnitsEntry = $data->{UnitsEntry} = $frame->BrowseEntry(
	  	-textvariable => \$data->{units},
        -width => 10,
#		-state => 'readonly', -takefocus=>1,
		-browsecmd => sub{ $data->UnitsReset;},
		) ->pack(-side=>'left');
	$UnitsEntry->bind('<Return>', sub{$data->UnitsReset;} );
#	&::StatusMessage($UnitsEntry, "Select name for system of units");
	$UnitsEntry->bind('<Enter>', [ \&::WidgetEnter,
				"Enter/Select name for system of units" ] );
	$UnitsEntry->bind('<Leave>', 
				sub{$data->LeaveUnitsEntry($UnitsEntry);} );	

	my $NewMaterialButton = $data->{NewMaterialButton} = $frame->Button(
			-text=>"Material",
			-command=>sub{$data->NewMaterialWindow;}
		) ->pack( -side=>'left', -padx => 4 );
	&::StatusMessage($NewMaterialButton, "Open New Material Window");
}

sub LeaveUnitsEntry
{
	my ($data, $widget) = @_;
	my $units = $data->{units};

	&::WidgetLeave($widget);

	return if not defined $units or $units eq "";

	my $DataFile = $data->{DataFile};
	my $Units = $data->{Units}{$DataFile};
	$units = undef unless &::InList($units, $Units);
}

sub UnitsReset
{
	my $data = shift;
	my $units = $data->{units};


	return if not defined $units or $units eq "";

	my $DataFile = $data->{DataFile};
	my $Units = $data->{Units}{$DataFile};

	if( not &::InList($units, $Units) )
	{
		$::OldStatus = $::status_message = "Error, Units not in list";
		$::mw->bell;
		$units = undef;
		return;
	}

	my $mat;
	my $DataBase = $data->{DataBase};
	for $mat ( @{$data->{Materials}} )
	{
		my $Material = $mat->{Material};
		if( $mat->{Material} and $DataBase->{$Material} )
		{
			$mat->{ref_state} = 'default';
			$mat->MaterialSelected;
		}
	}
}

sub InsertFiles
{
	my $data = shift;
	
	return if( $#_ < 0 );
		
	my $FileEntry = $data->{FileEntry};
	my $DataFiles = $data->{DataFiles};
	
	my $file;
	for $file ( @_)
	{
		$FileEntry->insert('end', $file);
		push( @{$DataFiles},$file );
	}
}


sub UnitsList
{
	my $data = shift;
	my $DataFile = $data->{DataFile};
	my $UnitsEntry = $data->{UnitsEntry};
	
	$UnitsEntry->delete(0,'end');
	my $units;
	for $units ( @{$data->{Units}{$DataFile}} )
	{
		$UnitsEntry->insert('end', $units);
	}	
}

sub FileEntryIndex
{
	my $data = shift;
	my $DataFile = $data->{DataFile};
	my $FileEntry = $data->{FileEntry};
	
	my $i = 0;
	my $file;
	for $file ( $FileEntry->get(0, 'end') )
	{
		if( $file eq $DataFile ) {return $i;}
		$i++;
	}
	return -1;
}

1;
