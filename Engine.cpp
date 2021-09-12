// Asteroid Style Game: The engine for holding and updating the game artifacts.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <math.h>
#include <stdlib.h>
#include "Engine.h"

using namespace std;
using namespace Asteroid;

// class Engine: private methods
// ─────────────────────────────
// Empty the game of objects.
void Engine::_Empty(bool Now) {
   size_t N = _Objects.size();
   if (Now) { // Kill all life in space.
      for (size_t n = 0; n < N; n++) delete _Objects[n];
      _Objects.clear();
   } else // Condemn them for deletion on the next tick.
      for (size_t n = 0; n < N; n++) _Objects[n]->SetDead();
}

// Collision-test for A and B: are both alive, with positive mass and closer to each other than their respective sizes?
bool Engine::_Crash(const Thing &A, const Thing &B) const {
   return !A.GetDead() && !B.GetDead() && A.Mass() > 0 && B.Mass() > 0 && abs(A._Pos - B._Pos) <= A.GetRadius() + B.GetRadius();
}

// Rebound these objects.
void Engine::_Boing(const Thing &A, const Thing &B, ObjPos &PosA, ObjPos &PosB) const {
// Intentionally copy the directions, as the result reference may be object directions.
   ObjPos DirA(A._Dir); double MassA(A.Mass());
   ObjPos DirB(B._Dir); double MassB(B.Mass());
   double Mass(MassA + MassB);
   if (Mass > 0.0 && abs(A._Pos - B._Pos) > abs(A._Pos - B._Pos + 0.1*(DirA - DirB)))
      PosA = (DirA*(MassA - MassB) + 2.0*DirB*MassB)/Mass, Thing::LimitAbs(PosA, MaxShipSpeed),
      PosB = (DirB*(MassB - MassA) + 2.0*DirA*MassA)/Mass, Thing::LimitAbs(PosB, MaxShipSpeed);
}

#if 1
// itoa() is not a standard part of C++.
// https://www.journaldev.com/40684/itoa-function-c-plus-plus
static char *ItoA(int N, char *Buf, int Base) {
   int Bx = 0;
// The base case.
   if (N == 0) {
      Buf[Bx++] = '0', Buf[Bx] = '\0';
      return Buf;
   }
   int Bs = 0;
// Flip the sign on negative values, after tacking on a '-' sign.
   if (N < 0) Bs++, Buf[Bx++] = '-', N = -N;
   Bs += (int)floor(log(N)/log(Base)) + 1;
// Go through the digits one by one from left to right.
   while (Bx < Bs) {
   // The base exponent; e.g., 10² = 1000, for the third digit.
      int Exp = (int)pow(Base, Bs - 1 - Bx);
   // The base digit.
      int D = N/Exp;
      Buf[Bx++] = D + '0', N -= Exp*D;
   }
   Buf[Bx] = '\0';
   return Buf;
}
#else
#   define ItoA(N, Buf, Base) itoa((N), (Buf), (Base))
#endif

