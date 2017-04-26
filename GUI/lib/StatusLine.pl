require Tk::Dialog;

sub StatusLine
{
	$StatusLine = $mw->Frame();
	$StatusLine->Label( -text => "status:", -relief => 'flat')
	           ->pack( -side=>'left' );
	$StatusFrame = $StatusLine->Frame( -relief=>'groove', -borderwidth=>2 );
	$StatusLabel = $StatusFrame->Label(
			  -textvariable => \$status_message,
			  -justify => 'left',
		       ) ->pack( -side=>'left' );
	$StatusFrame->pack(-side=>'left', -expand=>1, -fill=>'both');
	$StatusLine->pack( -side=>'bottom', -expand=>1, -fill=>'both', 
	                   -anchor=>'s');
			   
	$ErrorMessage = $mw->Dialog( -title => "$program: ERROR",
			          -buttons => [ qw( Continue ) ],
				  -default_button => "Continue",
				  -bitmap => 'error',
		      );
	
}


sub ErrorMessage
{
	my ($message, $err_status, $new_status) = @_;
	#limit size of status message to avoid window jumping
	$err_status =~ s/(.{0,85})(.*)/$1/;
	$err_status .= "  . . ." if defined $2 and $2 ne "";
	
	$OldStatus = $status_message =  $err_status;
	$ErrorMessage->configure( -text => $message );
	&ShowError;
	$OldStatus = $status_message = $new_status;
}

#
# $ErrorMessage->Show takes unduely long to display window 
# It also moves main window.
# This routine attempts to circumvent Bug
#
sub ShowError
{
	$ErrorMessage->raise;
	$ErrorMessage->deiconify;
	my $status = $ErrorMessage->Show();
#	$mw->geometry("-10+5");
	return $status;
}

sub StatusMessage
{
	my ($widget, $msg) = @_;
	$widget->bind('<Enter>', [ \&WidgetEnter, $msg ] );
	$widget->bind('<Leave>', \&WidgetLeave );
}

sub WidgetEnter
{
	return if $_[0]->cget( -state ) eq 'disabled';
	$OldStatus=$status_message;
	$status_message=$_[1];
}

sub WidgetLeave
{
	return if $_[0]->cget( -state ) eq 'disabled';
	$status_message=$OldStatus;
}

1;

