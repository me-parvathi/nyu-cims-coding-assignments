structure NQueens:
sig
  val count_solutions: int -> int
end =
struct

  (* `parallel_sum(lo, hi, f)` computes the sum:
   *   f(lo) + f(lo+1) + ... + f(hi-1)
   * It executes the function `f(i)` at every index `i` in the range 
   * lo <= i <= hi-1, and adds up the results. All of the function calls
   * `f(i)` in this range are executed in parallel.
   *)
  fun parallel_sum(lo, hi, f) =
    if lo >= hi then
      0
    else if lo + 1 = hi then
      f(lo)
    else
      let
        val mid = lo + (hi - lo) div 2
        val (left, right) =
          ForkJoin.par (fn () => parallel_sum (lo, mid, f),
                        fn () => parallel_sum (mid, hi, f))
      in
        left + right
      end


  (* Search from the given input board. Requires that the input board does NOT
   * have any threatened queens, and that the input board contains exactly one
   * queen in each row up until `row`. This function returns a count of
   * how many solutions it finds.
   *)
  fun search(board, row) =
    if row >= ChessBoard.size(board) then
      1
    else
      (* In parallel, consider N children, one for each column (col). For
       * each child, we either stop (due to a threatened queen) or continue
       * the search recursively. All of the results from the children are
       * summed together.
       *)
      parallel_sum(0, ChessBoard.size(board), fn col =>
        if ChessBoard.test_queen_is_threatened(board, row, col) then
          (* Stop: this board has a threatened queen. Return a count of 0
           * for this child. *)
          0
        else
          (* Continue searching from this child recursively *)
          search(ChessBoard.put_queen(board, row, col), row + 1))


  fun count_solutions(size) =
    search(ChessBoard.empty(size), 0)

end
