# Asset layout

The active asset set was imported from `Tile_Scrolling_Tutorial_Game_Assets`
on 2026-08-12 and normalized by purpose instead of the original Scratch
target/costume layout.

- `audio/`: music, sound effects, and character voice clips
- `backgrounds/`: stage and scrolling background images
- `characters/`: Mario sprites grouped by small, super, and fire state
- `effects/`: smoke and projectile effects
- `enemies/`: enemy sprites grouped by enemy and color variant, including
  the gorilla boss in `enemies/boss_gorilla/`
- `items/`: coins and power-ups
- `maps/`: editable project map files
- `metadata/`: untouched source manifests and import summary
- `reference/`: Scratch placeholders, viewport art, and palette-embedded copies
- `runtime/`: compact atlases loaded by the current SDL renderer
- `tiles/`: blocks, terrain, pipes, masks, and scenery
- `ui/`: HUD and end-box art

`runtime/mario_super.png` contains 32 x 56 cells: idle on row 0, four
walking frames on row 1, and jump on row 2. `runtime/world_tiles.png`
contains four 32 x 32 cells: wood block, question block, coin, and opened
gold block.

`enemies/boss_gorilla/` holds the gorilla boss frames cut from the boss
sprite sheet and named per boss action: `idle_1..3`, `throw_windup_1..2`,
`throw_1..2`, `recover_1..2`, `charge_1..4`, `slam_1..5`, `hurt_1..2`,
`enraged_1..4`, `dodge_1..6`, and `death_1..3`. Frames face right and are
trimmed like the other sprites, so `ActorRenderer` anchors them bottom-center
from the texture size; the tallest standing frame is 88 px.

The previous `assets/` directory is preserved unchanged at
`../backups/assets_before_tile_scrolling_import_2026-08-12/`.
