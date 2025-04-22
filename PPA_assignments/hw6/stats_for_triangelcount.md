============================bkp2============================================
[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- soc-LiveJournal1.txt 
Loading graph (if large, this might take a while...)
  Warning: graph contains 518382 self-loops
Loaded graph in 5.5334s
num vertices 4847571
num edges    68993773
--------------------
warmup 0.0000
repeat 1
time 63.4398s

average 63.4398s
minimum 63.4398s
maximum 63.4398s
std dev 0.0000s
--------------------
num pointed triangles 946400853
num cyclic triangles  243660960
[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- com-orkut.ungraph.txt 
Loading graph (if large, this might take a while...)
Loaded graph in 9.1934s
num vertices 3072441
num edges    117185083
--------------------
warmup 0.0000
repeat 1
time 79.4798s

average 79.4798s
minimum 79.4798s
maximum 79.4798s
std dev 0.0000s
--------------------
num pointed triangles 627584181
num cyclic triangles  0

==================================bkp=====================================================================

[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- cit-Patents.txt
Loading graph (if large, this might take a while...)
  Warning: graph contains 1 self-loops
Loaded graph in 1.3608s
num vertices 3774768
num edges    16518948
--------------------
warmup 0.0000
repeat 1
time 1.2048s

average 1.2048s
minimum 1.2048s
maximum 1.2048s
std dev 0.0000s
--------------------
num pointed triangles 7515027
num cyclic triangles  0
[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- soc-LiveJournal1.txt 
Loading graph (if large, this might take a while...)
  Warning: graph contains 518382 self-loops
Loaded graph in 5.6490s
num vertices 4847571
num edges    68993773
--------------------
warmup 0.0000
repeat 1
time 63.3144s

average 63.3144s
minimum 63.3144s
maximum 63.3144s
std dev 0.0000s
--------------------
num pointed triangles 946400853
num cyclic triangles  243660960
[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- com-orkut.ungraph.txt 
Loading graph (if large, this might take a while...)
Loaded graph in 9.2531s
num vertices 3072441
num edges    117185083
--------------------
warmup 0.0000
repeat 1
time 84.7253s

average 84.7253s
minimum 84.7253s
maximum 84.7253s
std dev 0.0000s
--------------------
num pointed triangles 627584181
num cyclic triangles  0

====================================================linearintersection===================================================
[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- cit-Patents.txt
Loading graph (if large, this might take a while...)
  Warning: graph contains 1 self-loops
Loaded graph in 1.3851s
num vertices 3774768
num edges    16518948
--------------------
warmup 0.0000
repeat 1
time 0.3482s

average 0.3482s
minimum 0.3482s
maximum 0.3482s
std dev 0.0000s
--------------------
num pointed triangles 7515027
num cyclic triangles  0
[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- soc-LiveJournal1.txt 
Loading graph (if large, this might take a while...)
  Warning: graph contains 518382 self-loops
Loaded graph in 5.5623s
num vertices 4847571
num edges    68993773
--------------------
warmup 0.0000
repeat 1
time 9.3169s

average 9.3169s
minimum 9.3169s
maximum 9.3169s
std dev 0.0000s
--------------------
num pointed triangles 946400853
num cyclic triangles  243660960
[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- com-orkut.ungraph.txt 
Loading graph (if large, this might take a while...)
Loaded graph in 9.3043s
num vertices 3072441
num edges    117185083
--------------------
warmup 0.0000
repeat 1
time 22.3526s

average 22.3526s
minimum 22.3526s
maximum 22.3526s
std dev 0.0000s
--------------------
num pointed triangles 627584181
num cyclic triangles  0
[pb3071@crunchy1 hw6]$ 

==================================================same code without linear intersection coun==========================
Loading graph (if large, this might take a while...)
  Warning: graph contains 1 self-loops
Loaded graph in 1.3917s
num vertices 3774768
num edges    16518948
--------------------
warmup 0.0000
repeat 1
time 1.2222s

average 1.2222s
minimum 1.2222s
maximum 1.2222s
std dev 0.0000s
--------------------
num pointed triangles 7515027
num cyclic triangles  0
[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- soc-LiveJournal1.txt 
Loading graph (if large, this might take a while...)
  Warning: graph contains 518382 self-loops
Loaded graph in 5.7215s
num vertices 4847571
num edges    68993773
--------------------
warmup 0.0000
repeat 1
time 64.1282s

average 64.1282s
minimum 64.1282s
maximum 64.1282s
std dev 0.0000s
--------------------
num pointed triangles 946400853
num cyclic triangles  243660960
[pb3071@crunchy1 hw6]$ ./main @mpl procs 32 -- com-orkut.ungraph.txt 
Loading graph (if large, this might take a while...)
Loaded graph in 9.4239s
num vertices 3072441
num edges    117185083
--------------------
warmup 0.0000
repeat 1
time 87.1766s

average 87.1766s
minimum 87.1766s
maximum 87.1766s
std dev 0.0000s
--------------------
num pointed triangles 627584181
num cyclic triangles  0