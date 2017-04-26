require 'Parameters.pl';
require 'MaterialSelected.pl';

sub MaterialEntryFrame
{
	my $mat = shift;
	my $frame = $mat->{EntryFrame};

	my $MaterialButton = $mat->{MaterialButton} = $frame->Button(
			-width => 14, -text=>"Material",
	             -command => sub{$mat->Parameters;},
		)->pack( -side=>'left' );
	&::StatusMessage($MaterialButton,
		"List parameters for selected material");

	$mat->{OldMat} = $mat->{Material} = undef;
	my $MaterialEntry = $mat->{MaterialEntry} = $frame->BrowseEntry(
#			-label => "Material", -labelWidth=>14,
            -width => 32,
			-variable => \$mat->{Material},
			-browsecmd => sub{$mat->MaterialSelected;},
		) ->pack( -side=>'left' );
	&::StatusMessage($MaterialEntry, "Enter/Select material in Data Base");
	$MaterialEntry->bind('<Return>', sub{$mat->MaterialSelected;} );
	$MaterialEntry->bind('<Enter>', sub{$mat->{OldMat} = $mat->{Material};}
		);	
	$MaterialEntry->bind('<Leave>', sub{$mat->{Material} = $mat->{OldMat};}
		);
}

sub MaterialList
{
	my $mat = shift;
	return unless $_[0];
	$mat->{DataBase} = $_[0];

	my $MaterialEntry = $mat->{MaterialEntry};	
	$MaterialEntry->delete(0,'end');
	
	my $material;
	for $material ( sort keys %{$mat->{DataBase}} )
	{
		$MaterialEntry->insert('end', $material);
	}
	$mat->{SetRefState} = 'active';
}

sub DisabledMaterialEntryFrame
{
	my ($mat) = @_;

	$mat->{MaterialButton}->configure(
		        	-takefocus=>0, -state=>'disabled',
				-foreground => $::disabledforeground,
				-highlightcolor=>$::highlightbackground,
				-highlightbackground=>$::highlightbackground,
		);		
	$mat->{MaterialEntry}->configure(
		        	-takefocus=>0, -state=>'disabled',
				-foreground => $::disabledforeground,
				-highlightcolor=>$::highlightbackground,
				-highlightbackground=>$::highlightbackground,
		);
}

sub NormalMaterialEntryFrame
{
	my ($mat) = @_;

	$mat->{MaterialEntry}->configure(
		        	-takefocus=>1, -state=>'normal',
				-foreground => $::foreground,
				-highlightcolor=>$::highlightcolor,
				-highlightbackground=>$::highlightbackground,
			);

	if( defined $mat->{Material}  and $mat->{Material} )
	{	
		$mat->{MaterialButton}->configure(
		        	-takefocus=>1, -state=>'normal',
				-foreground => $::foreground,
				-highlightcolor=>$::highlightcolor,		
			);
	}
	else
	{
		$mat->{MaterialButton}->configure(
		        	-takefocus=>0, -state=>'disabled',
				-foreground => $::disabledforeground,
				-highlightcolor=>$::highlightbackground,
		);
		$mat->{MaterialEntry}->configure(
			-highlightcolor=>'red',
			-highlightbackground=>'red',
		);
	}
}

1;

