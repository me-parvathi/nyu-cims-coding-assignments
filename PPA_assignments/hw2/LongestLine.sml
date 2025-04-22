structure LongestLine =
struct
  (* A newline character *)
  fun is_newline c = (c = #"\n")
  
  (*
    We now define a result as a 5–tuple:
      (bestLine, bestLen, prefix, suffix, lineCount)
    where:
      bestLine  = global 1–based line number whose line (complete) has maximal length
      bestLen   = length of that best line
      prefix    = number of non-newline characters from the beginning of the chunk
      suffix    = number of characters after the last newline in the chunk
      lineCount = number of newlines in the chunk (i.e. complete lines)
  *)
  type result = int * int * int * int * int
  
  (* Process a single character *)
  fun process_char chars i =
    let
      val c = Seq.nth chars i
    in
      if is_newline c then
         (* For a newline we “complete” a (possibly empty) line.
            We set prefix = 0 and suffix = 0 and count a newline.
            (We choose bestLine = 1 and bestLen = 0 for an empty line.) *)
         (1, 0, 0, 0, 1)
      else
         (* For a non-newline character, there is no newline in the chunk.
            Thus, the single-character chunk is its own prefix and suffix,
            and the only candidate line is of length 1. *)
         (1, 1, 1, 1, 0)
    end
  
  (* Combine two segments A and B, whose results are
       A = (l1, len1, pre1, suf1, cnt1)
       B = (l2, len2, pre2, suf2, cnt2)
     In the combined chunk the new complete line count is cnt1 + cnt2.
  
     Also, the best candidate for a line may come from:
       1. A’s best candidate (candidate1: (l1, len1)),
       2. The “glued” line spanning the boundary (candidate2: (cnt1+1, suf1+pre2)), or 
       3. B’s best candidate (candidate3: (l2+cnt1, len2)).
  
     In addition, the combined chunk’s prefix is:
       - If A had no newline (cnt1 = 0) then the combined prefix is A.prefix + B.prefix;
       - Otherwise, it is simply A.prefix.
  
     And the combined suffix is:
       - If B had no newline (cnt2 = 0) then the combined suffix is A.suffix + B.suffix;
       - Otherwise, it is B.suffix.
  *)
  fun combine ((l1, len1, pre1, suf1, cnt1),
               (l2, len2, pre2, suf2, cnt2)) =
    let
      val merged = suf1 + pre2
      val mergedLine = cnt1 + 1  (* the spanning candidate comes right after all complete lines in A *)
      val candidate1 = (l1, len1)
      val candidate2 = (mergedLine, merged)
      val candidate3 = (l2 + cnt1, len2)
      (* A helper: choose the candidate with the larger length (tie: pick the left one) *)
      fun max_candidate ((ln1, n1): int * int, (ln2, n2): int * int) =
          if n1 >= n2 then (ln1, n1) else (ln2, n2)
      val best_candidate = max_candidate(candidate1, max_candidate(candidate2, candidate3))
      val newPrefix = if cnt1 = 0 then pre1 + pre2 else pre1
      val newSuffix = if cnt2 = 0 then suf1 + suf2 else suf2
      val newCount = cnt1 + cnt2
      val (bestL, bestLen) = best_candidate
    in
      (bestL, bestLen, newPrefix, newSuffix, newCount)
    end
  
  (* The identity is the “neutral” result.
     We choose (0,0,0,0,0) so that if no non-newline characters are seen,
     longest_line will return (1,0) (adjusting 0 to 1). *)
  val identity : result = (0, 0, 0, 0, 0)
  
  (* The main function *)
  fun longest_line (chars: char Seq.t) =
    let
      val (bestLine, bestLen, _, _, _) =
          Parallel.reduce combine identity (0, Seq.length chars) (process_char chars)
      val bestLine' = if bestLine = 0 then 1 else bestLine
    in
      (bestLine', bestLen)
    end
end