// Update Internal
// ───────────────
// Move the objects, check for collisions, handle rebounds and set the appropriate flags, in that order.
void Engine::_StateTick() {
// Increment the counter.
   if (++_Ticks >= 0x7fffffff) _Ticks = 1000;
// Set the sound flags to zero; they will be set back to true below, if required.
   _BoomSnd = NoOT, _DiedSnd = false, _AlienSnd = false;
// Free and remove objects which are now dead from the previous tick.
   for (size_t N = 0; N < _Objects.size(); )
      if (_Objects[N]->GetDead()) {
         delete _Objects[N]; _Objects.erase(_Objects.begin() + N);
      } else N++;
// Hold the count, because other objects will be added as parts of explosions.
   size_t N = _Objects.size();
// Tick each object: get them each to do their thing in the next state tick.
   for (size_t n = 0; n < N; n++) _Objects[n]->Tick();
// Collisions: see who collided and sound out their explosions.
// Note that the Tick() calls above may have added objects after the size N count, but this is OK.
   for (size_t n0 = 0; n0 < N; n0++) for (size_t n1 = n0 + 1; n1 < N && !_Objects[n0]->GetDead(); n1++)
   if (_Crash(*_Objects[n0], *_Objects[n1])) {
   // When worlds collide!
   // Set rebound in motion.
      _Boing(*_Objects[n0], *_Objects[n1], _Objects[n0]->_Dir, _Objects[n1]->_Dir);
   // Was this fatal?
      bool Lethal0 = _Objects[n0]->Lethal(*_Objects[n1]), Lethal1 = _Objects[n1]->Lethal(*_Objects[n0]);
   // Blow them up.
      if (Lethal0) _Objects[n0]->Boom();
      if (Lethal1) _Objects[n1]->Boom();
      if (Lethal0 || Lethal1) {
      // Something blew up: was it a rock?
      // Set the largest explosion sound, if true.
         if (Lethal0 && _Objects[n0]->Rocky()) _BoomSnd = _Objects[n0]->Type();
         if (Lethal1 && _Objects[n1]->Rocky() && (_BoomSnd == NoOT || _Objects[n1]->Mass() > _Objects[n0]->Mass()))
            _BoomSnd = _Objects[n1]->Type();
      // Did our ship blow up yet?
         if ((Lethal0 && _Objects[n0]->Type() == ShipOT) || (Lethal1 && _Objects[n1]->Type() == ShipOT))
         // Oh dear, lost a ship.
            _Lives--, _DiedSnd = true,
         // Wait for another ship to arrive or time out at the end of the game.
            _NewLifeWait = time(0) + RevivePause;
      // Set the score and pointer to whatever object may have been shot.
         int Sc = 0; Thing *Obj = nullptr;
         if (Lethal1 && _Objects[n0]->Type() == LanceOT) {
            Sc = _Objects[n1]->Score(); if (_Objects[n1]->Type() == AlienOT) Obj = _Objects[n1];
         } else if (Lethal0 && _Objects[n1]->Type() == LanceOT) {
            Sc = _Objects[n0]->Score(); if (_Objects[n0]->Type() == AlienOT) Obj = _Objects[n0];
         }
      // Have we shot an alien?
         if (Obj != nullptr) {
         // Alien kill: label it.
            Thing *Lab = AddThing(LabelOT, Obj->_Pos, Obj->_Dir); Lab->SetPts(SmallLF);
         // Alien sound.
            _AlienSnd = true;
         // Label an extra life or score.
         // A score label generates a warning, when compiled under VC2005.
         // This is OK.
         //(@) Side note: itoa(), which was in the original, is not part of C++, and so has been replaced.
            if (Thing::RandB()) _Lives++, Lab->SetCaption("EXTRA LIFE");
            else {
               char Cap[100]; ItoA(Sc, Cap, 10), Lab->SetCaption(Cap);
            }
         }
      // Score the kill.
         _Score += Sc;
      }
   }
// Add a rock to the game, with probability Prob.
   double Prob = 2.0*_Level*RockMakeProb*(1.0 - 1.0/(1.0 + (double)_Ticks/HalfMaxTicks));
   if (Thing::RandB(Prob)) AddKuypier(BoulderOT, _Ticks);
// Add an alien to the game, with conditional probability Prob.
// (Only one alien at a time may be present.)
   Prob = AlienProb*(1.0 - 1.0/(1.0 + (double)_Ticks/HalfMaxTicks));
   if (Thing::RandB(Prob) && _Types(AlienOT) == 0) AddKuypier(AlienOT, 0);
// Wrap the game space: update the size, as it may have changed.
   N = _Objects.size();
// Check for strays outside the game space.
   for (size_t n = 0; n < N; n++) {
   // The Kuypier extra space (rocks and aliens may roam well off the screen).
      int X = _Xs/KuyperSize, Y = _Ys/KuyperSize;
   // Players are stuck on the screen.
      if (!_Objects[n]->Kuypier()) X = 0, Y = 0;
   // Reset the new position on the other side of the play area.
      ObjPos Pos(_Objects[n]->_Pos);
      if (Pos.real() < -X) Pos = ObjPos(_Xs + X, Pos.imag());
      if (Pos.imag() < -Y) Pos = ObjPos(Pos.real(), _Ys + Y);
      if (Pos.real() > _Xs + X) Pos = ObjPos(-X, Pos.imag());
      if (Pos.imag() > _Ys + Y) Pos = ObjPos(Pos.real(), -Y);
      _Objects[n]->_Pos = Pos;
   }
}

