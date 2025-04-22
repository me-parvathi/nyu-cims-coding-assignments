val n = CommandLineArgs.parseInt "n" 13
val doExt = CommandLineArgs.parseFlag "ext"

val _ =
  if not doExt then
    let val nsol = Benchmark.run (fn _ => NQueens.count_solutions n)
    in print ("number of solutions: " ^ Int.toString nsol ^ "\n")
    end
  else
    let
      val (nsol, nboards) = Benchmark.run (fn _ =>
        NQueensExt.count_solutions_and_boards n)
    in
      print ("number of solutions: " ^ Int.toString nsol ^ "\n");
      print ("number of boards:    " ^ Int.toString nboards ^ "\n")
    end
