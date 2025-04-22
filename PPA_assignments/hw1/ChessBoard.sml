(* Simple implementation of a chess board for the N-Queens problem.
 *
 * A board of size N has N rows and N columns. Positions on the board are
 * identified by their row and column index, indexed from 0. We use the
 * convention that (row=0, col=0) is the top-left corner.
 *
 * The position (row, col) is in-bounds if:
 *   0 <= row <= size-1  AND  0 <= col <= size-1
 *
 * ---------------------------------------------------------------------------
 *
 * ChessBoard.empty(N)
 *   creates an empty chess board of size N.
 *   (N rows, N columns)
 *
 * ChessBoard.size(board)
 *   returns the board size.
 *
 * ChessBoard.test_queen_is_threatened(board, row, col)
 *   returns true if a queen at position (row, col) would be threatened by
 *   another queen already on the board. (Returns false otherwise.)
 *
 * ChessBoard.put_queen(board, row, col)
 *   returns a new board that has an additional queen at position (row, col).
 *
 * ChessBoard.board_to_string(board)
 *   returns a string showing the board.
 *   For example:
 *     val board = ChessBoard.empty 5
 *     val board = ChessBoard.put_queen(board, 0, 0)
 *     val board = ChessBoard.put_queen(board, 1, 3)
 *     val board = ChessBoard.put_queen(board, 3, 2)
 *     val board = ChessBoard.put_queen(board, 4, 0)
 *     val () = print(ChessBoard.board_to_string(board))
 *   This prints:
 *     +-----------+
 *     | Q . . . . |
 *     | . . . Q . |
 *     | . . . . . |
 *     | . . Q . . |
 *     | Q . . . . |
 *     +-----------+
 *)
structure ChessBoard :>
sig
  type board
  type t = board

  val empty: int -> board
  val size: board -> int
  val test_queen_is_threatened: board * int * int -> bool
  val put_queen: board * int * int -> board
  val board_to_string: board -> string
end =
struct

  datatype board = Board of {size: int, queens: (int * int) list}
  type t = board


  fun empty size =
    if size < 0 then
      raise Fail
        ("ERROR: ChessBoard.empty(" ^ Int.toString size ^ "): size must be >= 0")
    else
      Board {size = size, queens = []}


  fun size (Board {size, ...}) = size


  fun position_error func_name size row col =
    raise Fail
      ("ERROR: " ^ func_name ^ "(board," ^ Int.toString row ^ ","
       ^ Int.toString col ^ "): board has size " ^ Int.toString size ^ "x"
       ^ Int.toString size ^ ". The position (row=" ^ Int.toString row
       ^ ", col=" ^ Int.toString col ^ ") is out-of-bounds.")


  fun test_queen_is_threatened (Board {size, queens}, row, col) =
    if row < 0 orelse row >= size orelse col < 0 orelse col >= size then
      position_error "ChessBoard.test_queen_is_threatened" size row col
    else
      List.exists
        (fn (x, y) =>
           row = x orelse col = y orelse row - col = x - y
           orelse row + col = x + y) queens


  fun put_queen (Board {size, queens}, row, col) =
    if row < 0 orelse row >= size orelse col < 0 orelse col >= size then
      position_error "ChessBoard.put_queen" size row col
    else
      Board {size = size, queens = (row, col) :: queens}


  fun board_to_string (Board {size, queens}) =
    let
      val flags = Array.tabulate (size * size, fn i => false)
      val _ =
        List.app (fn (row, col) => Array.update (flags, size * row + col, true))
          queens

      fun make_row r =
        "| "
        ^
        CharVector.tabulate (2 * size, fn i =>
          if i mod 2 = 1 then #" "
          else if Array.sub (flags, size * r + (i div 2)) then #"Q"
          else #".") ^ "|\n"

      val top_bottom_line =
        "+-" ^ CharVector.tabulate (2 * size, fn _ => #"-") ^ "+\n"
    in
      top_bottom_line ^ String.concat (List.tabulate (size, make_row))
      ^ top_bottom_line
    end


  fun example () =
    let
      val board = empty 5
      val board = put_queen (board, 0, 0)
      val board = put_queen (board, 1, 3)
      val board = put_queen (board, 3, 2)
      val board = put_queen (board, 4, 0)
      val () = print (board_to_string board)
    in
      ()
    end

end
