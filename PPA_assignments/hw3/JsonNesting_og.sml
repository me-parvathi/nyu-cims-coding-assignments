structure JsonNesting:
sig
  val bracket_depths: char Seq.t -> bool Seq.t -> (int * int * int) Seq.t
end =
struct

  fun is_quote c = (c = #"\"")
  fun is_backslash c = (c = #"\\")
  fun is_bracket c =
    c = #"{" orelse c = #"}" orelse c = #"[" orelse c = #"]"

  (* Implement the function `bracket_depths json_chars in_string_flags` where
   * `json_chars` is a sequence of characters from a valid JSON file, and
   * `in_string_flags` is a sequence of booleans indicating whether or not
   * each character of the JSON file is inside a string (i.e., the result of
   * the first coding task in JsonFindStrings.sml). Here, you can assume that
   * these flags have been computed correctly.
   *
   * Your goal here is to compute the nesting depth of every bracket in the
   * JSON file. Brackets can either be curly or square, i.e.:
   *    { } [ ]
   * 
   * The output should be a sequence of tuples `(i, dc, ds)`, each of which
   * corresponds to one bracket in the file, where:
   *
   *   `i` is the index (in `json_chars`) of the bracket
   *
   *   `dc` is the "curly depth", i.e., the number of
   *   curly bracket pairs that surround index `i`
   *   (NOT including the bracket at position `i`)
   *
   *   `ds` is the "square depth", i.e., the number of
   *   square bracket pairs that surround index `i`
   *   (NOT including the bracket at position `i`)
   *
   * For example, consider the following example input, which has 10 brackets:
   *
   *   in_string_flags:   01111111000111111100000000000000000
   *        json_chars:   {"he][o":[{"wo{ld":1},[1,[2,3],4]]}
   *                      ^        ^^         ^ ^  ^   ^  ^^^
   *
   * The output for this example should be:
   *   (0,  0, 0)      open { at index 0; no surrounding brackets
   *   (9,  1, 0)      open [ at index 9; 1 surrounding curly pair
   *   (10, 1, 1)      open { at index 10; 1 surrounding curly pair, and 1 surrounding square pair
   *   (20, 1, 1)      close } at index 20; 1 surrounding curly and 1 surrounding square
   *   (22, 1, 1)      etc...
   *   (25, 1, 2)
   *   (29, 1, 2)
   *   (32, 1, 1)
   *   (33, 1, 0)
   *   (34, 0, 0)
   *
   * Notice that the strings of the example include bracket characters, but
   * these are just part of the string, and should NOT be parsed as actual
   * brackets of the JSON file.
   *
   * We encourage using functions such as the following:
   *
   *   Parallel.tabulate: (int * int) -> (int -> 'a) -> 'a Seq.t
   *   Parallel.reduce: ('a * 'a -> 'a) -> 'a -> (int * int) -> (int -> 'a) -> 'a
   *   Parallel.scan: ('a * 'a -> 'a) -> 'a -> (int * int) -> (int -> 'a) -> 'a Seq.t (* length N+1 *)
   *   Parallel.filter: (int * int) -> (int -> 'a) -> (int -> bool) -> 'a Seq.t
   *
   * There are also a few utility function defined above; feel free to add
   * more or edit these as desired.
   *
   *   is_quote: char -> bool
   *   is_backslash: char -> bool
   *   is_bracket: char -> bool
   *
   * And, as always, you will need functions on sequences:
   *
   *   Seq.length: 'a Seq.t -> int           O(1) work, O(1) span
   *   Seq.nth: 'a Seq.t -> int -> 'a        O(1) work, O(1) span
   *
   * You may assume that tabulate, reduce, scan, and filter all have linear
   * work and polylogarithmic span, assuming the functions given as argument
   * cost O(1) work and span.
   *
   * Formally, assuming the functions f, g, and p all require O(1) work and
   * span, you may assume the following cost specifications:
   *   tabulate (lo, hi) f        O(hi-lo) work, O(log(hi-lo)) span
   *   reduce g z (lo, hi) f      O(hi-lo) work, O(log(hi-lo)) span
   *   scan g z (lo, hi) f        O(hi-lo) work, O(log(hi-lo)) span
   *   filter (lo, hi) f p        O(hi-lo) work, O(log(hi-lo)) span
   *
   * COST REQUIREMENT:
   *   **Your solution must have O(N) work and O(polylog(N)) span**,
   *   where N is the length (number of characters) of the input.
   *)
  fun bracket_depths json_chars in_string_flags =
    raise Fail "JsonNesting.bracket_depths: TODO: your code here"

end
