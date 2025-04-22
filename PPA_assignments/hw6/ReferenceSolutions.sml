structure ReferenceSolutions:
sig
  val intersection_count: ('a * 'a -> order) -> ('a Seq.t * 'a Seq.t) -> int
  val triangle_count: Graph.t -> int * int
end =
struct

  (* sequential solution just for correctness testing *)
  fun intersection_count cmp (s, t) =
    let
      val n = Seq.length s
      val m = Seq.length t
      fun loop acc i j =
        if i >= n then
          acc
        else if j >= m then
          acc
        else
          case cmp (Seq.nth s i, Seq.nth t j) of
            LESS => loop acc (i + 1) j
          | EQUAL => loop (acc + 1) (i + 1) (j + 1)
          | GREATER => loop acc i (j + 1)
    in
      loop 0 0 0
    end


  (* brute force, *very* slow solution, don't run this on anything substantial *)
  fun triangle_count g =
    let
      val n = Graph.num_vertices g

      fun exists_edge u v =
        let
          val nbrs = Graph.out_neighbors (g, u)
          val i = BinarySearch.search Int.compare nbrs v
        in
          i < Graph.out_degree (g, u) andalso Seq.nth nbrs i = v
        end

      val pointed = Parallel.reduce op+ 0 (0, n) (fn u =>
        Parallel.reduce op+ 0 (0, n) (fn v =>
          Parallel.reduce op+ 0 (0, n) (fn w =>
            if exists_edge u v andalso exists_edge v w andalso exists_edge u w then
              1
            else
              0)))

      val cyclic = Parallel.reduce op+ 0 (0, n) (fn u =>
        Parallel.reduce op+ 0 (0, n) (fn v =>
          Parallel.reduce op+ 0 (0, n) (fn w =>
            if exists_edge u v andalso exists_edge v w andalso exists_edge w u then
              1
            else
              0)))
    in
      (pointed, cyclic div 3)
    end

end
