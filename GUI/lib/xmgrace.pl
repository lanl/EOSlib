package xmgrace;    # namespace

$main::XMGRACE = "xmgrace -free -geometry 792x612";

# plot range
$xlabel="";
$xmin=0;
$xmax=0;
$xmajor=0;      # major tics
$xminor=0;      # number of minor tics per major tic
$xskip =0;      # skip labels on major tics
#
$ylabel="";
$ymin=0;
$ymax=0;
$ymajor=0;
$yminor=0;
$yskip = 0;
#
$view   = '0.22, 0.20, 1.25, 0.88';
$title="";
$subtitle="";
$legend = "";
$legend_x = 0.65;
$legend_y = 0.79;
$legend_size = 1.4;
#
$g=0;               # graph number
$n=0;               # curve number
$Sn = "S0";
$color=0;
@color = qw( black brown violet orange magenta maroon );
$linewidth  = 2;
$linestyle  = 1;     # 1=solid, 4=dashed
#
$symbol     = 0;     # symbol type: 0=none, 1=circle
$sfill      = 1;     # symbol fill: 0=open, 1=solid 
$slinewidth = 1;     # symbol linewidth
$slinestyle = 1;     # symbol linestyle
$ssize      = 1;     # symbol size
#
# header2
$yformat = "decimal";
$yprec   = 1;
#
# Subroutines
#
sub header
{
    # draw ellipse parameters
    ($x1, $y1, $x2, $y2) = split(/,/,$view);
    $dx = 0.015*($xmax-$xmin) * ($y2-$y1)/($x2-$x1);
    $dy = 0.015*($ymax-$ymin);
    if( $g==0 )
{
    print PLOT <<EOF;
@ version 50120
@ page size 792,612
EOF
}
    print PLOT <<EOF;
@ g$g on
@ with g$g
@ view $view
@ world $xmin, $ymin, $xmax, $ymax
@ frame linewidth 1.5
#
@ title "$title"
@ title font "Helvetica"
@ title size 1.7
@ subtitle "$subtitle"
@ subtitle font "Helvetica"
@ subtitle size 1.4
#
@ default font "Helvetica"
@ xaxis  label char size 2.5
@ xaxis  label "$xlabel"
@ xaxis  label font "Helvetica"
@ xaxis  tick on
@ xaxis  tick major $xmajor
@ xaxis  tick minor ticks $xminor
@ xaxis  ticklabel skip $xskip
@ xaxis  ticklabel char size 2.0
@ xaxis  ticklabel font "Helvetica"
@ xaxis  ticklabel offset spec
@ xaxis  ticklabel offset 0.000 , 0.030
@    xaxis  tick major size 1.000000
@    xaxis  tick major color "cyan"
@    xaxis  tick major linewidth 1.5
@    xaxis  tick major linestyle 1
@    xaxis  tick major grid on
@    xaxis  tick minor color "cyan"
@    xaxis  tick minor linewidth 1.0
@    xaxis  tick minor linestyle 3
#
@ yaxis  label char size 2.5
@ yaxis  label "$ylabel"
@ yaxis  label font "Helvetica"
@ yaxis  tick on
@ yaxis  tick major $ymajor
@ yaxis  tick minor ticks $yminor
@ yaxis  ticklabel skip $yskip
@ yaxis  ticklabel char size 2.0
@ yaxis  ticklabel font "Helvetica"
@ yaxis  ticklabel offset spec
@ yaxis  ticklabel offset 0.000 , 0.030
@    yaxis  tick major color "cyan"
@    yaxis  tick major linewidth 1.5
@    yaxis  tick major linestyle 1
@    yaxis  tick major grid on
@    yaxis  tick minor color "cyan"
@    yaxis  tick minor linewidth 1.0
@    yaxis  tick minor linestyle 3
#
@ legend on
@ legend font "Helvetica"
@ legend $legend_x, $legend_y
@ legend box linewidth 1.0
@ legend box linestyle 1
@ legend char size $legend_size
@ legend length 6
#
#
EOF
    # defaults for sub NewLine
    #$g=0;               # graph
    $Sn = "S0";         # curve within graph
    $linestyle = 1;
    $symbol    = 0;
    $legend    = "";
    # updated by  sub EndLine
    $n     = 0;         # curve number of graph
    $Sn    = "S0";
    $ncolor = @color;
    $color = 0;         # counter for color
}

