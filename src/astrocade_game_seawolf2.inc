
;===============================================================================
; Sea Wolf II assembler definitions
;
; Hardware ports, input masks, memory ownership, structure offsets, gameplay
; constants, and TERSE runtime values used by seawolf2.asm.  This file is
; self-contained and has no external include dependency.
;===============================================================================

; Sea Wolf II I/O ownership.  Ports $00-$0F are the Astrocade data-chip
; registers; $10-$13 are cabinet inputs; $19 is the Function Generator color
; pair; $40-$43 are discrete sound, counter and station-lamp outputs.  The ROM
; does not access $14-$18 or $1A-$3F.  Hardware reads at $08/$0E/$0F exist,
; but this ROM uses those three addresses only as write registers.
PORT_COLOR_0            EQU     $00
PORT_COLOR_1            EQU     $01
PORT_COLOR_2            EQU     $02
PORT_COLOR_3            EQU     $03
PORT_COLOR_4            EQU     $04
PORT_COLOR_5            EQU     $05
PORT_COLOR_6            EQU     $06
PORT_COLOR_7            EQU     $07
PORT_VIDEO_MODE         EQU     $08 ; W mode bit 0; R returns/clears FG intercept
PORT_COLOR_SPLIT        EQU     $09 ; split X in bits 0-5, background color 6-7
PORT_VBLANK_LINE        EQU     $0A
PORT_COLOR_BLOCK        EQU     $0B ; high I/O address bits select color 0-7
PORT_FUNCGEN_CONTROL    EQU     $0C
PORT_INTERRUPT_VECTOR   EQU     $0D ; IM 2 vector low byte; write clears IRQ
PORT_INTERRUPT_ENABLE   EQU     $0E ; write clears IRQ; read is lightpen V
PORT_INTERRUPT_LINE     EQU     $0F ; write clears IRQ; read is lightpen H
PORT_EXPAND_COLOR       EQU     $19
PORT_LEFT_STATION_HANDLE  EQU   $10
PORT_RIGHT_STATION_HANDLE EQU   $11
PORT_COIN_START         EQU     $12
PORT_DIP_SWITCHES       EQU     $13
PORT_SOUND_EVENTS       EQU     $40
PORT_SOUND_CONTROL      EQU     $41
PORT_LEFT_LAMPS         EQU     $42
PORT_RIGHT_LAMPS        EQU     $43

; $40 rising-edge sound triggers:
;   bit 0 left torpedo, 1 left ship hit, 2 left mine hit,
;   bit 3 right torpedo, 4 right ship hit, 5 right mine hit.
; $41 ownership:
;   bits 0-2 dive pan, bit 3 dive trigger, bit 4 right sonar,
;   bit 5 left sonar, bit 6 mechanical coin counter, bit 7 sample enable.
; Each station lamp latch uses bits 0-3 for torpedoes 4..1, bit 4 for READY
; and active-low RELOAD, and bit 5 for the hit/explosion lamp.

; Cabinet/station ABI.  Logical player 1 occupies the right station and is the
; only station active in a one-player game.  Logical player 2 occupies the left
; station and is enabled only in a two-player game.
;
;              handle   lamps   torpedo records   color   hit-side flag
; left / P2     $10      $42     even: $C0FA...    $08     bit 3 = 1
; right / P1    $11      $43     odd:  $C113...    $04     bit 3 = 0
;
; Both handle ports contain a six-bit Gray-coded position and an active-high
; fire input.  Torpedo-record address parity becomes target flag bit 3 on a
; hit; scoring, hit lamps, bonus state, and collision sound consume that bit.
RAW_HANDLE_POSITION_MASK        EQU $3F
RAW_HANDLE_GRAY_MSB             EQU $20
DECODED_HANDLE_POSITION_MASK    EQU $7F
HANDLE_FIRE_MASK                EQU $80
STATION_PORT_PARITY_BIT         EQU $00 ; port $10 left, port $11 right

