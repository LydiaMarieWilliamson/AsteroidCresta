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
void Engine::_Empty(bool killNow) {
   size_t sz = _Objects.size();

   if (killNow) { // Kill all life in space.
      for (size_t n = 0; n < sz; ++n) {
         delete _Objects[n];
      }

      _Objects.clear();
   } else { // Condemn them for deletion on the next tick.
      for (size_t n = 0; n < sz; ++n) {
         _Objects[n]->SetDead();
      }
   }
}

// Collision-test for o1 and o2: are both alive, with positive mass and closer to each other than their respective sizes?
bool Engine::_Crash(const Thing &o1, const Thing &o2) const {
   return !o1.GetDead() && !o2.GetDead() && o1.Mass() > 0 && o2.Mass() > 0 && abs(o1._Pos - o2._Pos) <= o1.GetRadius() + o2.GetRadius();
}

// Rebound these objects.
void Engine::_Boing(const Thing &o1, const Thing &o2, ObjPos &nd1, ObjPos &nd2) const {
// Intentionally copy the directions, as the result reference may be object directions.
   ObjPos d1(o1._Dir);
   double m1(o1.Mass());
   ObjPos d2(o2._Dir);
   double m2(o2.Mass());
   double m12(m1 + m2);

   if (m12 > 0.0 && abs(o1._Pos - o2._Pos) > abs(o1._Pos - o2._Pos + (d1 - d2)*0.1)) {
      nd1 = (d1*(m1 - m2)/m12) + (d2*2.0*m2/m12);
      Thing::LimitAbs(nd1, MaxShipSpeed);
      nd2 = (d2*(m2 - m1)/m12) + (d1*2.0*m1/m12);
      Thing::LimitAbs(nd2, MaxShipSpeed);
   }
}

#if 1
// itoa() is not a standard part of C++.
// https://www.journaldev.com/40684/itoa-function-c-plus-plus
char *ItoA(int N, char *Buf, int Base) {
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
   _Ticks++;
   if (_Ticks >= 0x7fffffff)
      _Ticks = 1000;

// Set the sound flags to zero; they will be set back to true below, if required.
   _BoomSnd = NoOT;
   _DiedSnd = false;
   _AlienSnd = false;

// Free and remove objects which are now dead from the previous tick.
   for (size_t rn = 0; rn < _Objects.size(); )
      if (_Objects[rn]->GetDead()) {
         delete _Objects[rn];
         _Objects.erase(_Objects.begin() + rn);
      } else {
         ++rn;
      }

// Hold the count, because other objects will be added as parts of explosions.
   size_t sz = _Objects.size();

// Tick each object: get them each to do their thing in the next state tick.
   for (size_t n = 0; n < sz; ++n) {
      _Objects[n]->Tick();
   }

// Collisions: see who collided and sound out their explosions.
// Note that the Tick() calls above may have added objects after the size sz count, but this is OK.
   for (size_t n = 0; n < sz; ++n) for (size_t e = n + 1; e < sz && !_Objects[n]->GetDead(); ++e)
   if (_Crash(*_Objects[n], *_Objects[e])) {
   // When worlds collide!
   // Set rebound in motion.
      _Boing(*_Objects[n], *_Objects[e], _Objects[n]->_Dir, _Objects[e]->_Dir);

   // Was this fatal?
      bool fn = _Objects[n]->Lethal(*_Objects[e]);
      bool fe = _Objects[e]->Lethal(*_Objects[n]);

   // Blow them up.
      if (fn) _Objects[n]->Boom();
      if (fe) _Objects[e]->Boom();

      if (fn || fe) {
      // Something exploded, was it a rock?
      // Set the largest explosion sound, if true.
         if (fn && _Objects[n]->Rocky())
            _BoomSnd = _Objects[n]->Type();
         if (fe && _Objects[e]->Rocky() && (_BoomSnd == NoOT || _Objects[e]->Mass() > _Objects[n]->Mass()))
            _BoomSnd = _Objects[e]->Type();

      // Did our ship explode yet?
         if ((fn && _Objects[n]->Type() == ShipOT) || (fe && _Objects[e]->Type() == ShipOT)) {
         // Oh dear, lost a ship.
            --_Lives;
            _DiedSnd = true;

         // Wait for another ship to arrive or time out at the end of the game.
            _NewLifeWait = time(0) + RevivePause;
         }
      // Set the score and pointer to whatever object may have been shot.
         int as = 0;
         Thing *ak = nullptr;

         if (fe && _Objects[n]->Type() == LanceOT) {
            as = _Objects[e]->Score();
            if (_Objects[e]->Type() == AlienOT)
               ak = _Objects[e];
         } else if (fn && _Objects[e]->Type() == LanceOT) {
            as = _Objects[n]->Score();
            if (_Objects[n]->Type() == AlienOT)
               ak = _Objects[n];
         }
      // Have we shot an alien?
         if (ak != nullptr) {
         // Alien kill: label it.
            Thing *l = AddThing(LabelOT, ak->_Pos, ak->_Dir);
            l->SetPts(SmallLF);

         // Alien sound.
            _AlienSnd = true;

         // Label an extra life or score.
         // A score label generates a warning, when compiled under VC2005.
         // This is OK.
         //(@) Side note: itoa(), which was in the original, is not part of C++, and so has been replaced.
            if (Thing::RandB()) {
               ++_Lives;
               l->SetCaption("EXTRA LIFE");
            } else {
               char s[100];
               ItoA(as, s, 10);
               l->SetCaption(s);
            }
         }
      // Score the kill.
         _Score += as;
      }
   }
// Add a rock to the game, with probability prob.
   double prob = 2.0*_Level*RockMakeProb*(1.0 - 1.0/(1.0 + (double)_Ticks/HalfMaxTicks));

   if (Thing::RandB(prob))
      AddKuypier(BoulderOT, _Ticks);

// Add an alien to the game, with conditional probability prob.
// (Only one alien at a time may be present.)
   prob = AlienProb*(1.0 - 1.0/(1.0 + (double)_Ticks/HalfMaxTicks));

   if (Thing::RandB(prob) && _Types(AlienOT) == 0)
      AddKuypier(AlienOT, 0);

// Wrap the game space: update the size, as it may have changed.
   sz = _Objects.size();

// Check for strays outside the game space.
   for (size_t n = 0; n < sz; ++n) {
   // The Kuypier extra space (rocks and aliens may roam well off the screen).
      int kh = _Xs/KuyperSize;
      int kv = _Ys/KuyperSize;

   // Players are stuck on the screen.
      if (!_Objects[n]->Kuypier()) {
         kh = 0;
         kv = 0;
      }
   // Reset the new position on the other side of the play area.
      ObjPos p(_Objects[n]->_Pos);

      if (p.real() < -kh) p = ObjPos(_Xs + kh, p.imag());
      if (p.imag() < -kv) p = ObjPos(p.real(), _Ys + kv);
      if (p.real() > _Xs + kh) p = ObjPos(-kh, p.imag());
      if (p.imag() > _Ys + kv) p = ObjPos(p.real(), -kv);

      _Objects[n]->_Pos = p;
   }
}