sub header2
{
print PLOT <<EOF;
@    yaxis  tick place normal
#
#
@ g1 on
@ with g1
@ view $view
@ world $xmin, $ymin, $xmax, $ymax
#
@ xaxis  bar off
@ xaxis  tick off
@ xaxis  ticklabel off
#
@ yaxis  label "$ylabel"
@ yaxis  label char size 2.5
@ yaxis  label font "Helvetica"
@ yaxis  tick on
@ yaxis  tick major $ymajor
@ yaxis  tick minor ticks $yminor
@ yaxis  ticklabel skip $yskip
@ yaxis  ticklabel char size 2.0
@ yaxis  ticklabel offset spec
@ yaxis  ticklabel offset 0.000 , 0.030
@ yaxis  ticklabel font "Helvetica"
@    yaxis  tick major size 1.0
@    yaxis  tick major linewidth 1.5
@    yaxis  tick major linestyle 1
@    yaxis  tick minor linewidth 1.0
@    yaxis  tick minor linestyle 1
@    yaxis  ticklabel format $yformat
@    yaxis  ticklabel prec $yprec
@    yaxis  label      color "red"
@    yaxis  tick major color "red"
@    yaxis  tick minor color "red"
@    yaxis  ticklabel  color "red"
@    yaxis  label place opposite
@    yaxis  ticklabel place opposite
@    yaxis  tick place opposite
@ legend on
@ legend font "Helvetica"
@ legend $legend_x, $legend_y
@ legend box linewidth 1.0
@ legend box linestyle 1
@ legend char size $legend_size
@ legend length 6
#
#
EOF
}

sub NewLine
{
    my ( $legend, $c ) = @_;
    $c = $color[$color % $ncolor] unless $c;
    $Sn = "S$n";
    print PLOT "@ with g$g\n";
    print PLOT "@ $Sn legend \"$legend\"\n"
        if defined($legend) && $legend ne "";
print PLOT <<EOF;
@ $Sn line linewidth $linewidth
@ $Sn line linestyle $linestyle
@ $Sn line color "$c"
@ $Sn symbol $symbol
@ $Sn symbol size $ssize
@ $Sn symbol color "$c"
@ $Sn symbol pattern 1
@ $Sn symbol fill color "$c"
@ $Sn symbol fill pattern $sfill
@ $Sn symbol linewidth $slinewidth
@ $Sn symbol linestyle $slinestyle

@ target G$g.$Sn
@ type xy
EOF
}
sub EndLine
{
    print PLOT "&\n";
    ++$n;
    ++$color;
}

sub DrawCircle
{
  my ($x,$y,$color,$fill) = @_;
  $fill = 1 if "$fill" eq "";
  # bounding box for ellipse
  $xleft  = $x - $dx;
  $xright = $x + $dx;
  $yleft  = $y - $dy;
  $yright = $y + $dy;

  print xmgrace::PLOT <<EOF;
@ with ellipse
@    ellipse on
@    ellipse loctype world
@    ellipse g0
@    ellipse $xleft, $yleft, $xright, $yright
@    ellipse linestyle 1
@    ellipse linewidth 2
@    ellipse color "$color"
@    ellipse fill color "$color"
@    ellipse fill pattern $fill
@ ellipse def
EOF
}
sub AddString
{
  my ($x,$y,$color,$str,$just,$size) = @_;
  $just = 6 if "$just" eq "";
  $size=1.2 if "$size" eq "";

  print xmgrace::PLOT <<EOF;
@ with string
@    string on
@    string loctype world
@    string g0
@    string $x, $y
@    string color "$color"
@    string rot 0
@    string font 4
@    string just $just
@    string char size $size
@    string def "$str"

EOF
}

1;