// The number of type T objects.
int Engine::_Types(TypeT T) const {
   int Ts = 0;
   size_t N = _Objects.size();
   for (size_t n = 0; n < N; n++) if (_Objects[n]->Type() == T) Ts++;
   return Ts;
}

// A pointer to the ship.
Ship *Engine::_GetShip() const {
// We hold the index of the ship for fast lookup as we expect our routines to access the ship object many times.
// We check that our saved indexed indeed holds the ship index, and if not, we search for it.
   size_t N = _Objects.size();
   if (_ShipIx >= 0 && static_cast<size_t>(_ShipIx) < N && _Objects[_ShipIx]->Type() == ShipOT)
   // Here we hold an index to it.
      return static_cast<Ship *>(_Objects[_ShipIx]);
   else for (size_t n = 0; n < N; n++) if (_Objects[n]->Type() == ShipOT) {
   // Allow const method to set the property.
   // This is for optimization only, and I don't regard it as modifying the state of the object (much).
      *const_cast<int *>(&_ShipIx) = n;
      return static_cast<Ship *>(_Objects[n]);
   }
   return nullptr;
}

// class Engine: public methods
// ────────────────────────────
// Make a new Engine object.
Engine::Engine() {
// The default playing area.
// See the playing area accessors for more information.
   _Xs = 535, _Ys = 400;
   _ShipIx = -1, _Level = 0.5;
   _HiScore = 0, _Score = 0, _ExScore = 0, _Lives = 0;
   _Active = false, _Ticks = 0, _NewLifeWait = 0;
}

// Free an Engine object.
Engine::~Engine() {
   try {
      size_t N = _Objects.size();
      for (size_t n = 0; n < N; n++) delete _Objects[n];
   } catch(...) { }
}

// Add a type-T object.
Thing *Engine::AddThing(TypeT T) {
   Thing *Obj = nullptr;
   switch (T) {
      case BoulderOT: Obj = new Boulder(*this); break;
      case StoneOT: Obj = new Stone(*this); break;
      case PebbleOT: Obj = new Pebble(*this); break;
      case ShipOT: Obj = new Ship(*this); break;
      case AlienOT: Obj = new Alien(*this); break;
      case LanceOT: Obj = new Lance(*this); break;
      case DebrisOT: Obj = new Debris(*this); break;
      case SparkOT: Obj = new Spark(*this); break;
      case ThrustOT: Obj = new Thrust(*this); break;
      case LabelOT: Obj = new Label(*this, DefLabelTime); break;
      default:
#if 0
         assert(false); // Error.
#endif
      break;
   }
   if (Obj != nullptr) _Objects.push_back(Obj);
   return Obj;
}

// Add a type-T object at Pos, with orientation Dir.
Thing *Engine::AddThing(TypeT T, const ObjPos &Pos, const ObjPos &Dir) {
   Thing *Obj = AddThing(T); Obj->_Pos = Pos, Obj->_Dir = Dir;
   return Obj;
}

// Add a randomly-located object into the Kuypier region.
// The velocity may increase statistically according to the difficulty level and value of Tick as the game goes on.
Thing *Engine::AddKuypier(TypeT T, int Tick) {
   Thing *Obj = AddThing(T);
// The orientation.
   double Speed = RockSpeedMult*(0.5 + MaxShipSpeed*_Level*(1.0 - 1.0/(1.0 + (double)Tick/HalfMaxTicks)));
   Obj->_Dir = ObjPos(Speed*(-1.0 + 2.0*Thing::RandR()), Speed*(-1.0 + 2.0*Thing::RandR()));
// The position.
   Obj->_Pos = Thing::RandB()?
   // Place it either left or right.
      ObjPos(Thing::RandB()? -_Xs/KuyperSize/2: _Xs + _Xs/KuyperSize/2, Thing::RandR()*(_Ys + 2*_Ys/KuyperSize) - _Ys/KuyperSize):
   // Place it either top or bottom.
      ObjPos(Thing::RandR()*(_Xs + 2*_Xs/KuyperSize) - _Xs/KuyperSize, Thing::RandB()? -_Ys/KuyperSize/2: _Ys + _Ys/KuyperSize/2);
   return Obj;
}

