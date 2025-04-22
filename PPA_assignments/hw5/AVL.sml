functor AVL(Data: TREE_DATA) :> JUST_JOIN
                                where type key = Data.key
                                where type value = Data.value
                                where type avalue = Data.avalue =
struct

  structure Data = Data

  type key = Data.key
  type value = Data.value
  type avalue = Data.avalue

  datatype tree =
    Leaf_
  | Node_ of {l: tree, r: tree, k: key, v: value, av: avalue, h: int}

  datatype exposed = Leaf | Node of tree * key * value * tree


  fun expose Leaf_ = Leaf
    | expose (Node_ {l, r, k, v, ...}) = Node (l, k, v, r)


  fun aval Leaf_ = Data.z
    | aval (Node_ {av, ...}) = av

  fun h (Leaf_) = 0
    | h (Node_ {h = height, ...}) = height


  fun make_node (l, k, v, r) =
    Node_
      { l = l
      , k = k
      , v = v
      , r = r
      , h = 1 + Int.max (h l, h r)
      , av = Data.g (Data.g (aval l, Data.f (k, v)), aval r)
      }


  fun heights_okay Leaf_ = true
    | heights_okay (Node_ {l, r, ...}) =
        h l <= h r + 1 andalso h r <= h l + 1


  fun all_heights_okay Leaf_ = true
    | all_heights_okay (t as Node_ {l, r, ...}) =
        heights_okay t andalso all_heights_okay l andalso all_heights_okay r


  fun rotate_left Leaf_ = Leaf_
    | rotate_left (Node_ {l, k, v, r, ...}) =
        case r of
          Leaf_ => raise Fail "AugAVL.rotate_left failed"
        | Node_ {l = rl, k = k', v = v', r = rr, ...} =>
            make_node (make_node (l, k, v, rl), k', v', rr)


  fun rotate_right Leaf_ = Leaf_
    | rotate_right (Node_ {l, k, v, r, ...}) =
        case l of
          Leaf_ => raise Fail "AugAVL.rotate_right failed"
        | Node_ {l = ll, k = k', v = v', r = lr, ...} =>
            make_node (ll, k', v', make_node (lr, k, v, r))


  fun join_right (l, k, v, r) =
    case l of
      Leaf_ => raise Fail "AugAVL.join_right: bug: should be impossible"
    | Node_ {l = ll, k = k', v = v', r = lr, ...} =>
        if h lr <= h r + 1 then
          let
            val r' = make_node (lr, k, v, r)
          in
            if h r' <= h l + 1 then make_node (ll, k', v', r')
            else rotate_left (make_node (ll, k', v', rotate_right r'))
          end
        else
          let val t = make_node (ll, k', v', join_right (lr, k, v, r))
          in if heights_okay t then t else rotate_left t
          end


  fun join_left (l, k, v, r) =
    case r of
      Leaf_ => raise Fail "AugAVL.join_left: bug: should be impossible"
    | Node_ {l = rl, k = k', v = v', r = rr, ...} =>
        if h rl <= h l + 1 then
          let
            val l' = make_node (l, k, v, rl)
          in
            if h l' <= h r + 1 then make_node (l', k', v', rr)
            else rotate_right (make_node (rotate_left l', k', v', rr))
          end
        else
          let val t = make_node (join_left (l, k, v, rl), k', v', rr)
          in if heights_okay t then t else rotate_right t
          end


  fun join Leaf = Leaf_
    | join (Node (l, k, v, r)) =
        if h l > h r + 1 then join_right (l, k, v, r)
        else if h r > h l + 1 then join_left (l, k, v, r)
        else make_node (l, k, v, r)

end
