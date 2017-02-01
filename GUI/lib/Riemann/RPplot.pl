sub RPplot
{
    my $riemann = shift;
    my $LeftMaterial = $riemann->{LeftMaterial};
    my $RightMaterial = $riemann->{RightMaterial};

    my $LeftState = $riemann->{LeftState};
    my %left    = split(/ /,
        $LeftMaterial->{InitState}->EvaluateState('', qw(u P)) );
        
    my $RightState = $riemann->{RightState};    
    my %right   = split(/ /,
        $RightMaterial->{InitState}->EvaluateState('', qw(u P)) );
        
    my %contact_l = split(/ /, $LeftState->EvaluateState('', qw(u P)) );
    my %contact_r = split(/ /, $RightState->EvaluateState('', qw(u P)) );
    my @type;
    $type[0] = $left{P}  < $contact_l{P} ? 'shock' : 'isentrope';
    $type[1] = $right{P} < $contact_r{P} ? 'shock' : 'isentrope';

    # find bounds
    #
    my $xmin = ::min($left{'u'},$right{'u'});
    my $xmax = ::max($left{'u'},$right{'u'});
    my $ymin = ::min($left{'P'},$right{'P'});
    my $ymax = ::max($left{'P'},$right{'P'});
    #
    #
    # shock-shock
    if( $type[0] eq 'shock' && $type[1] eq 'shock' )
    {
        my $mat = $LeftMaterial;
        my $command  = "$::Run Wave file ${$mat->{DataFile}}";
        $command .= " material $mat->{Material}";
        $command .= ' ' . $mat->{InitState}->EvaluateState('0', qw(V e u) );
        $command .= " init_left u_p $right{u}";
        my $units = ${$mat->{units}};
        $command .= " units hydro::$units" if $units;
        open(WAVE, "$command |");
        &skip_lines(6); # skip header
        my $line = <WAVE>;
        $line =~ s/^\s*//;
        my ($V,$e,$u, $Pleft) = split(/\s+/,$line);

        $mat = $RightMaterial;
        $command  = "$::Run Wave file ${$mat->{DataFile}}";
        $command .= " material $mat->{Material}";
        $command .= ' ' . $mat->{InitState}->EvaluateState('0', qw(V e u) );
        $units = ${$mat->{units}};
        $command .= " u_p $left{u}";
        $command .= " units hydro::$units" if $units;
        open(WAVE, "$command|");
        &skip_lines(6); # skip header
        $line = <WAVE>;
        $line =~ s/^\s*//;
        ($V,$e,$u, $Pright) = split(/\s+/,$line);
        $ymax = ::min($Pleft,$Pright);
    }
    # shock-isentrope
    if( $type[0] eq 'shock' && $type[1] eq 'isentrope' )
    {
        my $mat = $LeftMaterial;
        my $command  = "$::Run Wave file ${$mat->{DataFile}}";
        $command .= " material $mat->{Material}";
        $command .= ' ' . $mat->{InitState}->EvaluateState('0', qw(V e u) );
        $command .= " init_left Ps $right{'P'}";
        my $units = ${$LeftMaterial->{units}};
        $command .= " units hydro::$units" if $units;
        open(WAVE, "$command |");
        &skip_lines(6); # skip header
        my $line = <WAVE>;
        $line =~ s/^\s*//;
        my ($V,$e,$uleft) = split(/\s+/,$line);
        $xmax = $uleft;
    }
    # isentrope-shock
    if( $type[0] eq 'isentrope' && $type[1] eq 'shock' )
    {
        my $mat = $RightMaterial;
        my $command  = "$::Run Wave file ${$mat->{DataFile}}";
        $command .= " material $mat->{Material}";
        $command .= ' ' . $mat->{InitState}->EvaluateState('0', qw(V e u) );
        $command .= " Ps $left{'P'}";
        my $units = ${$mat->{units}};
        $command .= " units hydro::$units" if $units;
        open(WAVE, "$command |");
        &skip_lines(6); # skip header
        my $line = <WAVE>;
        $line =~ s/^\s*//;
        my ($V,$e,$uright) = split(/\s+/,$line);
        $xmax = $uright;
    }
    # isentrope-isentrope
    else
    {
        # ToDo: check for u > escape velocity
        my $mat = $LeftMaterial;
        my $command  = "$::Run Wave file ${$mat->{DataFile}}";
        $command .= " material $mat->{Material}";
        $command .= ' ' . $mat->{InitState}->EvaluateState('0', qw(V e u) );
        $command .= " left u2 $right{'u'} nsteps 0 valid_u";
        my $units = ${$LeftMaterial->{units}};
        $command .= " units hydro::$units" if $units;
        open(WAVE, "$command |");
        &skip_lines(6); # skip header
        my $line = <WAVE>;
        $line =~ s/^\s*//;
        my ($V,$e,$u,$Pleft) = split(/\s+/,$line);

        $mat = $RightMaterial;
        $command  = "$::Run Wave file ${$RightMaterial->{DataFile}}";
        $command .= " material $mat->{Material}";
        $command .= ' ' . $mat->{InitState}->EvaluateState('0', qw(V e u) );
        $command .= " u2 $left{'u'} nsteps 0 valid_u";
        $units = ${$RightMaterial->{units}};
        $command .= " units hydro::$units" if $units;
        open(WAVE, "$command |");
        &skip_lines(6); # skip header
        $line = <WAVE>;
        $line =~ s/^\s*//;
        ($V,$e,$u,$Pright) = split(/\s+/,$line);
        $ymin = ::min($Pleft,$Pright);
    }
    my $xaxis_min = $xmin;
    my $xaxis_max = $xmax;
    my $yaxis_min = $ymin;
    my $yaxis_max = $ymax;
    for $mat ( $LeftMaterial, $RightMaterial )
    {
        next unless $mat->{init_state} eq 'InitShock';
        my $u = $mat->{RefState}->{u}{value};
        my $P = $mat->{RefState}->{P}{value};
        $xaxis_min = ::min($xaxis_min, $u);
        $xaxis_max = ::max($xaxis_max, $u);
        $yaxis_min = ::min($yaxis_min, $P);
        $yaxis_max = ::max($yaxis_max, $P);
    }
    #
    # Plot
    $::plot++;
    my $base = $::tag . "_" . $::plot;
    open(xmgrace::PLOT,"> $base.agr") || die "can't create plot file";


    if( $LeftMaterial->{x_var} eq 'u_p'
                and &::IsNum($LeftMaterial->{x_min})
                and &::IsNum($LeftMaterial->{x_max}) )
    {
        $xaxis_min = $LeftMaterial->{x_min};
        $xaxis_max = $LeftMaterial->{x_max};
        $xmin = ::min($xmin,$xaxis_min);
        $xmax = ::max($xmax,$xaxis_max);
    }
    
    if( $LeftMaterial->{y_var} eq 'P'
                and &::IsNum($LeftMaterial->{y_min})
                and &::IsNum($LeftMaterial->{y_max}) )
    {
        $yaxis_min = $LeftMaterial->{y_min};
        $yaxis_max = $LeftMaterial->{y_max};
        $ymin = ::min($ymin,$yaxis_min);
        $ymax = ::max($ymax,$yaxis_max);
    }
    $yaxis_min = 0 if $yaxis_min < 0.001*$yaxis_max;

    $xmgrace::xlabel =  "u\\sp\\N $LeftState->{u}{unit}";
    $xmgrace::xmin = $xaxis_min;
    $xmgrace::xmax = $xaxis_max;
    $xmgrace::xmajor = 5;        # major tics
    $xmgrace::xminor = 4;        # number of minor tics per major tic
    $xmgrace::xskip  = 0;        # skip labels on major tics


    $xmgrace::ylabel = "P $LeftState->{P}{unit}";
    $xmgrace::ymin = $yaxis_min;
    $xmgrace::ymax = $yaxis_max;
    $xmgrace::ymajor = 5;
    $xmgrace::yminor = 4;
    $xmgrace::yskip  = 0;
    #
    $xmgrace::view   = '0.22, 0.20, 1.23, 0.88';
    $xmgrace::title = "\\R{blue}$LeftMaterial->{Material}\\R{} /"
                      . " \\R{red}$RightMaterial->{Material}";
    $xmgrace::subtitle = "solid shock, dashed rarefaction, dotted initial shock";
    $xmgrace::legend = "";
    $xmgrace::legend_x = 0.78;
    $xmgrace::legend_y = 0.85;
    #
    &xmgrace::header;


    # mark solution
    my $u = $contact_l{u};
    my $P = $contact_l{P};
    xmgrace::DrawCircle($u,$P, 'green',1);
    $u = $contact_r{u};
    $P = $contact_r{P};
    xmgrace::DrawCircle($u,$P, 'green',1);

    # mark states (left & right)
    $u = $left{u};
    $P = $left{P};
    xmgrace::DrawCircle($u,$P, 'blue',0);

    $u = $right{u};
    $P = $right{P};
    xmgrace::DrawCircle($u,$P, 'red',0);
    #
    # Hugoniot locus for outgoing waves
    #
    my $command;
    my @dir   = qw(left right);
    my @color = qw(blue red);
    my %line  = ( 'shock'=>'solid', 'isentrope'=>'dashed' );
    my $n = 0;
    my $mat;
    for $mat ( $LeftMaterial, $RightMaterial )
    {
        $command = "$::Run Wave file " . ${$mat->{DataFile}};
        $command .= " material " . $mat->{Material};
        my $units = ${$mat->{units}};
        $command .= " units hydro::$units" if $units;
        $command .= ' ' . $mat->{InitState}->EvaluateState('0', qw(V e u) );
        $command .= " $type[$n] $dir[$n] nsteps $::nsteps";
        my ($r1, $r2);
        $r1 = ($n==0) ? $left{P} : $right{P};
        if( $type[$n] eq 'shock' )
        {
            $r2 = $ymax;
            $xmgrace::linestyle = 1;        # solid
        }
        else
        {
            $r2 = $ymin;
            $xmgrace::linestyle = 4;        # dashed
        }
        $command .= " P range $r1 $r2";
        goto error unless open(WAVE, "$command |");

        &skip_lines(6); # skip header
        &xmgrace::NewLine("",$color[$n]);
          my $line;
          while( $line = <WAVE> )
          {
              $line =~ s/^\s*//;
              my ($V, $e, $u, $P) = split(/\s+/,$line);
              print xmgrace::PLOT "$u $P\n";
          }
          close(WAVE);
        &xmgrace::EndLine();    

        $n++;
    }
    #
    # Hugoniot locus for incoming waves
    #
    $xmgrace::linestyle = 2;        # dotted
    $n = 0;
    for $mat ( $LeftMaterial, $RightMaterial )
    {
        next unless $mat->{init_state} eq 'InitShock';
        $command = "$::Run Wave file " . ${$mat->{DataFile}};
        $command .= " material " . $mat->{Material};
        my $units = ${$mat->{units}};
        $command .= " units hydro::$units" if $units;
        my $RefState = $mat->{RefState};
        $command .= ' ' . $RefState->EvaluateState('0', qw(V e u) );
        my $dir = $mat->{init_state_dir};
        $dir =~ s/init_//;
        $command .= " shock $dir nsteps $::nsteps";
        my $r1 = $RefState->{P}{value}*(1+1e-5);
        $command .= " P range $r1 $ymax";
        goto error unless open(WAVE, "$command|");
        
        $u = $RefState->{u}->{value};
        $P = $RefState->{P}->{value};
        xmgrace::DrawCircle($u,$P,"black",1);
        &skip_lines(6); # skip header
        my $color = ($n==0) ? 'blue' : 'red';
        &xmgrace::NewLine("",$color);
           my $line;
           while( $line = <WAVE> )
           {
               $line =~ s/^\s*//;
               my ($V, $e, $u, $P) = split(/\s+/,$line);
               print xmgrace::PLOT "$u $P\n";
           }
           close(WAVE);
        &xmgrace::EndLine();
        ++$n;

        goto error if( $? != 0 );
    }
print xmgrace::PLOT<<EOF;
@ autoticks
EOF

    close(xmgrace::PLOT);
	system("$::XMGRACE $base.agr&");

    $::OldStatus = $::status_message = "plot file: $base.agr";
    $riemann->{PlotFile} = $base;
    $riemann->{FileLabel}->configure(-foreground=>$::foreground);
    $riemann->{FileEntry}->configure(
                    -takefocus=>1, -state=>'normal',
                -highlightcolor => $::highlightcolor,
                -foreground => $::foreground,
        );    
    return;
error : 
    print "\nERROR with command\n$command\n";    
    close(xmgrace::PLOT);
    close(WAVE);
    ::ErrorMessage(
        "Failed computing wave locii for Riemann problem",
        "Error executing: $command",
        "Reset states for Riemann Problem"
        );
}

1;
