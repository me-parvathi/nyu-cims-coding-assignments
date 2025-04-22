(* Each input
 *   (keys, k)
 * will be tested by building a tree from the keys and then running your code:
 *   Splitters.split_leq (tree, k)
 *   Splitters.split_geq (tree, k)
 * and checking that these return the correct set of keys.
 *
 * The input sequences don't have to be sorted. The only requirement is
 * that there are no duplicate keys.
 *
 * Feel free to add more tests as you like!
 *)
type key = int
type test = (key Seq.t * key)
val tests: test list =
  [ (Seq.fromList [0], 1)
  , (Seq.fromList [1, 2, 3, 4, 5, 6, 7], 3)
  , (Seq.tabulate (fn i => i) 100, 20)
  , (Seq.tabulate (fn i => 2 * i + 1) 50, 98)
  ]


(* ========================================================================= *)

fun refsol (keys, k) =
  let
    val leqs = Seq.filter (fn x => x <= k) keys
    val geqs = Seq.filter (fn x => x >= k) keys
  in
    (leqs, geqs)
  end

(* For the purpose of testing *)
structure D: TREE_DATA =
struct
  type key = int
  val cmp = Int.compare
  type value = unit
  type rvalue = unit
  type avalue = unit
  fun f _ = ()
  fun g _ = ()
  val z = ()
end

structure AVL = AVL(D)
structure S = Splitters(AVL)
structure F = TreeFuncs(AVL)

fun make i j =
  List.foldl (fn (k, t) => AVL.join (AVL.Node (t, k, (), AVL.join AVL.Leaf)))
    (AVL.join AVL.Leaf) (List.tabulate (j - i, fn k => i + k))

fun from_keys s =
  F.from_seq (Seq.map (fn k => (k, ())) s)

fun inorder t =
  case AVL.expose t of
    AVL.Leaf => Seq.fromList []
  | AVL.Node (l, k, v, r) =>
      let val (l, r) = ForkJoin.par (fn () => inorder l, fn () => inorder r)
      in Seq.append (Seq.append (l, Seq.singleton k), r)
      end

datatype 'a result = Okay of 'a | Raised of exn

fun result f =
  Okay (f ())
  handle exn => Raised exn

fun leftmost (x, y) =
  if Option.isSome x then x else y
fun diff eq (x, y) =
  Parallel.reduce leftmost NONE (0, Int.min (Seq.length x, Seq.length y))
    (fn i => if eq (Seq.nth x i, Seq.nth y i) then NONE else SOME i)


fun output (i, n) msg expected result =
  case result of
    Raised exn =>
      print
        ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ " (" ^ msg ^ ")"
         ^ ": Failed: raised exception: " ^ exnMessage exn ^ "\n")
  | Okay out =>
      let
        val out = inorder out
      in
        if Seq.length out <> Seq.length expected then
          print
            ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ " (" ^ msg ^ ")"
             ^ ": Failed: output has size " ^ Int.toString (Seq.length out)
             ^ " but should be size " ^ Int.toString (Seq.length expected)
             ^ "\n")
        else
          case diff op= (out, expected) of
            NONE =>
              print
                ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ " (" ^ msg
                 ^ ")" ^ ": Passed\n")
          | SOME j =>
              print
                ("Test " ^ Int.toString i ^ "/" ^ Int.toString n ^ " (" ^ msg
                 ^ ")" ^ ": Failed: expected key "
                 ^ Int.toString (Seq.nth expected j) ^ " at output index "
                 ^ Int.toString j ^ " (of the in-order traversal) but got key "
                 ^ Int.toString (Seq.nth out j) ^ "\n")
      end


fun test_splitters (i, n) (keys, k) =
  let
    val tree = from_keys keys
    val (expected_leqs, expected_geqs) = refsol (keys, k)
    val leqs = result (fn () => S.split_leq (tree, k))
    val geqs = result (fn () => S.split_geq (tree, k))
  in
    output (i, n) "split_leq" expected_leqs leqs;
    output (i, n) "split_geq" expected_geqs geqs
  end


val num_tests = List.length tests
val _ =
  List.foldl (fn (test, i) => (test_splitters (i, num_tests) test; i + 1)) 1
    tests
