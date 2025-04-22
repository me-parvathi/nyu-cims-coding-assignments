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
    Count triangles where u points to both v and w, and v points to w.
    We use the provided intersection_count function, taking advantage of the fact
    that neighbor lists are already sorted with no duplicates or self-loops.
  *)
  fun pointed_for_vertex (g: Graph.t, u: Graph.vertex) : int =
    let
      val out_u = Graph.out_neighbors(g, u)
      
      (* Calculate total count of triangles pointed at vertex u *)
      fun count_triangles_with_v (v) =
        intersection_count Int.compare (out_u, Graph.out_neighbors(g, v))
      
      (* Sum the triangle counts for all out-neighbors of u *)
      fun sum_over_neighbors (lo, hi) =
        if lo >= hi then 0
        else if hi - lo = 1 then 
          count_triangles_with_v (Seq.nth out_u lo)
        else
          let
            val mid = lo + (hi - lo) div 2
            val (left, right) = ForkJoin.par (
              fn () => sum_over_neighbors (lo, mid),
              fn () => sum_over_neighbors (mid, hi)
            )
          in
            left + right
          end
    in
      sum_over_neighbors (0, Seq.length out_u)
    end

  (*
    cyclic_for_vertex (g, u):
    Count triangles where u→v, v→w, w→u forms a cycle.
    We use the provided intersection_count function on sorted sequences.
  *)
  fun cyclic_for_vertex (g: Graph.t, u: Graph.vertex) : int =
    let
      val out_u = Graph.out_neighbors(g, u)
      val in_u = Graph.in_neighbors(g, u)
      
      (* Calculate cyclic triangles involving edge u→v *)
      fun count_triangles_with_v (v) =
        intersection_count Int.compare (Graph.out_neighbors(g, v), in_u)
      
      (* Sum the triangle counts for all out-neighbors of u *)
      fun sum_over_neighbors (lo, hi) =
        if lo >= hi then 0
        else if hi - lo = 1 then 
          count_triangles_with_v (Seq.nth out_u lo)
        else
          let
            val mid = lo + (hi - lo) div 2
            val (left, right) = ForkJoin.par (
              fn () => sum_over_neighbors (lo, mid),
              fn () => sum_over_neighbors (mid, hi)
            )
          in
            left + right
          end
    in
      sum_over_neighbors (0, Seq.length out_u)
    end

  (* 
    Parallel summation over vertices with divide-and-conquer 
    to achieve the desired polylog(N) span
  *)
  fun sum_pointed (g: Graph.t, lo: int, hi: int) : int =
    if lo >= hi then 0
    else if hi - lo = 1 then pointed_for_vertex(g, lo)
    else
      let
        val mid = lo + (hi - lo) div 2
        val (left, right) = ForkJoin.par (
          fn () => sum_pointed(g, lo, mid),
          fn () => sum_pointed(g, mid, hi)
        )
      in
        left + right
      end

  fun sum_cyclic (g: Graph.t, lo: int, hi: int) : int =
    if lo >= hi then 0
    else if hi - lo = 1 then cyclic_for_vertex(g, lo)
    else
      let
        val mid = lo + (hi - lo) div 2
        val (left, right) = ForkJoin.par (
          fn () => sum_cyclic(g, lo, mid),
          fn () => sum_cyclic(g, mid, hi)
        )
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