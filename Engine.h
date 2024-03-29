#ifndef OnceOnlyEngine_h
#define OnceOnlyEngine_h

// Asteroid Style Game: The engine for holding and updating the game artifacts.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <time.h>
#include <vector>
#include "Objects.h"

namespace Asteroid {
// Game Presets
// Used to calculate size of Kuypier region.
const int KuyperSize = 6;
// Timings in seconds: pause before new life, default text label life, pause before EndGame().
const int RevivePause = 2, DefLabelTime = 2, EndGamePause = 3;
// Maxima: FireCharge(), object speed (controls the game speed), alien speed.
const int MaxCharge = 6, MaxShipSpeed = 14, MaxAlienSpeed = 8;
// Timings in ticks: to half max rock speed, before rocks can blow up, before fire charge increases.
const int HalfMaxTicks = 1500, RockLifeTicks = 25, ReChargeTicks = 12;
// Ship rotate delta per tick.
const double ShipRotateRate = 9.0;
// Probabilities: rock and alien creation at 0.5 HalfMaxTicks; spontaneous rock explosion.
const double RockMakeProb = 0.02, AlienProb = 0.005, RockBreakProb = 0.001;
// Game speed controls: ship thrust factor, ship fire recoil, alien thrust factor and initial rock speed factor.
const double ShipPushMult = 0.25, FireRecoilMult = 0.01, AlienPushMult = 1.0, RockSpeedMult = 0.735;

// The game logic engine and game object roster
// ────────────────────────────────────────────
class Engine {
private:
   std::vector<Asteroid::Thing *> _Objects;
   int _Ticks, _ShipIx, _Lives, _InitRocks;
   int _Score, _ExScore, _HiScore;
   int _Xs, _Ys;
   time_t _NewLifeWait, _EndDemoMark, _EndGameMark;
   bool _Active, _DiedSnd, _AlienSnd;
   TypeT _BoomSnd;
   double _Level;
#if 0
   void _Bury(); //(@) Not used anywhere.
#endif
   void _Empty(bool Now);
   bool _Crash(const Thing &A, const Thing &B) const;
   void _Boing(const Thing &A, const Thing &B, ObjPos &PosA, ObjPos &PosB) const;
   void _StateTick();
   int _Types(TypeT T) const;
   Ship *_GetShip() const;
public:
   Engine();
   virtual ~Engine();
// Add type-T objects to the game.
// For internal use only.
   Thing *AddThing(TypeT T);
   Thing *AddThing(TypeT T, const ObjPos &Pos, const ObjPos &Dir = ObjPos());
   Thing *AddKuypier(TypeT T, int Tick);
// Access internal objects.
// These are needed in order to render objects onto the screen device.
   size_t ObjN() const;
   Thing *ObjAtN(size_t N) const;
// State control.
   bool GetActive() const;
   bool InDemo() const;
   bool InGame() const;
   bool EndGame() const;
   void BegGame(int Rocks = 10);
   void BegDemo(time_t T = 20, int Rocks = 10);
   void Stop();
   void Tick();
   int GetLives() const;
   int GetScore() const;
   int GetExScore() const;
   int GetHiScore() const;
   void SetHiScore(int Score);
   double GetLevel() const;
   void SetLevel(const double &Level);
// The game and ship control input.
   void AddAlienCheat();
   void SetSpin(int Spin);
   void SetPushing(bool Pushing);
   void Fire();
   void ReLoad();
   int Charge() const;
// The sound flags (poll after calling Tick()).
   TypeT GetBoomSnd() const;
   bool GetLanceSnd() const;
   bool GetThrustSnd() const;
   bool GetAlienSnd() const;
   bool GetDiedSnd() const;
// The game playing area methods.
   void GetPlayDims(int *XsP, int *YsP) const;
   void SetPlayDims(int Xs, int Ys);
   int MinDim() const;
};
} // end of namespace Asteroid

#endif // OnceOnly
