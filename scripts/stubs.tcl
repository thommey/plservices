set numeric [localclient_create "tcl" "tickle" "newserv.sucks" "+i" "tcl" "tcl" "TCL Woohoo!"]
puts "Numeric: $numeric"
localclient_join $numeric #labspace3 0
