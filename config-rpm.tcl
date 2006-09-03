#!/usr/bin/wish
cd [file dirname [info script]]

set PACKAGES {qt-devel qt3-devel qt4-devel tk tcl}
## Examine packages to locate the installed Qt/Tk packages
foreach package $PACKAGES {
  puts "Examining RPM package: \"$package\"..."
  if {![catch {exec rpmquery $package} result]} {
    puts "    Found installed package: $result"
    set Package($package) $result
    ## Query all files to find location...
    foreach file [exec rpmquery $package -l] {
      if {[string match *qt*/bin/moc $file]} {
        set Location($package) [file dirname [file dirname $file]]
        break
      }
      if {[string match */lib/lib*.so* $file]} {
        set Location($package) [file dirname [file dirname $file]]
      }
    }
    puts "    Install Location: \"$Location($package)\""
    ## Query package information for finding version...
    set info [exec rpmquery $package -i]
    set Version($package) [lindex [regexp -inline {Version\s*:\s*([^\s]+)} \
                                          $info] end]
  }
};
# parray Package
# parray Location
# parray Version

package require Tk
wm title . "TileQt Configure Caller!"
grid [label .message -text "Utility for configuring TileQt!" \
        -font {arial 12 bold}] \
  -sticky snew -padx 10 -pady 4 -columnspan 3

## Create a table where the user can select for which package version tileqt
## should be build.
set column 0
foreach {pattern name} {qt* Qt tcl* Tcl tk* Tk} {
  set row 1
  foreach key [lsort [array names Package $pattern]] {
    if {![info exists L_$name]} {set L_$name $Location($key)}
    grid [radiobutton .$key -variable L_$name -value $Location($key) \
          -text "$name $Version($key)"] \
      -row $row -column $column -sticky snew -padx 2 -pady 2
    incr row
  }
  incr column
}

proc configure {} {
  set ::env(QTDIR) $::L_Qt
  set configure configure
  ## Run 'moc' to get Qt version...
  catch {exec $::L_Qt/bin/moc -v} result
  if {[string match {*(Qt 4.*)} $result]} {
    set configure configure-qt4
  }
  puts "/bin/sh $configure --prefix=$::L_Tk --with-tcl=$::L_Tcl/lib \
                                      --with-tk=$::L_Tk/lib \
                                      --with-qt-dir=$::L_Qt"
  exec /bin/sh $configure --prefix=$::L_Tk --with-tcl=$::L_Tcl/lib \
                                      --with-tk=$::L_Tk/lib \
                                      --with-qt-dir=$::L_Qt >&@ stdout
}
proc make {} { if {[file exists Makefile]} { exec make >&@ stdout } }
proc install {} { if {[file exists Makefile]} { exec make install >&@ stdout } }
proc demo {} { if {[file exists demos/demo.tcl]} { exec $::L_Tk/bin/wish demos/demo.tcl >&@ stdout } }
proc clean {} { if {[file exists Makefile]} { exec make distclean >&@ stdout } }

## The buttons section...
grid [button .configure -text "Run Configure!" -command configure -width 15] \
     [button .make      -text "Run Make!"      -command make -width 15] \
     [button .install   -text "Install!"       -command install -width 15] \
     -sticky snew -padx 10 -pady 2
grid [button .clean -text "Clean"              -command clean -width 15] \
     [button .demo  -text "Run Demo!"          -command demo -width 15] \
     [button .exit      -text "Exit"           -command exit -width 15] \
     -sticky snew -padx 10 -pady 2
