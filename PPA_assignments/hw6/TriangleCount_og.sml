functor TriangleCount
  (IC:
   sig
     val intersection_count: ('a * 'a -> order) -> ('a Seq.t * 'a Seq.t) -> int
   end):
sig
  val triangle_count: Graph.t -> int * int
end =
struct

  (* You can use this function; assume it is a correct implementation of
   * the first task.
   *)
  val intersection_count = IC.intersection_count


  (* The Graph type is defined in lib-local/Graph.sml. Copy/pasted here
   * for reference:
   * 
   * structure Graph:
   * sig
   *   type t
   *   type graph = t
   *   type vertex = int
   *   val in_degree: graph * vertex -> int
   *   val out_degree: graph * vertex -> int
   *   val in_neighbors: graph * vertex -> vertex Seq.t
   *   val out_neighbors: graph * vertex -> vertex Seq.t
   *   val num_vertices: graph -> int
   *   val num_edges: graph -> int
   *   ...
   * end
   *
   * All of these Graph.XXX functions require O(1) work and span. You may
   * assume that the output of the `in_neighbors` and `out_neighbors`
   * functions are sorted.
   *)


  fun triangle_count (g: Graph.t) : int * int =
    raise Fail "TODO: TriangleCount.sml: your code here"

end
