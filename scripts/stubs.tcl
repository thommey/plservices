set numeric [localclient_create "tcl" "tickle" "newserv.sucks" "+i" "tcl" "tcl" "TCL Woohoo!"]
puts "Numeric: $numeric"
localclient_join $numeric #labspace 0
irc_privmsg $numeric #labspace "\001ACTION does the harlem shake\001"
