functor TriangleCount
  (IC:
   sig
     val intersection_count: ('a * 'a -> order) -> ('a Seq.t * 'a Seq.t) -> int
   end):
sig
  val triangle_count: Graph.t -> int * int
end =
struct
  (* Linear-time intersection count function for sorted sequences *)
  fun linear_intersection_count (a1: int Seq.t, a2: int Seq.t) : int =
    let
      fun merge (i, j, count) =
        if i >= Seq.length a1 orelse j >= Seq.length a2 then
          count
        else
          case Int.compare(Seq.nth a1 i, Seq.nth a2 j) of
            LESS => merge(i+1, j, count)
          | GREATER => merge(i, j+1, count)
          | EQUAL => merge(i+1, j+1, count+1)
    in
      merge(0, 0, 0)
    end

  (*
    pointed_for_vertex (g, u):
    Count triangles where u points to both v and w, and v points to w.
    
    Since neighbor lists are already sorted with no duplicates or self-loops,
    we can use a linear-time merge algorithm for efficient intersection.
  *)
  fun pointed_for_vertex (g: Graph.t, u: Graph.vertex) : int =
    let
      val out_u = Graph.out_neighbors(g, u)
      
      fun count_for_v (acc, v) =
        let
          val out_v = Graph.out_neighbors(g, v)
        in
          acc + linear_intersection_count(out_u, out_v)
        end
    in
      Seq.foldl count_for_v 0 out_u
    end

  (*
    cyclic_for_vertex (g, u):
    Count triangles where u→v, v→w, w→u forms a cycle.
    Again using linear-time intersection since sequences are already sorted.
  *)
  fun cyclic_for_vertex (g: Graph.t, u: Graph.vertex) : int =
    let
      val out_u = Graph.out_neighbors(g, u)
      val in_u = Graph.in_neighbors(g, u)
      
      fun count_for_v (acc, v) =
        let
          val out_v = Graph.out_neighbors(g, v)
        in
          acc + linear_intersection_count(out_v, in_u)
        end
    in
      Seq.foldl count_for_v 0 out_u
    end

  (* 
    Parallel summation over vertices 
    To achieve optimal work and polylog span, we use parallel divide-and-conquer
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