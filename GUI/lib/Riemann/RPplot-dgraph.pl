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
    
    my $x_label = "u\\d\\s-4p\\s+4\\u $LeftState->{u}{unit}";
    my $y_label = "P $LeftState->{P}{unit}";

    my $xmin = $LeftMaterial->{x_min};
    my $xmax = $LeftMaterial->{x_max};
    my $xlimits = ($LeftMaterial->{x_var} eq 'u_p' and
                &::IsNum($xmin) and &::IsNum($xmax) ) ?
                ".limits x [ $xmin, $xmax ]" : "";
    
    my $ymin = $LeftMaterial->{y_min};
    my $ymax = $LeftMaterial->{y_max};
    my $ylimits = ($LeftMaterial->{y_var} eq 'P' and
                &::IsNum($ymin) and &::IsNum($ymax) ) ?
                ".limits y [ $ymin, $ymax ]" : "";
    
    $::plot++;
    my $base = $::tag . "_" . $::plot;
    open(OUT,"> $base.dg") || die "can't create plot file";
print OUT<<EOF;
#!/bin/sh
# ColorDgraph.pl skips next line \\
exec perl $::draw -f \$0 "\$@"

# Graphical solution of Riemann Problem
    
.view 0,0,1,1;
.pointsize 16;
.penwidth 2;
.ticlength 0.8;

.label x-axis "\\s-6$x_label"
.label y-axis "\\s-6$y_label"
.label screen 0.5 1.15 "Riemann Problem"
.label ".lb left"
.blue
.label screen 0 1.10 "\\ left material is $LeftMaterial->{Material}"
.red
.label screen 0 1.05 "right material is $RightMaterial->{Material}"
.black

$xlimits
$ylimits

EOF

    # mark solution
    my $u = $contact_l{u};
    my $P = $contact_l{P};
    if( (not $xlimits or ($xmin <= $u and $u <= $xmax))
        and (not $ylimits or ($ymin <= $P and $P <= $ymax)) )
    {
        print OUT ".green\n";
        print OUT ".points symbol circle\n";
        print OUT "$u $P\n";
    }
    $u = $contact_r{u};
    $P = $contact_r{P};
    if( (not $xlimits or ($xmin <= $u and $u <= $xmax))
        and (not $ylimits or ($ymin <= $P and $P <= $ymax)) )
    {
        print OUT "$u $P\n";
    }

    # mark states (left & right)
    print OUT ".black\n";
    print OUT ".points symbol solid_circle\n";
    $u = $left{u};
    $P = $left{P};
    if( (not $xlimits or ($xmin <= $u and $u <= $xmax))
        and (not $ylimits or ($ymin <= $P and $P <= $ymax)) )
    {
        print OUT "$u $P\n";
    }
    $u = $right{u};
    $P = $right{P};
    if( (not $xlimits or ($xmin <= $u and $u <= $xmax))
        and (not $ylimits or ($ymin <= $P and $P <= $ymax)) )
    {
        print OUT "$u $P\n";
    }

    # Hugoniot locus for outgoing waves
    my $command;
    my @dir   = qw(left right);
    my @color = qw(.blue .red);
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
        my ($range, $r1, $r2);
        if( $type[0] eq $type[1] )
        {
            $range = "u range";
            if( $dir[$n] eq 'left' )
            {
                $r1 = $left{u};
                $r2 = $xlimits ? ($type[$n] eq 'shock' ? $xmin : $xmax)
                               : $right{u};
            }
            else
            {
                $r1 = $right{u};
                $r2 = $xlimits ? ($type[$n] eq 'shock' ? $xmax : $xmin)
                               : $left{u};
            }        
            $command .= " valid_u" if $type[0] eq 'isentrope';
        }
        else
        {
            $range = "P range";
            if( $dir[$n] eq 'left' )
            {
                $r1 = $left{P};
                $r2 = $ylimits ? ($type[$n] eq 'shock' ? $ymax : $ymin)
                               : $right{P};
            }
            else
            {
                $r1 = $right{P};
                $r2 = $ylimits ? ($type[$n] eq 'shock' ? $ymax : $ymin)
                               : $left{P};
            }        
        }
        $command .= " $range $r1 $r2";
        goto error unless open(WAVE, "$command|");

        &skip_lines(4); # skip header
        print OUT "$color[$n]\n";
        print OUT ".line symbol ", $line{$type[$n]}, "\n";
        my $line;
        while( $line = <WAVE> )
        {
            $line =~ s/^\s*//;
            my ($V, $e, $u, $P) = split(/\s+/,$line);
            next if $xlimits and ($u < $xmin or $xmax < $u);
            next if $ylimits and ($P < $ymin or $ymax < $P);
            print OUT "$u $P\n";
        }
        close(WAVE);
        # Can fail when isentrope reaches escape velocity
        # goto error if( $? != 0);
        $n++;
    }

    # Hugoniot locus for incoming waves
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
        my ($range, $r1, $r2);
        if( $type[0] eq $type[1] )
        {
            $range = "u range";
            $r1 = $RefState->{u}{value};
            if( $dir eq 'left' )
            {   
                $r2 = $xlimits ? $xmin : $right{u};
            }
            else
            {
                $r2 = $xlimits ? $xmax : $left{u};
            }        
        }
        else
        {
            $range = "P range";
            $r1 = $RefState->{P}{value}*(1+1e-5);
            if( $dir eq 'left' )
            {
                $r2 = $ylimits ? $ymax : $right{P};
            }
            else
            {
                $r2 = $ylimits ? $ymax : $left{P};
            }        
        }
        $command .= " $range $r1 $r2";
        goto error unless open(WAVE, "$command|");
        
        print OUT ".black\n";
        $u = $RefState->{u}->{value};
        $P = $RefState->{P}->{value};
        if( (not $xlimits or ($xmin <= $u and $u <= $xmax))
            and (not $ylimits or ($ymin <= $P and $P <= $ymax)) )
        {
            print OUT ".points symbol circle\n";
            print OUT "$u $P\n";
        }
        print OUT ".line symbol solid", "\n";
        &skip_lines(4); # skip header        
        my $line;
        while( $line = <WAVE> )
        {
            $line =~ s/^\s*//;
            my ($V, $e, $u, $P) = split(/\s+/,$line);
            next if $xlimits and ($u < $xmin or $xmax < $u);
            next if $ylimits and ($P < $ymin or $ymax < $P);
            print OUT "$u $P\n";
        }
        close(WAVE);
        goto error if( $? != 0 );
    }

print OUT<<EOF;
.black    
.pointsize 16;
.penwidth 2;
EOF

    close(OUT);
    chmod 0777, "$base.dg";
    system("$base.dg gid $$ out $base.eps&");
    #system("$base.dg fifo $::FIFO out $base.eps&");
    #open(FIFO,"<$::FIFO") or die "can't open FIFO: $!";
    #push @::plotid, <FIFO>;
    #close(FIFO);


    $::OldStatus = $::status_message = "plot file: $base.eps";
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
    close(OUT);
    close(WAVE);
    ::ErrorMessage(
        "Failed computing wave locii for Riemann problem",
        "Error executing: $command",
        "Reset states for Riemann Problem"
        );
}

1;
