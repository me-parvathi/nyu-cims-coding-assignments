structure IntersectionCount:
sig
  val intersection_count: ('a * 'a -> order) -> ('a Seq.t * 'a Seq.t) -> int
end =
struct

  (* You might find this library function useful. It counts the number of
   * elements within s that are strictly less than x, assuming the input s
   * is sorted. O(log|s|) work and span. *)
  fun count_less (cmp: 'a * 'a -> order) (s: 'a Seq.t) (x: 'a) =
    BinarySearch.countLess cmp s x

  fun intersection_count cmp (s, t) =
    if Seq.length s = 0 orelse Seq.length t = 0 then 0
    else
      let
        (* keeping s as smaller sequence always *)
        val (s, t) =
          if Seq.length s > Seq.length t then (t, s) else (s, t)
        val m = Seq.length s
        val mid = m div 2

        val x = Seq.nth s mid
        val pos = count_less cmp t x
        val found =
          pos < Seq.length t andalso cmp (Seq.nth t pos, x) = EQUAL
        val current = if found then 1 else 0
        val s_left = Seq.subseq s (0, mid)
        val s_right = Seq.subseq s (mid + 1, m - (mid + 1))
        val t_left = Seq.subseq t (0, pos)
        val t_right =
          if found then
            Seq.subseq t (pos + 1, Seq.length t - (pos + 1))
          else
            Seq.subseq t (pos, Seq.length t - pos)
        val (left_count, right_count) =
          ForkJoin.par (fn () => intersection_count cmp (s_left, t_left),
                        fn () => intersection_count cmp (s_right, t_right))
      in
        left_count + right_count + current
      end

end