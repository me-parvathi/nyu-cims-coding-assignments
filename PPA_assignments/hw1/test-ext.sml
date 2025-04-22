
(* Add more if you'd like! *)
val tests =
  [ {input = 1, expected_output = (1, 2)}
  , {input = 2, expected_output = (0, 7)}
  , {input = 4, expected_output = (2, 61)}
  , {input = 5, expected_output = (10, 221)}
  ]

datatype 'a result = Okay of 'a | Raised of exn
fun result f =
  Okay (f ())
  handle exn => Raised exn

fun ostr (a, b) = "(" ^ Int.toString a ^ "," ^ Int.toString b ^ ")"

fun do_test (i, n) {input, expected_output} =
  case result (fn () => NQueensExt.count_solutions_and_boards input) of
    Raised exn => print ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ ": Failed: raised exception: " ^ exnMessage exn ^ "\n")
  | Okay output =>
      if output = expected_output then
        print ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ ": Passed\n")
      else
        print ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ ": Failed: expected output " ^ ostr expected_output ^ " but got " ^ ostr output ^ "\n")

val num_tests = List.length tests
val _ = List.foldl (fn (test, i) => (do_test (i, num_tests) test; i+1)) 1 tests