COIN_INPUT_MASK                 EQU $01 ; port $12 bit 0
INPUT_ONE_PLAYER_START_BIT      EQU $01 ; port $12 bit 1
INPUT_TWO_PLAYER_START_BIT      EQU $02 ; port $12 bit 2
INPUT_ONE_PLAYER_START_MASK     EQU $02
INPUT_TWO_PLAYER_START_MASK     EQU $04
INPUT_START_BUTTON_MASK         EQU $06
LANGUAGE_GERMAN_SWITCH_MASK     EQU $08 ; port $12 bit 3
LANGUAGE_FRENCH_SWITCH_MASK     EQU $40 ; port $11 bit 6

PLAYER_COUNT_ONE                EQU $01
PLAYER_COUNT_TWO                EQU $02

RAM_BASE                EQU     $C000
TERSE_DATA_STACK_TOP    EQU     $C3E2
TERSE_DATA_STACK_LOW_WATER EQU  $C3DE       ; two live TERSE cells
TERSE_CONTROL_STACK_TOP EQU     $C400
TERSE_CONTROL_STACK_LOW_WATER EQU $C3FA     ; three live control cells

VIDEO_RAM_BASE          EQU     $4000
VIDEO_RAM_LIMIT         EQU     $8000
WORK_RAM_LIMIT          EQU     $C400
NAMED_RUNTIME_STATE_END EQU     $C222
STACK_RESERVE_BASE      EQU     $C222
TERSE_CONTROL_STACK_FIRST EQU   $C3FE

; Work-RAM ownership, $C000-$C3FF
;   $C000-$C063  four 25-byte surface-target records
;   $C064-$C0F9  six 25-byte mine records
;   $C0FA-$C1C1  eight 25-byte interleaved station-torpedo records
;   $C1C2-$C1C7  three persistent scheduler cursor words
;   $C1C8-$C1C9  unreferenced; touched only by whole-RAM clear/diagnostic
;   $C1CA-$C1F7  frame guard, timers, game, score, station and bonus state
;   $C1F8-$C221  persistent foreground, text and raster-scheduler state
;   $C222-$C3E1  no absolute owner; downward-growing SP stack reserve
;   $C3E2-$C3F9  gap between the initialized SP and proven IX low-water mark
;   $C3FA-$C3FF  three live IX control cells at maximum TERSE nesting
;
; The power-on diagnostic destructively tests the complete $C000-$C3FF range.
; Its ROM-failure text buffer overlays $C000-$C001 before object pools exist.

; Native text renderer state used by the power-on and interactive diagnostics.
TEXT_X_POSITION_LO      EQU     $C1FE
TEXT_X_POSITION_HI      EQU     $C1FF
TEXT_Y_POSITION_LO      EQU     $C200
TEXT_Y_POSITION         EQU     $C201
TEXT_COLOR              EQU     $C202
TEXT_DOUBLE_SIZE_FLAG   EQU     $C1DD

; Foreground input, credit and prompt state.
START_ELIGIBILITY_FLAGS EQU     $C203
START_CREDIT_COST       EQU     $C204
LANGUAGE_SELECTION      EQU     $C205
CREDIT_COUNT            EQU     $C206
COIN_INPUT_QUEUE        EQU     $C207
COIN_INPUT_EDGE_LATCH   EQU     $C1F9
RESET_ONLY_RUNTIME_BYTE_C20A EQU $C20A
TARGET_TYPE_SEQUENCE_CURSOR EQU $C20B

START_FLAG_ONE_PLAYER   EQU     $02
START_FLAG_TWO_PLAYER   EQU     $04
START_FLAG_ONE_PLAYER_BIT EQU   $01
START_FLAG_TWO_PLAYER_BIT EQU   $02

; Operator switch bank S1 at port $13.  The manual numbers the two pricing
; contacts S1-1 and S1-2 even though their bits appear in reverse order here.
DIP_COINAGE_MASK                EQU $01 ; S1-2
DIP_PLAY_TIME_MASK              EQU $06 ; S1-3/S1-4
DIP_TWO_PLAYER_PRICE_MASK       EQU $08 ; S1-1
DIP_PRICING_MASK                EQU $09 ; S1-1/S1-2 together
DIP_EXTENDED_PLAY_MASK          EQU $30 ; S1-5/S1-6
DIP_MONITOR_TYPE_MASK           EQU $40 ; S1-7: 0 B/W, 1 color
DIP_SERVICE_MODE_MASK           EQU $80 ; S1-8: 0 test, 1 play

