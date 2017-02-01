sub NewMaterialWindow
{
	my $data = shift;

	my $top = $::mw->Toplevel();
	$top->withdraw;
	$top->group($::mw);
	$top->geometry("+10+5");
	$top->title("$::program: Material Window");

	my $mat = {};
	$mat->{'DataFile'} = \$data->{DataFile};
	$mat->{'units'}    = \$data->{units};
	$mat->{'Riemann'}  = $data->{Riemann};
	$mat->{'Parent'}   = $data;
	
	my $HeaderFrame = $top->Frame()
		->pack( -side=>'top',-anchor=>'w',-fill=>'x',-expand=>1);	
	$HeaderFrame->Label( -text=>"DataBase file", -width=>18 )
		->pack( -side=>'left' );
	$HeaderFrame->Label( -textvariable=>\$data->{DataFile},
	                     -relief=>'raised', -width=>32,
			     -anchor=>'w',
		)->pack( -side=>'left', -padx=>2 );
	$HeaderFrame->Label( -text=>"Units", -width=>16 )
		->pack( -side=>'left' );
	$HeaderFrame->Label( -textvariable=>\$data->{units},
	                     -relief=>'raised', -width=>10,
			     -anchor=>'w',
		)->pack( -side=>'left', -padx=>2 );
		
	my $canvas = $top->Scrolled('Canvas', -scrollbars=>'osow')
		->pack( -side=>'top',-anchor=>'w',-fill=>'x',-expand=>1);
	my $matframe = $mat->{'frame'} = $canvas->Frame();
	$mat = Material->new($mat);
	push( @{$data->{Materials}}, $mat );
	#my $width = 630;
	#my $height= 450;	
	my $width = 900;
	my $height= 500;	
	$canvas->createWindow(0,0, -window=>$matframe,
		-anchor=>'nw', -width=>$width, -height=>$height);
	$canvas->configure( -width=>$width, -height=>$height,
	                   -scrollregion=>[0,0, $width, $height] );

	my $DataBase = $data->{DataBase};
	if( scalar %$DataBase )
	{
	    $mat->MaterialList($DataBase);	    
	}
	else
	{
     	    $mat->{'DataBase'} = {};		
	}

	# user to destroy window $top
	$top->OnDestroy( sub{$data->DeleteMaterial($mat);} );
	$top->raise;
	$top->deiconify;
# ToDo: Add status line for each material window
}

1;
