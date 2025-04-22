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
   Goal: The span needs to remain polylog
   1. At each vertex I need to calculate the numer of possible triangles. 
      1.1 To achive each vertex in a graph, 
      I keep dividing like in hw1 to get  the actual summation at each vertx
  2. At each vertex:
      2.1 I need to calculate if it is a part of a triangle.  (either pointed or cyclic)
        2.1.1 Get the neighbours for sure and that is O(1)
        2.1.2 Now what do I do with these neighbours
  3. For each neighbour of the vertex - make an intersection with the out neighbours for pointed 
  and maybe idk both in  for cyclic. 

  *)
  fun pointed_triangles_at_u (g: Graph.t, u: Graph.vertex) : int =
    let
      val out_u = Graph.out_neighbors(g, u)
      

      fun count_triangles_with_v (v) =
        intersection_count Int.compare (out_u, Graph.out_neighbors(g, v))
      

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

  
  fun cyclic_triangles_at_u (g: Graph.t, u: Graph.vertex) : int =
    let
      val out_u = Graph.out_neighbors(g, u)
      val in_u = Graph.in_neighbors(g, u)
      

      fun count_triangles_with_v (v) =
        intersection_count Int.compare (Graph.out_neighbors(g, v), in_u)
      
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

  
  fun sum_over_pointed (g: Graph.t, lo: int, hi: int) : int =
    if lo >= hi then 0
    else if hi - lo = 1 then pointed_triangles_at_u(g, lo)
    else
      let
        val mid = lo + (hi - lo) div 2
        val (left, right) = ForkJoin.par (
          fn () => sum_over_pointed(g, lo, mid),
          fn () => sum_over_pointed(g, mid, hi)
        )
      in
        left + right
      end

  fun sum_over_cyclic (g: Graph.t, lo: int, hi: int) : int =
    if lo >= hi then 0
    else if hi - lo = 1 then cyclic_triangles_at_u(g, lo)
    else
      let
        val mid = lo + (hi - lo) div 2
        val (left, right) = ForkJoin.par (
          fn () => sum_over_cyclic(g, lo, mid),
          fn () => sum_over_cyclic(g, mid, hi)
        )
      in
        left + right
      end

  fun triangle_count (g: Graph.t) : int * int =
    let
      val n = Graph.num_vertices(g)
      (* Sum over the pointed triangles *)
      val pointed_total = sum_over_pointed(g, 0, n)
      (* Sum over the cyclic triangles. and finally divide by three - fom the ref soln *)
      val cyclic_total = sum_over_cyclic(g, 0, n)
    in
      (pointed_total, cyclic_total div 3)
    end

end