PRICING_1P_2C_2P_2C            EQU $00 ; S1-1 OFF, S1-2 OFF
PRICING_1P_1C_2P_1C            EQU $01 ; S1-1 OFF, S1-2 ON
PRICING_1P_2C_2P_4C            EQU $08 ; S1-1 ON,  S1-2 OFF
PRICING_1P_1C_2P_2C            EQU $09 ; S1-1 ON,  S1-2 ON

DIP_PLAY_TIME_1P70_2P90        EQU $00
DIP_PLAY_TIME_1P60_2P75        EQU $02
DIP_PLAY_TIME_1P50_2P60        EQU $04
DIP_PLAY_TIME_1P40_2P45        EQU $06

DIP_EXTENDED_PLAY_NONE         EQU $00
DIP_EXTENDED_PLAY_5000         EQU $10
DIP_EXTENDED_PLAY_6000         EQU $20
DIP_EXTENDED_PLAY_7000         EQU $30

DIP_MONITOR_BLACK_AND_WHITE    EQU $00
DIP_MONITOR_COLOR              EQU $40
DIP_SERVICE_MODE_TEST          EQU $00
DIP_SERVICE_MODE_PLAY          EQU $80

LANGUAGE_ENGLISH        EQU     $00
LANGUAGE_GERMAN         EQU     $01
LANGUAGE_FRENCH         EQU     $02

INITIAL_INTERRUPT_ENABLE EQU   $08
INITIAL_COLOR_SPLIT_VALUE EQU  $2A

PROMPT_TEXT_COLOR       EQU     EXPAND_COLORS_0_3
PROMPT_TEXT_X           EQU     $28
PROMPT_INITIAL_Y        EQU     $3E
PROMPT_LINE_Y_STEP      EQU     $0C
PROMPT_TEXT_X_WORD      EQU     $2800
PROMPT_INITIAL_Y_WORD   EQU     $3E00
PROMPT_LINE_Y_STEP_WORD EQU     $0C00
GERMAN_PROMPT_TABLE_OFFSET EQU  $011A
FRENCH_PROMPT_TABLE_OFFSET  EQU  $0339

FONT_ASCII_BASE         EQU     $30
FONT_ASCII_LAST         EQU     $5A
FONT_BYTES_PER_GLYPH    EQU     $0A
FONT_GLYPH_COUNT        EQU     $2B
VIDEO_ROW_STRIDE        EQU     $0050
TEXT_DOUBLE_ROW_STRIDE  EQU     $00A0
TEXT_NORMAL_X_ADVANCE   EQU     $0400
TEXT_DOUBLE_X_ADVANCE   EQU     $0800

; Function Generator control register at port $0C.  The hardware pipeline is
; expand -> shift/rotate -> flop -> OR/XOR -> display RAM.  Bits 0-1 select a
; shift of 0-3 two-bit pixels.  Expand alternates high and low source nibbles;
; rotate buffers the first four writes and emits four transposed bytes on the
; next four; flop reverses the four two-bit pixels in one byte.  OR/XOR combine
; with the addressed display byte and update intercept feedback readable at
; port $08.  Writing port $0C resets the shift latch, expansion phase and rotate
; counter.  Bit 7 has no hardware function.
FUNCGEN_SHIFT_MASK          EQU $03
FUNCGEN_ROTATE_BIT          EQU $02
FUNCGEN_EXPAND_BIT          EQU $03
FUNCGEN_OR_BIT              EQU $04
FUNCGEN_XOR_BIT             EQU $05
FUNCGEN_FLOP_BIT            EQU $06
FUNCGEN_ROTATE_MASK         EQU $04
FUNCGEN_EXPAND_MASK         EQU $08
FUNCGEN_OR_MASK             EQU $10
FUNCGEN_XOR_MASK            EQU $20
FUNCGEN_FLOP_MASK           EQU $40

