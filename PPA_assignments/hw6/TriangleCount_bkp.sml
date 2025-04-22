functor TriangleCount
  (IC:
   sig
     val intersection_count: ('a * 'a -> order) -> ('a Seq.t * 'a Seq.t) -> int
   end):
sig
  val triangle_count: Graph.t -> int * int
end =
struct
  (* Use the provided intersection_count *)
  val intersection_count = IC.intersection_count

  (*
    pointed_for_vertex (g, u):
      For a vertex u, for each out‐neighbor v (i.e. for each edge u→v),
      we count the number of w such that w is in the intersection of 
      out_neighbors(g, u) and out_neighbors(g, v). 
      Thus we “charge” the pointed triangle (u, v, w) (which has edges u→v, u→w, and v→w)
      exactly once.
  *)
  fun pointed_for_vertex (g: Graph.t, u: Graph.vertex) : int =
    let
      val out_u = Graph.out_neighbors(g, u)
      fun count_for_v (acc, v) =
          acc + intersection_count (Int.compare) (out_u, Graph.out_neighbors(g, v))
    in
      Seq.foldl count_for_v 0 out_u
    end

  (*
    cyclic_for_vertex (g, u):
      For a vertex u, for each out‐neighbor v (edge u→v),
      we count the number of w in the intersection of out_neighbors(g, v) and in_neighbors(g, u).
      This finds cyclic triangles (u,v,w) with edges u→v, v→w, and w→u.
      Since each cyclic triangle is “charged” on each of its three edges, we will divide by 3 later.
  *)
  fun cyclic_for_vertex (g: Graph.t, u: Graph.vertex) : int =
    let
      val out_u = Graph.out_neighbors(g, u)
      fun count_for_v (acc, v) =
          acc + intersection_count (Int.compare) (Graph.out_neighbors(g, v), Graph.in_neighbors(g, u))
    in
      Seq.foldl count_for_v 0 out_u
    end

  (*
    To achieve the desired O(polylog(n)) span, we use a parallel divide‐and‐conquer
    reduction over the vertices. The functions below sum over the vertices in the range [lo, hi).
  *)

  fun sum_pointed (g: Graph.t, lo: int, hi: int) : int =
    if lo >= hi then 0
    else if hi - lo = 1 then pointed_for_vertex(g, lo)
    else
      let
        val mid = lo + (hi - lo) div 2
        val (left, right) = ForkJoin.par (fn () => sum_pointed(g, lo, mid),
                                           fn () => sum_pointed(g, mid, hi))
      in
        left + right
      end

  fun sum_cyclic (g: Graph.t, lo: int, hi: int) : int =
    if lo >= hi then 0
    else if hi - lo = 1 then cyclic_for_vertex(g, lo)
    else
      let
        val mid = lo + (hi - lo) div 2
        val (left, right) = ForkJoin.par (fn () => sum_cyclic(g, lo, mid),
                                           fn () => sum_cyclic(g, mid, hi))
      in
        left + right
      end

  fun triangle_count (g: Graph.t) : int * int =
    let
      val n = Graph.num_vertices(g)
      val pointed_total = sum_pointed(g, 0, n)
      val cyclic_total = sum_cyclic(g, 0, n)
    in
      (pointed_total, cyclic_total div 3)
    end

end
