structure NQueensExt:
sig
  val count_solutions_and_boards: int -> int * int
end =
struct

  fun parallel_sum(lo, hi, f) =
    if lo >= hi then
      (0, 0)
    else if lo + 1 = hi then
      f(lo)
    else
      let
        val mid = lo + (hi - lo) div 2
        val (left, right) =
          ForkJoin.par (fn () => parallel_sum (lo, mid, f),
                        fn () => parallel_sum (mid, hi, f))
      in
        (#1 left + #1 right, #2 left + #2 right)
      end
    

  fun search(board, row) =
    if row >= ChessBoard.size(board) then
      (1, 0)
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
          (0, 1)
        else
          (* Continue searching from this child recursively *)
          let 
            val (nsols, nbc) = search(ChessBoard.put_queen (board, row, col), row + 1)
          in
            (nsols, nbc+1)
          end)

  fun count_solutions_and_boards(size) =
    (* raise Fail "TODO: NQueensExt.count_solutions_and_boards: replace me with your code" *)
    let
      val (nsols, nbc) = search(ChessBoard.empty(size), 0)
    in
      (nsols, nbc+1)
    end

end