FUNCGEN_MODE_REPLACE        EQU $00
FUNCGEN_MODE_EXPAND         EQU $08
FUNCGEN_MODE_EXPAND_OR      EQU $18
FUNCGEN_MODE_EXPAND_FLOP    EQU $48

; Observed controls are $00, $08-$0B, $18 and $48-$4B.  The low two bits of
; object modes vary with horizontal position.  OR appears only in double-size
; text.  This ROM never asserts rotate or XOR.

; Port $19 packs the output color for a zero source bit in bits 0-1 and the
; output color for a one source bit in bits 2-3.  Sea Wolf II always maps zero
; to color 0 and selects color 1, 2 or 3 for set pixels.
EXPAND_COLORS_0_1           EQU $04
EXPAND_COLORS_0_2           EQU $08
EXPAND_COLORS_0_3           EQU $0C

; CPU writes to $0000-$3FFF pass through the Function Generator and land at
; display RAM $4000-$7FFF.  The last two bytes form a safe expansion/shift
; scratch pair: write through $3FFE/$3FFF, then read the result at $7FFE/$7FFF.
MAGIC_WRITE_WINDOW_BASE     EQU $0000
MAGIC_WRITE_WINDOW_LIMIT    EQU $4000
FUNCGEN_SCRATCH_WRITE_0     EQU $3FFE
FUNCGEN_SCRATCH_WRITE_1     EQU $3FFF
FUNCGEN_SCRATCH_READ_0      EQU $7FFE
FUNCGEN_SCRATCH_READ_1      EQU $7FFF

PLAYER_STATUS_COLOR     EQU     EXPAND_COLORS_0_3
PLAYER_STATUS_Y         EQU     $B8
PLAYER_STATUS_LEFT_X    EQU     $07
PLAYER_STATUS_RIGHT_X   EQU     $82
PLAYER_STATUS_CENTER_X  EQU     $4D
PLAYER_STATUS_LEFT_X_WORD   EQU $0700
PLAYER_STATUS_RIGHT_X_WORD  EQU $8200
PLAYER_STATUS_CENTER_X_WORD EQU $4D00
PLAYER_STATUS_ROWS      EQU     $05
PLAYER_STATUS_SOURCE_WIDTH EQU  $03

SELF_TEST_TEXT_BUFFER   EQU     $C000
SELF_TEST_ROM_BLOCK_SIZE EQU    $0800
SELF_TEST_PALETTE_BYTES EQU     $08
SELF_TEST_FAILURE_PATTERN_BYTES EQU $0028
SELF_TEST_ROM_EXPECTED_SUM EQU  $FF
SELF_TEST_ROM_SENTINEL_X EQU    $32
SELF_TEST_FAILURE_COLOR EQU     $47

GAME_CLOCK_DIVIDER          EQU $C1DA
GAME_TIME_BCD               EQU $C1DB
LAST_DRAWN_GAME_TIME_BCD    EQU $C1DC
CONTROL_LOOP_EXIT_FLAG      EQU $C1DE
PATROL_COMPLETE_FLAG        EQU $C1DF
EXTENDED_PATROL_TIME_BCD    EQU $C1E0
ACTIVE_PLAYER_COUNT         EQU $C1FB

; Player score and torpedo-result state.  Each score is two packed-BCD bytes.
; A ship hit clears the active-low redraw latch and advances the station's
; consecutive-hit count/value.  A torpedo miss or mine collision clears that
; streak; reaching four ship hits awards the accumulated value a second time.
LEFT_SCORE_BCD_LO               EQU $C1E2
LEFT_SCORE_BCD_HI               EQU $C1E3
LEFT_SCORE_REDRAW_LATCH         EQU $C1E4
LEFT_HIT_STREAK_COUNT           EQU $C1E5
LEFT_HIT_STREAK_VALUE_BCD       EQU $C1E6
RIGHT_SCORE_BCD_LO              EQU $C1E7
RIGHT_SCORE_BCD_HI              EQU $C1E8
RIGHT_SCORE_REDRAW_LATCH        EQU $C1E9
RIGHT_HIT_STREAK_COUNT          EQU $C1EA
RIGHT_HIT_STREAK_VALUE_BCD      EQU $C1EB