// The number of type t objects.
int Engine::_Types(TypeT t) const {
   int rslt = 0;
   size_t sz = _Objects.size();

   for (size_t n = 0; n < sz; ++n) {
      if (_Objects[n]->Type() == t) {
         ++rslt;
      }
   }

   return rslt;
}

// A pointer to the ship.
Ship *Engine::_GetShip() const {
// We hold the index of the ship for fast lookup as we expect our routines to access the ship object many times.
// We check that our saved indexed indeed holds the ship index, and if not, we search for it.
   size_t sz = _Objects.size();

   if (_ShipIx >= 0 && static_cast<size_t>(_ShipIx) < sz && _Objects[_ShipIx]->Type() == ShipOT) {
   // Here we hold an index to it.
      return static_cast<Ship *>(_Objects[_ShipIx]);
   } else for (size_t n = 0; n < sz; ++n) if (_Objects[n]->Type() == ShipOT) {
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
   _Xs = 535;
   _Ys = 400;

   _ShipIx = -1;
   _Level = 0.5;
   _HiScore = 0;
   _Score = 0;
   _ExScore = 0;
   _Lives = 0;
   _Active = false;
   _Ticks = 0;
   _NewLifeWait = 0;
}

// Free an Engine object.
Engine::~Engine() {
   try {
      size_t sz = _Objects.size();
      for (size_t n = 0; n < sz; ++n) {
         delete _Objects[n];
      }
   } catch(...) { }
}

// Add a type-ot object.
Thing *Engine::AddThing(TypeT ot) {
   Thing *newObj = nullptr;

   switch (ot) {
      case BoulderOT: newObj = new Boulder(*this); break;
      case StoneOT: newObj = new Stone(*this); break;
      case PebbleOT: newObj = new Pebble(*this); break;
      case ShipOT: newObj = new Ship(*this); break;
      case AlienOT: newObj = new Alien(*this); break;
      case LanceOT: newObj = new Lance(*this); break;
      case DebrisOT: newObj = new Debris(*this); break;
      case SparkOT: newObj = new Spark(*this); break;
      case ThrustOT: newObj = new Thrust(*this); break;
      case LabelOT: newObj = new Label(*this, DefLabelTime); break;
      default:
#if 0
         assert(false); // Error.
#endif
      break;
   }

   if (newObj != nullptr) {
      _Objects.push_back(newObj);
   }

   return newObj;
}

// Add a type-ot object at pos with orientation dir.
Thing *Engine::AddThing(TypeT ot, const ObjPos &pos, const ObjPos &dir) {
   Thing *ob = AddThing(ot);
   ob->_Pos = pos;
   ob->_Dir = dir;
   return ob;
}

// Add a randomly-located object into the Kuypier region.
// The velocity may increase statistically according to the difficulty level and value of tick as the game goes on.
Thing *Engine::AddKuypier(TypeT ot, int tick) {
   Thing *ob = AddThing(ot);

// The orientation.
   double sc = 0.5 + MaxShipSpeed*_Level*(1.0 - 1.0/(1.0 + (double)tick/HalfMaxTicks));
   sc *= RockSpeedMult;

   double x = sc*(-1.0 + 2.0*Thing::RandR());
   double y = sc*(-1.0 + 2.0*Thing::RandR());
   ob->_Dir = ObjPos(x, y);

// The position.
   if (Thing::RandB()) {
   // Place it either left or right.
      x = Thing::RandB()? -_Xs/KuyperSize/2: _Xs + _Xs/KuyperSize/2;
      y = Thing::RandR()*(_Ys + 2*_Ys/KuyperSize) - _Ys/KuyperSize;
   } else {
   // Place it either top or bottom.
      x = Thing::RandR()*(_Xs + 2*_Xs/KuyperSize) - _Xs/KuyperSize;
      y = Thing::RandB()? -_Ys/KuyperSize/2: _Ys + _Ys/KuyperSize/2;
   }

   ob->_Pos = ObjPos(x, y);

   return ob;
}

// The object count.
size_t Engine::ObjN() const {
   return _Objects.size();
}

// The object at index n.
Thing *Engine::ObjAtN(size_t n) const {
   return _Objects[n];
}

// The game mode: active versus demo.
bool Engine::GetActive() const {
   return _Active;
}

// Is the game in demo?
bool Engine::InDemo() const {
   return _Active && _EndDemoMark > 0;
}

// Is the game in play?
bool Engine::InGame() const {
   return GetActive() && !InDemo();
}

// Test for ‟GAME OVER” after a short pause of its being set, to allow time for the label to be seen.
bool Engine::EndGame() const {
   return !_Active || (_EndGameMark > 0 && time(0) > _EndGameMark);
}

// Start a new game.
void Engine::BegGame(int rocks) {
   _Empty(true);

   _Ticks = 0;
   _Score = 0;
   _EndGameMark = 0;
   _Active = true;
   _EndDemoMark = 0;
   _InitRocks = rocks;

// Add the start-up label.
   Thing *l = AddThing(LabelOT, ObjPos(_Xs/2, _Ys/2));
   l->SetPts(MediumLF);

   l->SetCaption("NEW GAME");
   _Lives = 3;

// Setting this to non-zero will create new rocks and a ship after a short interval.
   _NewLifeWait = time(0) + RevivePause;
}

// Start a demo game for secs seconds.
// An earlier version had an ‟Aliens” flag, to permit a demo with only flocking aliens.
void Engine::BegDemo(time_t secs, int rocks) {
   _Empty(true);

   _Ticks = 0;
   _Score = 0;
   _EndGameMark = 0;
   _Active = true;
   _EndDemoMark = time(0) + secs;
   _InitRocks = rocks;
   _Lives = 1;

// Add the start-up label.
   Thing *l = AddThing(LabelOT, ObjPos(_Xs/2, _Ys/2));
   l->SetPts(MediumLF);
   l->SetCaption("DEMO");

// Setting this to non-zero will create new rocks and a ship after a short interval.
   _NewLifeWait = time(0) + RevivePause;
}

// Clear: stop the game.
void Engine::Stop() {
   _Active = false;
   _Lives = 0;
   _Empty(true);
}

// The Game State Machine, itself.
// This is meant to be called on the clock, so as to advance the game in 1/10 second increments.
// Graphics should be rendered between calls to Tick().
void Engine::Tick() {
   if (!_Active) return;
   if (InDemo()) {
   // Demo Mode: control the ship randomly.
      Ship *s = _GetShip();

      if (s != nullptr) {
      // Reset the fire lock.
         s->ReLoad(true);

      // One in 3 chance of firing.
         if (Thing::RandB(0.3)) s->Fire();

         if (Thing::RandB(0.1)) {
         // One in 10 chance of changing what ship was doing on last tick.
            s->SetSpin(0);
            s->SetPushing(false);

         // New random action: 1/5 thrust, 3/10 rotate left, 3/10 rotate right, 1/5 do nothing.
            double ra = Thing::RandR();

            if (ra < 0.2)
               s->SetPushing(true);
            else if (ra < 0.5)
               s->SetSpin(-1);
            else if (ra < 0.8)
               s->SetSpin(1);
         }
      }
   }
// Update the game objects.
   _StateTick();

// Deal with high-score and restart events.
   if (!InDemo()) {
   // Update the score records.
      _ExScore = _Score;

      if (_Score > _HiScore)
         _HiScore = _Score;
   }
// Implement the game start/end events.
   bool geflag = false;

// Do we need a new ship or to create initial rocks, etc.
   if (_NewLifeWait > 0 && time(0) > _NewLifeWait) {
   // Avoid repetitions.
      _NewLifeWait = 0;

   // Add a new ship and re-create the initial rocks or end the game if there are no lives left.
      if (_Lives > 0) {
         _Empty(false);

         for (int n = 0; n < _InitRocks; ++n)
            AddKuypier(BoulderOT, 0);

         AddThing(ShipOT, ObjPos(_Xs/2, _Ys/2));
      } else {
         geflag = true;
      }
   }
// Demo timeout.
   geflag |= (InDemo() && time(0) > _EndDemoMark);

   if (geflag && _EndGameMark == 0) {
   // Add the End-Of-Game label.
      _EndGameMark = time(0) + EndGamePause;

      Thing *l = AddThing(LabelOT, ObjPos(_Xs/2, _Ys/2));
      l->SetPts(LargeLF);
      l->SetCaption("GAME OVER");
   }
}

// Get the number of lives.
int Engine::GetLives() const {
   return _Lives;
}

// Get the current score.
int Engine::GetScore() const {
   return _Score;
}

// Get the last game score.
int Engine::GetExScore() const {
   return _ExScore;
}

// Get/set the high score.
// This can be set if the high score is stored between application launches.
int Engine::GetHiScore() const {
   return _HiScore;
}

void Engine::SetHiScore(int hs) {
   _HiScore = hs;
}

// Get/set the game level; higher = more difficult games.
double Engine::GetLevel() const {
   return _Level;
}

void Engine::SetLevel(const double &dif) {
   if (dif > 0.0 && dif <= 1.0)
      _Level = dif;
}

// Cheat: add an alien to the game.
void Engine::AddAlienCheat() {
   if (_Active && _Types(AlienOT) < 15)
      AddKuypier(AlienOT, 0);
}

// Rotate the ship (-1: left, 0: stop, +1: right).
void Engine::SetSpin(int r) {
   Ship *s = _GetShip();
   if (s != nullptr && !InDemo()) s->SetSpin(r);
}

// Thrust-switcher.
void Engine::SetPushing(bool on) {
   Ship *s = _GetShip();
   if (s != nullptr && !InDemo()) s->SetPushing(on);
}

// Fire, release fire and ship fire charge.
void Engine::Fire() {
   Ship *s = _GetShip();
   if (s != nullptr && !InDemo()) s->Fire();
}

void Engine::ReLoad() {
   Ship *s = _GetShip();
   if (s != nullptr && !InDemo()) s->ReLoad(false);
}

int Engine::Charge() const {
   Ship *s = _GetShip();
   return s != nullptr? s->FireCharge(): 0;
}

// Get the latest rock explosion (NoOT = silent), lance, thrust, alien and death sounds.
TypeT Engine::GetBoomSnd() const {
   return _BoomSnd;
}

bool Engine::GetLanceSnd() const {
   Ship *ship = _GetShip();
   return ship != nullptr && ship->JustFired();
}

bool Engine::GetThrustSnd() const {
   Ship *ship = _GetShip();
   return ship != nullptr && ship->GetPushing();
}

bool Engine::GetAlienSnd() const {
   return _AlienSnd;
}

bool Engine::GetDiedSnd() const {
   return _DiedSnd;
}

// Get/set the playing width and height.
// Note:
// ∙	Objects may occupy positions outside this region, and if so, should not be rendered (or DC clipped).
// ∙	The region outside *w and *h is known as the Kuypier area
//	and allows for asteroids to wander naturally into the game area.
// ∙	The ship cannot go into this region.
void Engine::GetPlayDims(int *w, int *h) const {
   if (w != nullptr) *w = _Xs;
   if (h != nullptr) *h = _Ys;
}

void Engine::SetPlayDims(int w, int h) {
   _Xs = w;
   _Ys = h;
}

// The minimum dimension.
int Engine::MinDim() const {
   return _Ys < _Xs? _Ys: _Xs;
}
