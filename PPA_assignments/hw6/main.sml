val filename =
  List.hd (CommandLineArgs.positional ())
  handle _ => Util.die "Usage: ./main @mpl procs <P> -- <SNAP_FILENAME>"

val _ = print "Loading graph (if large, this might take a while...)\n"
val (graph, tm) = Util.getTime (fn _ => Graph.load_from_snap_file filename)
val _ = print ("Loaded graph in " ^ Time.fmt 4 tm ^ "s\n")

val _ = print ("num vertices " ^ Int.toString (Graph.num_vertices graph) ^ "\n")
val _ = print ("num edges    " ^ Int.toString (Graph.num_edges graph) ^ "\n")

structure TC = TriangleCount(IntersectionCount)

val _ = print "--------------------\n"
val (pointed, cyclic) = Benchmark.run (fn _ => TC.triangle_count graph)
val _ = print "--------------------\n"

val _ = print ("num pointed triangles " ^ Int.toString pointed ^ "\n")
val _ = print ("num cyclic triangles  " ^ Int.toString cyclic ^ "\n")