LEFT_FIRE_EDGE_LATCH            EQU $C1EC
LEFT_TORPEDOES_REMAINING        EQU $C1ED
LEFT_LAMP_STATE                 EQU $C1EE
RIGHT_FIRE_EDGE_LATCH           EQU $C1EF
RIGHT_TORPEDOES_REMAINING       EQU $C1F0
RIGHT_LAMP_STATE                EQU $C1F1
RIGHT_BONUS_DISPLAY_ACTIVE      EQU $C1F2
RIGHT_BONUS_DISPLAY_VALUE_BCD   EQU $C1F3
LEFT_BONUS_DISPLAY_ACTIVE       EQU $C1F4
LEFT_BONUS_DISPLAY_VALUE_BCD    EQU $C1F5
RESET_ONLY_STATE_C1F6           EQU $C1F6
SUPER_SUB_SPAWN_COUNT           EQU $C1F7
NEW_HIGH_SCORE_FLAG             EQU $C1F8

HIGH_SCORE_BCD_LO               EQU $C208
HIGH_SCORE_BCD_HI               EQU $C209

; SELF_TEST_PARAMETER_SET_* layout.  The first two bytes form an executable
; JR instruction.  A successful range test jumps through IY and executes that
; JR, selecting the next diagnostic phase without a separate return address.
SELF_TEST_RANGE_LOWER_PAGE EQU  $02
SELF_TEST_RANGE_UPPER_PAGE EQU  $03
SELF_TEST_RANGE_START_LO   EQU  $04
SELF_TEST_RANGE_START_HI   EQU  $05

; Object/entity record ABI.  All three scheduler pools use this exact $19-byte
; layout.  Vertical motion is accelerated fixed-point; horizontal motion uses
; a constant fixed-point velocity.  In each little-endian pair the low byte is
; fractional and the high byte is the screen coordinate used by rendering and
; collision code.
OBJECT_RECORD_SIZE          EQU $19
OBJECT_FLAGS                EQU $00
OBJECT_TYPE                 EQU $01
OBJECT_TIMER                EQU $02
OBJECT_Y_ACCEL_LO           EQU $03
OBJECT_Y_ACCEL_HI           EQU $04
OBJECT_Y_VELOCITY_LO        EQU $05
OBJECT_Y_VELOCITY_HI        EQU $06
OBJECT_Y_POSITION_LO        EQU $07
OBJECT_Y_POSITION_HI        EQU $08
OBJECT_Y_MIN                EQU $09
OBJECT_UNUSED_0A            EQU $0A
OBJECT_UNUSED_0B            EQU $0B
OBJECT_X_VELOCITY_LO        EQU $0C
OBJECT_X_VELOCITY_HI        EQU $0D
OBJECT_X_POSITION_LO        EQU $0E
OBJECT_X_POSITION_HI        EQU $0F
OBJECT_UNUSED_10            EQU $10
OBJECT_X_MAX                EQU $11
OBJECT_BITMAP_PTR_LO        EQU $12
OBJECT_BITMAP_PTR_HI        EQU $13
OBJECT_FUNCGEN_CONTROL      EQU $14
OBJECT_MAGIC_ADDR_LO        EQU $15 ; Function Generator write-window address
OBJECT_MAGIC_ADDR_HI        EQU $16
OBJECT_COLOR                EQU $17
OBJECT_ANIMATION_FRAME      EQU $18

; $0A, $0B and $10 are unused padding.  Every ROM template supplies zero;
; CLEAR_OBJECT_RECORD and whole-record pool clearing/copying preserve zero.
; No constructor, updater, renderer, collision path or indirect field walk
; reads them, and no field write can make them nonzero.
;
; OBJECT_FUNCGEN_CONTROL holds the port-$0C mode.  Constructors seed expand
; and optional flop; MAP_COORDINATES_TO_MAGIC_ADDRESS replaces bits 0-1 with
; the current pixel shift.  OBJECT_MAGIC_ADDR is the saved CPU write-window
; address in $0000-$3FFF, not the display-RAM read address.  OBJECT_COLOR is the
; packed port-$19 expansion pair used whenever the object is drawn.

