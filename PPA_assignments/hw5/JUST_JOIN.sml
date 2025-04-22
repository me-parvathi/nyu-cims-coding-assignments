signature JUST_JOIN =
sig

  structure Data: TREE_DATA

  type key = Data.key
  type value = Data.value
  type avalue = Data.avalue

  type tree
  datatype exposed = Leaf | Node of tree * key * value * tree
(* Node of - Node is the constructor and the "of" keyword tells as that there is moer data paertaining to Node coming  *)

  val expose: tree -> exposed
  val join: exposed -> tree
  val aval: tree -> avalue

end
