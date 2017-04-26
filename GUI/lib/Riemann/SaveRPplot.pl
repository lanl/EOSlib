sub SaveRPplot
{
	my $riemann = shift;

	my $file = $riemann->{SaveFile};
	$file =~ s/\..*$//;
	$file .= '.agr';

	my $plot = $riemann->{PlotFile} . ".agr";
	if( not -r $plot )
	{
		$::OldStatus = $::status_message
		             = "ERROR, plot file not present\n";
		return;
	}

	if( system("cp $plot $file") )
	{
		$::OldStatus = $::status_message
		             = "ERROR, could not copy to `$file'\n";
	}
	else
	{
		$::OldStatus = $::status_message
			= "Riemann problem plot saved in file `$file'";		
	}
}

1;