BITMAP_SOURCE_WIDTH         EQU $00
BITMAP_ROW_COUNT            EQU $01
BITMAP_SOURCE_PIXELS_PER_BYTE EQU $08
BITMAP_LOGICAL_X_UNITS_PER_BYTE EQU $04

; OBJECT_FLAGS bits.  Bits 0-1 are unused by every object path in this ROM.
; Collision state is deliberately split: bit 6 drives the hit animation while
; bit 5 queues scoring/event processing for the foreground TERSE thread.
OBJECT_FLAG_SCORE_OVERLAY   EQU $04            ; bit 2: hit value is visible
OBJECT_FLAG_HIT_SIDE_BIT    EQU $03            ; 0 right, 1 left
OBJECT_FLAG_HIT_SIDE_MASK   EQU $08
OBJECT_FLAG_AT_BOUNDARY     EQU $10            ; bit 4
OBJECT_FLAG_HIT_PENDING     EQU $20            ; bit 5
OBJECT_FLAG_HIT_ANIMATION   EQU $40            ; bit 6
OBJECT_FLAG_ACTIVE          EQU $80            ; bit 7

; Surface-target IDs $00-$05 are ordered by scoring class.  The 1978 Midway
; flyer names the visible target classes; bitmap dimensions distinguish the ROM
; variants.  Mine and torpedo records are not traversed by PROCESS_SHIP_HIT.
OBJECT_TYPE_WARSHIP_A       EQU $00            ; 20x12 logical / 40x12 display
OBJECT_TYPE_WARSHIP_B       EQU $01            ; 16x11 logical / 32x11 display
OBJECT_TYPE_WARSHIP_C       EQU $02            ; 20x10 logical / 40x10 display
OBJECT_TYPE_FREIGHTER_A     EQU $03            ; 16x10 logical / 32x10 display
OBJECT_TYPE_FREIGHTER_B     EQU $04            ; 16x9 logical / 32x9 display
OBJECT_TYPE_PT_BOAT         EQU $05            ; 12x5 logical / 24x5 display
OBJECT_TYPE_MINE            EQU $06            ; 4x16 logical / 8x16 display
OBJECT_TYPE_TORPEDO         EQU $07            ; three perspective frames
OBJECT_TYPE_SUPER_SUB       EQU $08            ; 16x9 logical / 32x9 display

SCORE_FREIGHTER_BCD         EQU $01
SCORE_WARSHIP_BCD           EQU $03
SCORE_PT_BOAT_BCD           EQU $05
SCORE_SUPER_SUB_BCD         EQU $10
SCORE_UNIT_POINTS           EQU $64            ; one stored unit = 100 points
HIT_STREAK_LENGTH           EQU $04
BONUS_DISPLAY_ACTIVE_VALUE  EQU $3C
BONUS_DISPLAY_DURATION      EQU $78

; Collision resolver constants.  Horizontal overlap tests the torpedo's
; right-edge coordinate against a target span expanded by one four-pixel cell.
TORPEDO_COLLISION_X_BIAS        EQU $04
COLLISION_LAST_TARGET_LANE      EQU $02
COLLISION_SOUND_SIDE_RIGHT      EQU $30
COLLISION_SOUND_SIDE_LEFT       EQU $06
COLLISION_SOUND_CLASS_SHIP      EQU $12
COLLISION_SOUND_CLASS_MINE      EQU $24
SHIP_HIT_SOUND_TIMER_LOAD       EQU $40
MINE_HIT_SOUND_TIMER_LOAD       EQU $08
TORPEDO_SOUND_TIMER_LOAD        EQU $38
HIT_ANIMATION_TIMER_LOAD        EQU $06
RETIRED_OBJECT_TIMER_LOAD       EQU $2D
NEW_HIGH_SCORE_BLINK_PERIOD     EQU $1E
; Each station's six-bit lamp latch.  MAME labels bits 0..3 as torpedoes 4..1.
; Bit 4 lights READY; the physical/UI RELOAD indication is its inverse.  Bit 5
; drives the hit/explosion lamp, which the current MAME layout does not draw.
LAMP_TORPEDO_4_MASK             EQU $01
LAMP_TORPEDO_3_MASK             EQU $02
LAMP_TORPEDO_2_MASK             EQU $04
LAMP_TORPEDO_1_MASK             EQU $08
LAMP_TORPEDOES_MASK             EQU $0F
LAMP_READY_MASK                 EQU $10
LAMP_FULL_MAGAZINE_MASK         EQU $1F
LAMP_HIT_MASK                   EQU $20

