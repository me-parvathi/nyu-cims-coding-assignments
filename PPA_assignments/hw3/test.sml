(* Add more if you'd like! *)
val test_filenames =
  [ "test-json/0.json"
  , "test-json/1.json"
  , "test-json/2.json"
  , "test-json/3.json"
  , "test-json/4.json"
  , "test-json/5.json"
  , "test-json/6.json"
  ]


(* ======================================================================== *)


datatype 'a result =
  Okay of 'a
| Raised of exn

fun result f =
  Okay (f ())
  handle exn => Raised exn

fun ostr (a, b, c) =
  "(" ^ Int.toString a ^ "," ^ Int.toString b ^ "," ^ Int.toString c ^ ")"

fun leftmost (x, y) =
  if Option.isSome x then x else y
fun diff eq (x, y) =
  Parallel.reduce leftmost NONE (0, Int.min (Seq.length x, Seq.length y))
    (fn i => if eq (Seq.nth x i, Seq.nth y i) then NONE else SOME i)

fun btos true = "true"
  | btos false = "false"

fun test_flags (i, n) filename =
  let
    val contents = ReadFile.contentsSeq filename
    val contents_copy = Seq.map (fn c => c) contents
  in
    case result (fn () => JsonFindStrings.in_string_flags contents) of
      Raised exn =>
        print
          ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
           ^ ": Failed: raised exception: " ^ exnMessage exn ^ "\n")
    | Okay flags =>
        let
          val expected_flags = ReferenceSolutions.in_string_flags contents_copy
        in
          if Seq.length expected_flags <> Seq.length flags then
            print
              ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
               ^ ": Failed: output is length " ^ Int.toString (Seq.length flags)
               ^ " but should be length "
               ^ Int.toString (Seq.length expected_flags) ^ "\n")
          else
            case diff op= (flags, expected_flags) of
              NONE =>
                print
                  ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
                   ^ ": Passed\n")
            | SOME j =>
                print
                  ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
                   ^ ": Failed: expected value "
                   ^ btos (Seq.nth expected_flags j) ^ " at output index "
                   ^ Int.toString j ^ " but got " ^ btos (Seq.nth flags j)
                   ^ "\n")
        end
  end
  handle exn =>
    print
      ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
       ^ ": Unexpected error on input filename " ^ filename ^ ": "
       ^ exnMessage exn ^ "\n")


fun test_depths (i, n) filename =
  let
    val contents = ReadFile.contentsSeq filename
    val contents_copy = Seq.map (fn c => c) contents
    val correct_flags = ReferenceSolutions.in_string_flags contents
  in
    case
      result (fn () => JsonNesting.bracket_depths contents_copy correct_flags)
    of
      Raised exn =>
        print
          ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
           ^ ": Failed: raised exception: " ^ exnMessage exn ^ "\n")
    | Okay depths =>
        let
          val expected_depths = ReferenceSolutions.bracket_depths contents
        in
          if Seq.length expected_depths <> Seq.length depths then
            print
              ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
               ^ ": Failed: output is length "
               ^ Int.toString (Seq.length depths) ^ " but should be length "
               ^ Int.toString (Seq.length expected_depths) ^ "\n")
          else
            case diff op= (depths, expected_depths) of
              NONE =>
                print
                  ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
                   ^ ": Passed\n")
            | SOME j =>
                print
                  ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
                   ^ ": Failed: expected value "
                   ^ ostr (Seq.nth expected_depths j) ^ " at output index "
                   ^ Int.toString j ^ " but got " ^ ostr (Seq.nth depths j)
                   ^ "\n")
        end
  end
  handle exn =>
    print
      ("Test " ^ Int.toString i ^ "/" ^ Int.toString n
       ^ ": Unexpected error on input filename " ^ filename ^ ": "
       ^ exnMessage exn ^ "\n")


val () = print ("======== Testing JsonFindStrings.in_string_flags ========\n")
val num_tests = List.length test_filenames
val _ =
  List.foldl (fn (test, i) => (test_flags (i, num_tests) test; i + 1)) 1
    test_filenames

val () = print ("======== Testing JsonNesting.bracket_depths ========\n")
val _ =
  List.foldl (fn (test, i) => (test_depths (i, num_tests) test; i + 1)) 1
    test_filenames
