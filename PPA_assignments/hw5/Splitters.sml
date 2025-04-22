functor Splitters(JJ: JUST_JOIN):
sig
  val split_leq: JJ.tree * JJ.key -> JJ.tree
  val split_geq: JJ.tree * JJ.key -> JJ.tree
end =
struct

  open JJ
  structure TF = TreeFuncs(JJ)
  (* comparison function on keys *)
  val key_compare: key * key -> order = JJ.Data.cmp

  (* an empty tree *)
  val leaf: tree = join Leaf
  

  fun split_leq(t : tree, x : key) : tree =
    case expose t of
      Leaf => leaf  
    | Node(l, k', v', r) =>
        case Data.cmp(x, k') of
          LESS =>
            (* if k' > x, throw away this node;
             * only check the left subtree for smaller keys
             *)
            split_leq(l, x)

        | EQUAL =>
            (* keep this node; rebuild by joining once *)
            join (Node(l, k', v', leaf))

        | GREATER => 
            (* keep this node as well;
             * again exactly one call to join
             *)
            join (Node(l, k', v', split_leq(r, x)))

  

  fun split_geq(t : tree, x : key) : tree =
    case expose t of
      Leaf => leaf   (* no new calls to join; reuse global leaf *)

    | Node(l, k', v', r) =>
        (* compare x vs. k' *)
        case Data.cmp(x, k') of
            LESS =>
            (* x < k', so k' is >= x; we keep this node 
             * => exactly one call to join
             *)
              join(Node(split_geq(l, x), k', v', r))

          | EQUAL =>
            (* k' == x; again we keep the node => 1 call to join *)
              join(Node(split_geq(l, x), k', v', r))

          | GREATER =>
            (* x > k', so k' < x => discard this node
             * => 0 calls to join here
             *)
              split_geq(r, x)

end
