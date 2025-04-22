functor TreeFuncs(JJ: JUST_JOIN):
sig
  type key = JJ.key
  type value = JJ.value
  type tree = JJ.tree

  val singleton: key * value -> tree
  val split: tree * key -> tree * (value option) * tree
  val insert: tree * key * value -> tree
  val lookup: tree * key -> value option

  val join2: tree * tree -> tree

  val from_seq: (key * value) Seq.t -> tree
end =
struct

  open JJ

  (* comparison function on keys *)
  val key_compare: key * key -> order = JJ.Data.cmp

  (* an empty tree *)
  val leaf: tree = join Leaf

  fun singleton (k, v) =
    join (Node (leaf, k, v, leaf))

  fun split (t, k) =
    case expose t of
      Leaf => (leaf, NONE, leaf)
    | Node (l, k', v', r) =>
        case key_compare (k, k') of
          EQUAL => (l, SOME v', r)
        | LESS =>
            let val (ll, b, lr) = split (l, k)
            in (ll, b, join (Node (lr, k', v', r)))
            end
        | GREATER =>
            let val (rl, b, rr) = split (r, k)
            in (join (Node (l, k', v', rl)), b, rr)
            end


  fun split_last (t: tree) =
    case expose t of
      Leaf => raise Fail "TreeFuncs.split_last: bug: should be impossible"
    | Node (l, k, v, r) =>
        case expose r of
          Leaf => (l, (k, v))
        | _ =>
            let val (r', last) = split_last r
            in (join (Node (l, k, v, r')), last)
            end


  fun join2 (l, r) =
    case expose l of
      Leaf => r
    | _ => let val (l', (k, v)) = split_last l in join (Node (l', k, v, r)) end


  fun insert (t, k, v) =
    let val (l, _, r) = split (t, k)
    in join (Node (l, k, v, r))
    end


  fun lookup (t, k) =
    #2 (split (t, k))


  fun from_seq s =
    let
      val sorted_by_key =
        Mergesort.sort (fn (x, y) => key_compare (#1 x, #1 y)) s

      fun build i j =
        if i >= j then
          leaf
        else if i + 1 = j then
          singleton (Seq.nth sorted_by_key i)
        else
          let val m = i + (j - i) div 2
          in join2 (ForkJoin.par (fn () => build i m, fn () => build m j))
          end
    in
      build 0 (Seq.length s)
    end

end
