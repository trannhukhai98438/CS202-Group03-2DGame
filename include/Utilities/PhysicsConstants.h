#pragma once

// ============================================================
// Physics Constant
// ============================================================
namespace PhysicsConstants {

    constexpr float GRAVITY         = 980.f*1.6;   // px/s²   — downward acceleration //temporary
    constexpr float WALK_SPEED      = 150.f*1.6;   // px/s    — max horizontal walk speed //temporary
    constexpr float ACCELERATION    = 600.f*1.6;   // px/s²   — horizontal acceleration on ground //temporary
    constexpr float FRICTION        = 800.f*1.6;   // px/s²   — horizontal deceleration (Idle/Run) //temporary
    constexpr float SLIDE_FRICTION  = 250.f*1.6;   // px/s²   — deceleration during Slide (~0.6s visible) //temporary
    constexpr float JUMP_FORCE      = -400.f*1.6;  // px/s    — initial vertical velocity on jump //temporary
    constexpr float MAX_FALL_SPEED  = 600.f*1.6;   // px/s    — terminal velocity //temporary
    constexpr float AIR_CONTROL     = 0.65f*1.6;   // ratio   — fraction of ACCELERATION applied in air //temporary
    constexpr float STOP_THRESHOLD  = 5.f*1.6;     // px/s    — velocity below this treated as zero //temporary
    constexpr float SLIDE_THRESHOLD = 60.f*1.6;    // px/s    — min |vel.x| needed to enter Slide //temporary

} // namespace PhysicsConstants