// The object count.
size_t Engine::ObjN() const { return _Objects.size(); }

// The object at index N.
Thing *Engine::ObjAtN(size_t N) const { return _Objects[N]; }

// The game mode: active versus demo.
bool Engine::GetActive() const { return _Active; }

// Is the game in demo?
bool Engine::InDemo() const { return _Active && _EndDemoMark > 0; }

// Is the game in play?
bool Engine::InGame() const { return _Active && !InDemo(); }

// Test for ‟GAME OVER” after a short pause of its being set, to allow time for the label to be seen.
bool Engine::EndGame() const { return !_Active || (_EndGameMark > 0 && time(0) > _EndGameMark); }

// Start a new game.
void Engine::BegGame(int Rocks/* = 10*/) {
   _Empty(true);
   _Ticks = 0, _Score = 0, _EndGameMark = 0, _Active = true, _EndDemoMark = 0, _InitRocks = Rocks, _Lives = 3;
// Add the start-up label.
   Thing *Lab = AddThing(LabelOT, ObjPos(_Xs/2, _Ys/2)); Lab->SetPts(MediumLF), Lab->SetCaption("NEW GAME");
// Setting this to non-zero will create new rocks and a ship after a short interval.
   _NewLifeWait = time(0) + RevivePause;
}

// Start a demo game for T seconds.
// An earlier version had an ‟Aliens” flag, to permit a demo with only flocking aliens.
void Engine::BegDemo(time_t T/* = 20*/, int Rocks/* = 10*/) {
   _Empty(true);
   _Ticks = 0, _Score = 0, _EndGameMark = 0, _Active = true, _EndDemoMark = time(0) + T, _InitRocks = Rocks, _Lives = 1;
// Add the start-up label.
   Thing *Lab = AddThing(LabelOT, ObjPos(_Xs/2, _Ys/2)); Lab->SetPts(MediumLF), Lab->SetCaption("DEMO");
// Setting this to non-zero will create new rocks and a ship after a short interval.
   _NewLifeWait = time(0) + RevivePause;
}

// Clear: stop the game.
void Engine::Stop() { _Active = false, _Lives = 0, _Empty(true); }

// The Game State Machine, itself.
// This is meant to be called on the clock, so as to advance the game in 1/10 second increments.
// Graphics should be rendered between calls to Tick().
void Engine::Tick() {
   if (!_Active) return;
   if (InDemo()) {
   // Demo Mode: control the ship randomly.
      Ship *Sh = _GetShip();
      if (Sh != nullptr) {
      // Reset the fire lock.
         Sh->ReLoad(true);
      // One in 3 chance of firing.
         if (Thing::RandB(0.3)) Sh->Fire();
         if (Thing::RandB(0.1)) {
         // One in 10 chance of changing what ship was doing on last tick.
            Sh->SetSpin(0), Sh->SetPushing(false);
         // New random action: 1/5 thrust, 3/10 rotate left, 3/10 rotate right, 1/5 do nothing.
            double Act = Thing::RandR();
            if (Act < 0.2) Sh->SetPushing(true);
            else if (Act < 0.5) Sh->SetSpin(-1);
            else if (Act < 0.8) Sh->SetSpin(+1);
         }
      }
   }
// Update the game objects.
   _StateTick();
// Deal with high-score and restart events.
   if (!InDemo()) {
   // Update the score records.
      _ExScore = _Score; if (_Score > _HiScore) _HiScore = _Score;
   }
// Implement the game start/end events.
   bool Ended = false;
// Do we need a new ship or to create initial rocks, etc.
   if (_NewLifeWait > 0 && time(0) > _NewLifeWait) {
   // Avoid repetitions.
      _NewLifeWait = 0;
   // Add a new ship and re-create the initial rocks or end the game if there are no lives left.
      if (_Lives > 0) {
         _Empty(false);
         for (int n = 0; n < _InitRocks; n++) AddKuypier(BoulderOT, 0);
         AddThing(ShipOT, ObjPos(_Xs/2, _Ys/2));
      } else Ended = true;
   }
// Demo timeout.
   Ended |= InDemo() && time(0) > _EndDemoMark;
   if (Ended && _EndGameMark == 0) {
   // Add the End-Of-Game label.
      _EndGameMark = time(0) + EndGamePause;
      Thing *Lab = AddThing(LabelOT, ObjPos(_Xs/2, _Ys/2)); Lab->SetPts(LargeLF), Lab->SetCaption("GAME OVER");
   }
}

