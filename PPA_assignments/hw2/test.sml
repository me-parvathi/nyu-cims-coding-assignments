(* Add more if you'd like! *)
val tests =
  [ {input = "", expected_output = (1, 0)}
  , {input = "\n", expected_output = (1, 0)}
  , {input = "12345678", expected_output = (1, 8)}
  , {input = "12345\n", expected_output = (1, 5)}
  , {input = "\n12345", expected_output = (2, 5)}
  , {input = "\n1\n12\n123\n\n12345\n\n12\n12345\n\n", expected_output = (6, 5)}
  ]


(* ======================================================================== *)


fun input_from_string s =
  Seq.tabulate (fn i => String.sub (s, i)) (String.size s)

val tests =
  List.map
    (fn x =>
       { input = input_from_string (#input x)
       , expected_output = #expected_output x
       }) tests

datatype 'a result = Okay of 'a | Raised of exn
fun result f =
  Okay (f ())
  handle exn => Raised exn

fun ostr (a, b) =
  "(" ^ Int.toString a ^ "," ^ Int.toString b ^ ")"


fun do_test (i, n) {input, expected_output} =
  let
    val (expected_line, expected_len) = expected_output
  in
    case result (fn () => LongestLine.longest_line input) of
      Raised exn =>
        print
          ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
           ^ ": Failed: raised exception: " ^ exnMessage exn ^ "\n")
    | Okay (output as (line, len)) =>
        if line = expected_line andalso len = expected_len then
          print ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ ": Passed\n")
        else if len = expected_len then
          print
            ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
             ^ ": Failed: correct length, but incorrect line number."
             ^ " Expected output " ^ ostr expected_output ^ " but got "
             ^ ostr output ^ "\n")
        else if line = expected_line then
          print
            ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
             ^ ": Failed: correct line number, but incorrect length."
             ^ " Expected output " ^ ostr expected_output ^ " but got "
             ^ ostr output ^ "\n")
        else
          print
            ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
             ^ ": Failed: incorrect line number and length."
             ^ " Expected output " ^ ostr expected_output ^ " but got "
             ^ ostr output ^ "\n")
  end

val num_tests = List.length tests
val _ =
  List.foldl (fn (test, i) => (do_test (i, num_tests) test; i + 1)) 1 tests
