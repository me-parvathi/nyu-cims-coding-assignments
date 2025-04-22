val filename = List.hd (CommandLineArgs.positional ())
               handle _ => Util.die "Usage: ./main <FILE>\n"

val contents = ReadFile.contentsSeq filename

val (flags, depths) = Benchmark.run (fn _ =>
  let
    val flags = JsonFindStrings.in_string_flags contents
    val depths = JsonNesting.bracket_depths contents flags
  in
    (flags, depths)
  end)

(* ==========================================================================
 * Compare output against sequential reference solutions
 *)

fun seqs_equal eq (x, y) =
  Seq.length x = Seq.length y
  andalso
  Parallel.reduce (fn (a, b) => a andalso b) true (0, Seq.length x) (fn i =>
    eq (Seq.nth x i, Seq.nth y i))

val () = print ("checking in_string_flags for correctness...\n")
val expected_flags = ReferenceSolutions.in_string_flags contents
val () =
  if seqs_equal op= (flags, expected_flags) then
    print
      ("CORRECT: in_string_flags matches reference sequential implementation\n")
  else
    print
      ("INCORRECT: in_string_flags differ from reference sequential implementation\n")

val () = print ("checking bracket_depths for correctness...\n")
val expected_depths = ReferenceSolutions.bracket_depths contents
val () =
  if seqs_equal op= (depths, expected_depths) then
    print
      ("CORRECT: bracket_depths matches reference sequential implementation\n")
  else
    print
      ("INCORRECT: bracket_depths differs from reference sequential implementation\n")

fun dump d =
  Util.for (0, Seq.length d) (fn i =>
    let
      val (i, dc, ds) = Seq.nth d i
      val itos = Int.toString
    in
      print (itos i ^ " " ^ itos dc ^ " " ^ itos ds ^ "\n")
    end)

(* val () = print "\n"
val () = dump depths
val () = print "\n"
val () = dump expected_depths
val () = print "\n" *)


(* ==========================================================================
 * Compute rainbow bracket colors from output and print to terminal
 *)


(* We can simplify the rainbow bracket colors by just assigning a single depth
 * to every bracket -- the sum of its curly depth and square depth.
 *)
val depths_for_rainbow = Seq.map (fn (i, dc, ds) => (i, dc + ds)) depths

(* Now compute the rainbow bracket and print to terminal *)
val () = print "\n"
val () = TerminalColorString.print
  (RainbowBrackets.make contents depths_for_rainbow flags)
val () = print "\n"
