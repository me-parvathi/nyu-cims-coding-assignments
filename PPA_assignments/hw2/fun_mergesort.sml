fun mergesort(X) =
  if Seq.length(X) <= 1 then X
  else Parallel.reduce merge (Seq.empty()) (0, Seq.length(X) - 1)
         (fn i => Seq.singleton(Seq.nth X i))
