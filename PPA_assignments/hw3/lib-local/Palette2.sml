structure Palette2 =
struct

  structure TC = TerminalColors
  structure TCS = TerminalColorString

  fun rgb_to_tc (r, g, b) =
    let
      fun c x = Real.fromInt x / 255.0
    in
      TC.rgb {red = c r, green = c g, blue = c b}
    end

  val blue = rgb_to_tc (1, 44, 121)
  val violet = rgb_to_tc (112, 4, 96)
  val rouge = rgb_to_tc (160, 44, 93)
  val red = rgb_to_tc (160, 44, 93)
  val orange = rgb_to_tc (238, 107, 59)
  val yellow = rgb_to_tc (251, 191, 84)
  val light_green = rgb_to_tc (171, 217, 110)
  val teal = rgb_to_tc (22, 194, 135)
  val green = rgb_to_tc (6, 115, 83)
  val dark_teal = rgb_to_tc (4, 84, 89)
  val midnight = rgb_to_tc (39, 41, 73)
  val dark_purple = rgb_to_tc (26, 19, 51)
  val gray = rgb_to_tc (166, 166, 166)
  val light_gray = rgb_to_tc (175, 175, 175)

  val rainbow_order = Seq.fromList
    [ red
    , yellow
    , teal
    , light_green
    , orange
    , rouge
    , green
    , violet
    , dark_teal
    , midnight
    , blue
    , dark_purple
    ]

end
