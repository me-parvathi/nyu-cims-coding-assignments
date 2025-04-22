val filename = List.hd (CommandLineArgs.positional ())
               handle _ => Util.die "Usage: ./main <FILE>\n"

val contents = ReadFile.contentsSeq filename

val (i, len) = Benchmark.run (fn _ => LongestLine.longest_line contents)

val _ = print ("longest line: " ^ Int.toString i ^ "\n")
val _ = print ("length:       " ^ Int.toString len ^ "\n")