GAME_CLOCK_DIVIDER_RELOAD       EQU $3C
EXTENDED_SCORE_BASE_BCD         EQU $40

; Raw X-speed bytes are multiplied by four by ACTIVATE_TARGET_IN_LANE to form
; signed 8.8 velocities.  These become $0080, $0100 and $0200 per update.
TARGET_SPEED_SLOW_RAW       EQU $20
TARGET_SPEED_MEDIUM_RAW     EQU $40
TARGET_SPEED_FAST_RAW       EQU $80

; The bitmap symbols are defined at their descriptor bodies in $0E3F-$119E.
; The initial torpedo templates need literal low/high bytes before those
; forward definitions are resolved by zmac.
BITMAP_TORPEDO_NEAR_LO      EQU $2D
BITMAP_TORPEDO_NEAR_HI      EQU $11

; Scheduler pools contain record starts through the inclusive LAST address.
; Torpedo records are interleaved by station; successive records for one
; station are two records ($32 bytes) apart.
TARGET_POOL_BASE            EQU $C000
TARGET_LANE_UPPER_BASE      EQU $C000
TARGET_LANE_LOWER_BASE      EQU $C032
TARGET_LANE_UPPER_Y         EQU $1A
TARGET_LANE_LOWER_Y         EQU $33
TARGET_POOL_LAST            EQU $C04B
TARGET_POOL_COUNT           EQU $04
MINE_POOL_BASE              EQU $C064
MINE_LANE_UPPER_BASE        EQU $C064
MINE_LANE_MIDDLE_BASE       EQU $C096
MINE_LANE_LOWER_BASE        EQU $C0C8
MINE_LANE_UPPER_Y           EQU $4C
MINE_LANE_MIDDLE_Y          EQU $64
MINE_LANE_LOWER_Y           EQU $82
MINE_POOL_LAST              EQU $C0E1
MINE_POOL_COUNT             EQU $06
TORPEDO_POOL_BASE           EQU $C0FA
TORPEDO_POOL_LEFT_BASE      EQU $C0FA
TORPEDO_POOL_RIGHT_BASE     EQU $C113
TORPEDO_POOL_LAST           EQU $C1A9
TORPEDO_POOL_COUNT          EQU $08
TORPEDO_STATION_STRIDE      EQU $32
TORPEDO_AIM_HANDLE_MIN      EQU $12
TORPEDO_AIM_HANDLE_MAX      EQU $31
TORPEDO_TRAJECTORY_ENTRY_COUNT EQU $20
TORPEDO_TRAJECTORY_TABLE_BYTES EQU $40
TORPEDO_TRAJECTORY_OFFSET_MASK EQU $3E
TORPEDO_TRAJECTORY_LAST_OFFSET EQU $3E

TORPEDO_FRAME_NEAR_MIN_Y    EQU $78
TORPEDO_FRAME_MIDDLE_MIN_Y  EQU $46
TORPEDO_FRAME_FAR_MIN_Y     EQU $00

TARGET_SCHEDULER_CURSOR     EQU $C1C2
MINE_SCHEDULER_CURSOR       EQU $C1C4
TORPEDO_SCHEDULER_CURSOR    EQU $C1C6
UNUSED_SCHEDULER_GAP        EQU $C1C8       ; $C1C8-$C1C9, whole-RAM operations only

