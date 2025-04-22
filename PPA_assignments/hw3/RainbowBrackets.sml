structure RainbowBrackets:
sig
  val make: char Seq.t
            -> (int * int) Seq.t (* (bracket index, bracket depth) tuples *)
            -> bool Seq.t (* in string flags *)
            -> TerminalColorString.t
end =
struct

  structure TC = TerminalColors
  structure TCS = TerminalColorString

  val colors = Palette1.rainbow_order

  fun make chars bracket_depths in_string_flags =
    let
      val n = Seq.length chars

      val () =
        if
          Seq.length bracket_depths = 0
          orelse
          Parallel.reduce (fn (a, b) => a andalso b) true
            (0, Seq.length bracket_depths - 1)
            (fn i =>
               let
                 val (j1, _) = Seq.nth bracket_depths i
                 val (j2, _) = Seq.nth bracket_depths (i + 1)
               in
                 0 <= j1 andalso j1 < n andalso 0 <= j2 andalso j2 < n
                 andalso j1 < j2
               end)
        then
          ()
        else
          raise Fail
            "RainbowBrackets.make: unrecoverable error. bracket_depths indices must be in range, and sorted"

      fun in_string i =
        if i >= Seq.length in_string_flags then false
        else Seq.nth in_string_flags i
      fun is_string_start i =
        i < n andalso in_string i andalso (i = 0 orelse not (in_string (i - 1)))
      fun is_string_stop i =
        (i = n andalso in_string (n - 1))
        orelse (not (in_string i) andalso i > 0 andalso in_string (i - 1))

      fun no_brackets_strand start stop =
        if start >= stop then
          TCS.empty
        else
          let
            (* val () = print
              ("no_brackets_strand " ^ Int.toString start ^ " "
               ^ Int.toString stop ^ "\n") *)

            val boundaries =
              Parallel.filter (start, stop + 1) (fn i => i) (fn i =>
                is_string_start i orelse is_string_stop i)
            val num_string_ranges = Seq.length boundaries div 2
            fun string_range i =
              (Seq.nth boundaries (2 * i), Seq.nth boundaries (2 * i + 1))
          in
            if num_string_ranges = 0 then
              TCS.italic
                (TCS.foreground Palette2.light_gray
                   (TCS.fromString (CharVector.tabulate (stop - start, fn k =>
                      Seq.nth chars (start + k)))))
            else
              Parallel.reduce TCS.append TCS.empty (0, num_string_ranges + 1)
                (fn i =>
                   let
                     (* val () = print
                       ("no_brackets_strand.piece " ^ Int.toString i ^ "\n") *)

                     val start_here =
                       if i = 0 then start else #2 (string_range (i - 1))

                     val (string_start, string_stop) =
                       if i = num_string_ranges then (stop, stop)
                       else string_range i

                     val non_string_part =
                       CharVector.tabulate (string_start - start_here, fn k =>
                         Seq.nth chars (start_here + k))

                     val string_part =
                       CharVector.tabulate (string_stop - string_start, fn k =>
                         Seq.nth chars (string_start + k))
                   in
                     TCS.append
                       ( TCS.italic
                           (TCS.foreground Palette2.light_gray
                              (TCS.fromString non_string_part))
                       , TCS.foreground Palette1.yellow
                           (TCS.fromString string_part)
                       )
                   end)
          end

      fun strand i =
        let
          val start =
            if i = 0 then 0 else 1 + #1 (Seq.nth bracket_depths (i - 1))
          val (stop, d) = Seq.nth bracket_depths i
          val d = d mod (Seq.length colors)
        in
          (* print
            ("strand " ^ Int.toString i ^ " " ^ Int.toString start ^ " "
             ^ Int.toString stop ^ "\n"); *)
          TCS.append
            ( no_brackets_strand start stop
            , TCS.bold (TCS.foreground (Seq.nth colors d) (TCS.fromChar
                (Seq.nth chars stop)))
            )
        end

      val last =
        let
          val start =
            if Seq.length bracket_depths = 0 then 0
            else 1 + #1 (Seq.nth bracket_depths (Seq.length bracket_depths - 1))
          val stop = Seq.length chars
        in
          (* print ("last " ^ Int.toString start ^ " " ^ Int.toString stop ^ "\n"); *)
          no_brackets_strand start stop
        end
    in
      TCS.background TC.white (TCS.append
        ( Parallel.reduce TCS.append TCS.empty (0, Seq.length bracket_depths)
            strand
        , last
        ))
    end

end
