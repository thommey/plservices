set numeric [client_create "tcl" "tickle" "newserv.sucks" "+i" "tcl" "tcl" "TCL Woohoo!"]
puts "Numeric: $numeric"
client_join $numeric #labspace 0
client_privmsg $numeric #labspace "\001ACTION does the harlem shake\001"