// Get the number of lives.
int Engine::GetLives() const { return _Lives; }

// Get the current score.
int Engine::GetScore() const { return _Score; }

// Get the last game score.
int Engine::GetExScore() const { return _ExScore; }

// Get/set the high score.
// This can be set if the high score is stored between application launches.
int Engine::GetHiScore() const { return _HiScore; }
void Engine::SetHiScore(int Score) { _HiScore = Score; }

// Get/set the game level; higher = more difficult games.
double Engine::GetLevel() const { return _Level; }
void Engine::SetLevel(const double &Level) {
   if (Level > 0.0 && Level <= 1.0) _Level = Level;
}

// Cheat: add an alien to the game.
void Engine::AddAlienCheat() {
   if (_Active && _Types(AlienOT) < 15) AddKuypier(AlienOT, 0);
}

// Rotate the ship (Spin == -1: left, Spin == 0: stop, Spin == +1: right).
void Engine::SetSpin(int Spin) {
   Ship *Sh = _GetShip();
   if (Sh != nullptr && !InDemo()) Sh->SetSpin(Spin);
}

// Thrust-switcher.
void Engine::SetPushing(bool Pushing) {
   Ship *Sh = _GetShip();
   if (Sh != nullptr && !InDemo()) Sh->SetPushing(Pushing);
}

// Fire, release fire and ship fire charge.
void Engine::Fire() {
   Ship *Sh = _GetShip();
   if (Sh != nullptr && !InDemo()) Sh->Fire();
}
void Engine::ReLoad() {
   Ship *Sh = _GetShip();
   if (Sh != nullptr && !InDemo()) Sh->ReLoad(false);
}
int Engine::Charge() const {
   Ship *Sh = _GetShip();
   return Sh != nullptr? Sh->FireCharge(): 0;
}

// Get the latest rock explosion (NoOT = silent), lance, thrust, alien and death sounds.
TypeT Engine::GetBoomSnd() const { return _BoomSnd; }
bool Engine::GetLanceSnd() const {
   Ship *Sh = _GetShip();
   return Sh != nullptr && Sh->JustFired();
}
bool Engine::GetThrustSnd() const {
   Ship *Sh = _GetShip();
   return Sh != nullptr && Sh->GetPushing();
}
bool Engine::GetAlienSnd() const { return _AlienSnd; }
bool Engine::GetDiedSnd() const { return _DiedSnd; }

// Get/set the playing width and height.
// Note:
// ∙	Objects may occupy positions outside this region, and if so, should not be rendered (or DC clipped).
// ∙	The region outside *XsP and *YsP is known as the Kuypier area
//	and allows for asteroids to wander naturally into the game area.
// ∙	The ship cannot go into this region.
void Engine::GetPlayDims(int *XsP, int *YsP) const {
   if (XsP != nullptr) *XsP = _Xs;
   if (YsP != nullptr) *YsP = _Ys;
}

void Engine::SetPlayDims(int Xs, int Ys) { _Xs = Xs, _Ys = Ys; }

// The minimum dimension.
int Engine::MinDim() const { return _Ys < _Xs? _Ys: _Xs; }
