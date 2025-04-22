structure JsonFindStrings =
struct
  fun is_quote c = (c = #"\"")
  fun is_backslash c = (c = #"\\")
(* The whole thinf really is about counting backslashes
  1. all backslashes inside string are valid
  2. then count the number of \ - odd means " is escaped and even means " marks the end of the string
    2.1 keep track of start and beginnning??? *)
  fun in_string_flags json_chars =
    let
      val n = Seq.length json_chars
      
      
      val backslash_starts = Parallel.tabulate (0, n) (fn i =>
        if i = 0 then
          if is_backslash (Seq.nth json_chars i) then i else ~1
        else if is_backslash (Seq.nth json_chars i) then
          if is_backslash (Seq.nth json_chars (i-1)) then
            ~1  
          else
            i   
        else
          ~1    
      )

      (* indices to keep track of the start of a backslash seq *)
      val run_indices = Parallel.tabulate (0, n) (fn i =>
        if is_backslash (Seq.nth json_chars i) then
          let
            fun find_start j =
              if j < 0 then ~1
              else if Seq.nth backslash_starts j >= 0 then 
                Seq.nth backslash_starts j
              else
                if j = 0 then ~1
                else find_start (j-1)
          in
            find_start i
          end
        else ~1
      )

      (* Third pass: Count backslashes before each quote *)
      fun count_backslashes_before i =
        if i <= 0 then 0
        else if not (is_backslash (Seq.nth json_chars (i-1))) then 0
        else
          let
            val run_start = Seq.nth run_indices (i-1)
          in
            if run_start = ~1 then 0
            else i - run_start
          end

      (* Fourth pass: Determine if each quote is string-terminating *)
      val quote_states = Parallel.scan
        (fn (a, b) => not a = b)  (* Toggle state on terminating quotes *)
        false
        (0, n)
        (fn i =>
          if is_quote (Seq.nth json_chars i) then
            let
              val num_backslashes = count_backslashes_before i
            in
              num_backslashes mod 2 = 0  (* Even number of backslashes means terminating quote *)
            end
          else false
        )

      (* Final pass: Mark characters in strings *)
      val result = Parallel.tabulate (0, n) (fn i =>
        let
          val in_string = Seq.nth quote_states i
          val is_terminating_quote = 
            is_quote (Seq.nth json_chars i) andalso
            count_backslashes_before i mod 2 = 0
        in
          in_string orelse is_terminating_quote
        end
      )
    in
      result
    end
end