; Raster-interrupt scheduler state.  Each ROM schedule record is ten bytes:
; scanline, unused byte, colors 4-7, then the IM2 vector and motion-state
; pointers used for the following schedule record.
INTERRUPT_SCHEDULE_CURSOR   EQU $C1FC
INTERRUPT_SCHEDULE_BASE     EQU $C20D
INTERRUPT_BASE_SCANLINE     EQU $C20F
INTERRUPT_MOTION_STATE_PTR  EQU $C210

RASTER_MOTION_STATE_0       EQU $C212
RASTER_MOTION_STATE_1       EQU $C216
RASTER_MOTION_STATE_2       EQU $C21A
RASTER_MOTION_STATE_3       EQU $C21E

RASTER_SCHEDULE_RECORD_SIZE EQU $0A
RASTER_SCHEDULE_END         EQU $FF
RASTER_MOTION_PERIOD        EQU $50
RASTER_RECORD_SCANLINE      EQU $00
RASTER_RECORD_UNUSED_01     EQU $01
RASTER_RECORD_COLOR_4       EQU $02
RASTER_RECORD_COLOR_5       EQU $03
RASTER_RECORD_COLOR_6       EQU $04
RASTER_RECORD_COLOR_7       EQU $05
RASTER_RECORD_HANDLER_LO    EQU $06
RASTER_RECORD_HANDLER_HI    EQU $07
RASTER_RECORD_MOTION_LO     EQU $08
RASTER_RECORD_MOTION_HI     EQU $09

RASTER_MOTION_PHASE_TIMER   EQU $00
RASTER_MOTION_VELOCITY      EQU $01
RASTER_MOTION_OFFSET_LO     EQU $02
RASTER_MOTION_OFFSET_HI     EQU $03

; Frame-timed discrete-sound producers.  The interrupt handler treats every
; nonzero byte as an asserted line and decrements the timers at 60 Hz.
; $C1D0-$C1D5 are packed in reverse address order onto port $40.
SOUND_RIGHT_MINE_HIT_TIMER  EQU $C1D0       ; port $40 bit 5
SOUND_RIGHT_SHIP_HIT_TIMER  EQU $C1D1       ; port $40 bit 4
SOUND_RIGHT_TORPEDO_TIMER   EQU $C1D2       ; port $40 bit 3
SOUND_LEFT_MINE_HIT_TIMER   EQU $C1D3       ; port $40 bit 2
SOUND_LEFT_SHIP_HIT_TIMER   EQU $C1D4       ; port $40 bit 1
SOUND_LEFT_TORPEDO_TIMER    EQU $C1D5       ; port $40 bit 0
COIN_COUNTER_PULSE_TIMER    EQU $C1D6       ; port $41 bit 6
SOUND_LEFT_SONAR_TIMER      EQU $C1D7       ; port $41 bit 5
SOUND_RIGHT_SONAR_TIMER     EQU $C1D8       ; port $41 bit 4
SOUND_DIVE_PAN_TIMER        EQU $C1D9       ; bits 7-5 -> port $41 bits 2-0;
                                                    ; bit 5 also triggers bit 3

FRAME_WORK_GUARD_COUNTER    EQU $C1CA
SOUND_TIMER_BLOCK           EQU $C1CB       ; 16 timers, $C1CB-$C1DA
LEFT_RELOAD_TIMER           EQU $C1CB
RIGHT_RELOAD_TIMER          EQU $C1CC
NEW_HIGH_SCORE_BLINK_TIMER  EQU $C1CC       ; reused while no player is active
SONAR_CADENCE_TIMER         EQU $C1CD
RIGHT_BONUS_DISPLAY_TIMER   EQU $C1CE
LEFT_BONUS_DISPLAY_TIMER    EQU $C1CF
SONAR_PING_COUNT            EQU $C1E1
SOUND_DIVE_PAN_XOR          EQU $C1FA

TERSE_DISPATCH_OPCODE   EQU     $E9FD           ; bytes FD E9: JP (IY)
