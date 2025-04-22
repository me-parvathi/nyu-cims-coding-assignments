structure LongestLine:
sig
  val longest_line: char Seq.t -> int * int
end =
struct

  fun is_newline (c: char) : bool = (c = #"\n")

  (* `longest_line(chars)` should return a tuple `(i, k)` where
   *   `i` is the line number of the longest line, and
   *   `k` is the length of the longest line
   *
   * Note that by convention, line numbers are indexed from 1, not 0.
   * For example, on the input "1\n123\n12", the longest line is line
   * number 2.
   *
   * If there are multiple lines that all are tied for the longest, then
   * you should return the first one which is longest. (The smallest line
   * number.)
   *
   * Your solution should consist of one big parallel reduction over the whole
   * input sequence. The cost should be O(n) work and O(log n) span in the
   * worst-case, where n is the length of the input.
   *
   * Some useful functions:
   *   Seq.length(s)       length of a sequence
   *   Seq.nth s i         get the ith element
   *   is_newline(c)       (defined above) check if a character is a newline
   *
   * The syntax for a reduction is:
   *   Parallel.reduce g z (lo, hi) f
   * The arguments are:
   *   g: 'a * 'a -> 'a    (for any type 'a) an associative "combining" function
   *   z: 'a               an identity element for `g`
   *   lo: int, hi: int    range of indices for `f`
   *   f: int -> 'a        a function to generate one element of the reduction
   *)
  fun longest_line (chars: char Seq.t) =
    raise Fail "TODO: LongestLine.longest_line: insert your code here"

end
