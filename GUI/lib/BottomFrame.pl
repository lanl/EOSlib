sub BottomFrame
{
	$BottomFrame = $mw->Frame();
	$ExitButton = $BottomFrame->Button( -text => "Exit",
	             -command => \&Exit,
		     -activeforeground => 'red',
		     -highlightcolor => 'red',
		     -highlightthickness => 2,
		     -takefocus => 1,
		     -underline => 0,
	     ) ->pack( -side => 'left', -expand => 1);
	$mw->bind( '<Key-E>', sub{$ExitButton->invoke()} );
	&StatusMessage($ExitButton, "Quit and destroy window ?");
	
	$HelpButton = $BottomFrame->Button( -bitmap => 'question',
	             -command => \&NoHelp,
		     -activeforeground => 'red',
	     ) ->pack( -side => 'left' );
	&StatusMessage($HelpButton, "Help not yet available");
	
	$BottomFrame->pack( -side=>'bottom', -expand=>1, -fill=>'x', -anchor=>'s');
	
	$ExitWindow = $mw->Toplevel( -takefocus=>1, -borderwidth=>4 );
	$ExitWindow->withdraw();
	#$ExitWindow->resizable(0,0);
	$ExitWindow->geometry("+1000+30");
	$ExitWindow->title("MESSAGE");
	$ExitWindow->Label(
		-text => "EXIT " . $program . "?",
		-foreground => 'red',
		) ->pack( -side=>'top', -fill=>'both', -expand=>1 );
	$TerminateButton = $ExitWindow->Button(
		-text => "Terminate",
		-foreground => 'red',         -background => 'black',
		-activeforeground => 'black', -activebackground => 'red',
		-borderwidth => 4,
		-command => \&cleanup
		#-command => sub{ $mw->destroy(); $mw = 0; }
		) ->pack( -side=>'left', -fill=>'both', -expand=>1 );
	$TerminateButton->bind('<Return>', sub{$TerminateButton->invoke;} );
	$ContinueButton = $ExitWindow->Button(
	        -text => "Continue",
		-foreground => 'green',       -background => 'black',
		-activeforeground => 'black', -activebackground => 'green',
		-borderwidth => 4,
		-command => sub{
				$ExitWindow->withdraw;
				$mw->raise();
				$mw->deiconify();
			       }
		) ->pack( -side=>'left', -fill=>'both', -expand=>1 );
	$ContinueButton->bind('<Return>', sub{$ContinueButton->invoke;} );
	
	$mw->protocol('WM_DELETE_WINDOW', \&Exit);

	# quantities needed to get -state=>disabled to behave properly
	$disabledforeground = $HelpButton->cget( -disabledforeground );
	$foreground = $HelpButton->cget( -foreground );
	$highlightthickness = $HelpButton->cget(-highlightthickness);
	$highlightbackground = $HelpButton->cget(-highlightbackground);
	$highlightcolor = $HelpButton->cget(-highlightcolor);

	my $RadioButton = $mw->Radiobutton;
	$indicatorcolor = $RadioButton->cget(-selectcolor);
	$RadioButton->destroy;
}

sub Exit
{
	#$mw->withdraw;
	$mw->iconify;
	$ExitWindow->overrideredirect(1);
	$ExitWindow->raise();
	$ExitWindow->deiconify();
	$ExitWindow->bell();
	$ExitWindow->overrideredirect(0);
	$ExitWindow->bind('<Enter>', sub{$ExitWindow->focusForce;} );
}

sub NoHelp
{
	print "No help available\n";
	$HelpButton->configure( -state => 'disabled' );
}

1;
