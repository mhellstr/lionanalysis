    # line styles for ColorBrewer RdYlBu
    # for use with divering data
    # provides 8 colors with red low, pale yellow middle, and blue high
    # compatible with gnuplot >=4.2
    # author: Anna Schneider

    # line styles
    set style line 1 lt 1 lc rgb '#D73027' # red
    set style line 2 lt 1 lc rgb '#F46D43' # orange
    set style line 3 lt 1 lc rgb '#FDAE61' # 
    set style line 4 lt 1 lc rgb '#FEE090' # pale orange
    set style line 5 lt 1 lc rgb '#E0F3F8' # pale blue
    set style line 6 lt 1 lc rgb '#ABD9E9' # 
    set style line 7 lt 1 lc rgb '#74ADD1' # medium blue
    set style line 8 lt 1 lc rgb '#4575B4' # blue

    # palette
    set palette defined ( 0 '#D73027',                          1 '#F46D43',                          2 '#FDAE61',                          3 '#FEE090',                          4 '#E0F3F8',                          5 '#ABD9E9',                          6 '#74ADD1',                          7 '#4575B4' )

    minx1 = 1.45; maxx1 = 3.45; # #bond(1-o-3-h)
    miny2 = 0.5; maxy2 = 82.5; # #angle(1-o-2-o_h-3-h)
    mytitle=''
    filexy='ardf.txt_7'; filex='ardf.txt_8'; filey='ardf.txt_6';
    set multiplot title mytitle

    bigwidth=0.6
    bigx=0.3
    smallwidth=0.2
    LW=3

    set lmargin 0
    set rmargin 0
    set tmargin 0
    set bmargin 0
    set origin bigx, bigx
    set size bigwidth, bigwidth

    unset colorbox
    unset ytic
    unset xtic
    set xrange [minx1:maxx1]
    set yrange [miny2:maxy2]
    plot filexy u 1:2:3 w image notitle


    set origin bigx, bigx-smallwidth
    set size bigwidth, smallwidth
    unset ytic
    set xtic
    set xrange [minx1:maxx1]
    unset yrange
    set xlabel '#bond(1-o-3-h)'
    unset ylabel
    plot filex u 1:2 w l lw LW notitle


    set origin bigx-smallwidth, bigx
    set size smallwidth, bigwidth
    set ytic
    unset xtic
    unset xrange
    set yrange [miny2:maxy2]
    unset xlabel
    set ylabel '#angle(1-o-2-o_h-3-h)' offset 1,0
    plot filey u 2:1 w l lw LW notitle

    unset xlabel
    unset ylabel
    unset multiplot
