(* feel free to add more! *)
val test_intersection = List.map (fn (s, t) => (Seq.fromList s, Seq.fromList t))
  [ ([1, 3, 5, 10, 11, 12], [5, 6, 7, 8, 9, 10])
  , ([1, 2, 3], [1, 2, 3])
  , ([1, 2, 3], [4, 5, 6, 7])
  , (List.tabulate (100, fn i => i), List.tabulate (33, fn i => 3 * i))
  ]

(* feel free to add more! *)
val test_graphs =
  [ "test-graph/g1.txt"
  , "test-graph/g2.txt"
  , "test-graph/g3.txt"
  , "test-graph/g4.txt"
  , "test-graph/g5.txt"
  , "test-graph/g6.txt"
  , "test-graph/g7.txt"
  ]


(* ======================================================================= *)

structure IC = IntersectionCount
structure TC = TriangleCount(ReferenceSolutions)

(* ======================================================================= *)


datatype 'a result =
  Okay of 'a
| Raised of exn

fun result f =
  Okay (f ())
  handle exn => Raised exn

fun output (i, n) msg eq tos expected result =
  case result of
    Raised exn =>
      print
        ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ " (" ^ msg ^ ")"
         ^ ": Failed: raised exception: " ^ exnMessage exn ^ "\n")
  | Okay out =>
      if eq (expected, out) then
        print
          ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ " (" ^ msg ^ ")"
           ^ ": Passed\n")
      else
        print
          ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ " (" ^ msg ^ ")"
           ^ ": Failed: expected " ^ tos expected ^ " but got " ^ tos out ^ "\n")


fun test_ic (i, n) (s, t) =
  let
    val expected = ReferenceSolutions.intersection_count Int.compare (s, t)
  in
    output (i, n) "intersection_count" op= Int.toString expected
      (result (fn () => IC.intersection_count Int.compare (s, t)))
  end

fun test_tc (i, n) filename =
  let
    val g = Graph.load_from_snap_file filename
    val expected = ReferenceSolutions.triangle_count g
  in
    output (i, n) "triangle_count" op=
      (fn (a, b) => "(" ^ Int.toString a ^ "," ^ Int.toString b ^ ")") expected
      (result (fn () => TC.triangle_count g))
  end

val num_tests = List.length test_intersection
val _ =
  List.foldl (fn (test, i) => (test_ic (i, num_tests) test; i + 1)) 1
    test_intersection


val num_tests = List.length test_graphs
val _ =
  List.foldl (fn (test, i) => (test_tc (i, num_tests) test; i + 1)) 1
    test_graphs
