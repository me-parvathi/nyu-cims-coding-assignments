signature TREE_DATA =
sig

  type key
  type value
  type avalue

  (* must define a total ordering over keys *)
  val cmp: key * key -> order

  (* produce a single "augmented value" for a particular key-value pair. *)
  val f: key * value -> avalue

  (* function g is used to combine augmented values.
   * g must be ASSOCIATIVE.
   *)
  val g: avalue * avalue -> avalue

  (* value z must be an appropriate identity element for g *)
  val z: avalue

end
