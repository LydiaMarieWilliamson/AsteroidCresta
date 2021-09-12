// Asteroid Style Game: The objects to hold the state of the game artifacts.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Objects.h"
#include "Engine.h"

using namespace std;
using namespace Asteroid;

#ifdef M_PI
static const double TwoPi = 2.0*M_PI; // 2π
#else
static const double TwoPi = 6.28318530717958648;
#endif

// class Thing: protected methods
// ──────────────────────────────
// The (mean) radius of this object.
double Thing::_SizeUp() const {
   double R = 0.0;
   for (int n = 0; n < _Points; n++) R += abs(_Point[n])/_Points;
   return R;
}

// Add N copies of type T to the list, with the given SpeedUp.
void Thing::_Replicate(TypeT T, int N, const double &SpeedUp/* = 1.0*/) {
   if (N > 0) {
   // Space away the main from the origin moving them in oposite directions.
      ObjPos NDir(_Dir), NPos(_Dir), BDir(_Dir/2.0);
      Thing::RotateVector(NDir, TwoPi/4.0);
      if (NDir != 0.0) NDir *= SpeedUp/abs(NDir);
      if (NPos != 0.0) NPos *= _Radius/abs(NPos), Thing::RotateVector(NPos, TwoPi/4.0);
      double Phi = TwoPi/N;
      for (int n = 0; n < N; n++)
         _Owner->AddThing(T, _Pos + NPos, BDir + NDir), Thing::RotateVector(NDir, Phi), Thing::RotateVector(NPos, Phi);
   }
}

// The Tick()-handler, for default motion and updates.
void Thing::_Tick() {
   if (!_Dead) {
   // Increment the internal tick counter.
   // Check the range just in case the game was left running for several years.
      if (++_Ticks >= 0x7fffffff) _Ticks = 1000;
   // Move and rotate the object.
      _Pos += _Dir, Rotate(_Twist);
   }
}

// class Thing: public methods
// ───────────────────────────
// Make a new Thing object; the base constructor is to be called by derived classes in their constructors.
Thing::Thing(Engine &Owner) {
   _Point = nullptr, _Points = 0, _Dead = false, _Radius = 0.0, _Twist = 0.0, _Ticks = 0, _Owner = &Owner, _Pts = SmallLF;
// The creation time.
   _Now = time(0);
}

// Free the Thing object.
Thing::~Thing() {
   try {
      delete[] _Point;
   } catch(...) { }
}

// Get/set the dead state.
bool Thing::GetDead() const { return _Dead; }
void Thing::SetDead() { _Dead = true; }

// Get the owner.
Engine *Thing::GetOwner() const { return _Owner; }

// Get the radius.
double Thing::GetRadius() const { return _Radius; }

// Rotate the object by Rad radians.
void Thing::Rotate(const double &Rad) {
   if (Rad != 0.0) for (int n = 0; n < _Points; n++) Thing::RotateVector(_Point[n], Rad);
}

// The point count in the rendering.
int Thing::GetPoints() const { return _Points; }

// The point score: always relative to the current position; Points ∈ [0, GetPoints()).
ObjPos Thing::PosPoints(int Points) const { return !_Dead && Points < _Points? _Point[Points] + _Pos: ObjPos(); }

// Get/set the caption.
std::string Thing::GetCaption() const { return _Caption; }
void Thing::SetCaption(const std::string &Caption) { _Caption = Caption; }

// Get/set the font size.
FontT Thing::GetPts() const { return _Pts; }
void Thing::SetPts(FontT Pts) { _Pts = Pts; }

// A uniformly-distributed random number over [0, 1).
double Thing::RandR() { return (double)rand()/RAND_MAX; }

// A random boolean value with probability Prob for ‟true”.
bool Thing::RandB(double Prob/* = 0.5*/) { return rand() < RAND_MAX*Prob; }

// Rotate the vector Pos around the origin by Rad radians, where Rad > 0 means counter-clockwise and Rad < 0 means clockwize.
void Thing::RotateVector(ObjPos &Pos, double Rad) {
   if (Pos != 0.0) {
      Rad += arg(Pos);
      double PosA = abs(Pos);
      Pos = ObjPos(PosA*cos(Rad), PosA*sin(Rad));
   }
}

// Limit the absolute value.
void Thing::LimitAbs(ObjPos &Pos, const double A) {
   double PosA = abs(Pos);
   if (PosA > A && PosA != 0.0) Pos *= A/PosA;
}

// class Rock: protected methods
// ─────────────────────────────
// Create rock points.
// This is a circle with random variation made to the points, sized up/down by Scale.
void Rock::_Sculpt(const double &Scale) {
   _Points = 21, _Point = new ObjPos[_Points];
   const double Vf = 0.25;
   double Alpha = 0.0;
// Rumple it.
   for (int n = 0; n < _Points - 1; n++)
      _Point[n] = ObjPos(20.0*sin(Alpha), 20.0*cos(Alpha)),
      Alpha += TwoPi/(_Points - 1),
      _Point[n] *= Scale,
      _Point[n] += ObjPos(Vf*_Point[n].real()*(2.0*Thing::RandR() - 1.0), Vf*_Point[n].imag()*(2.0*Thing::RandR() - 1.0));
// Connect the final point.
   _Point[_Points - 1] = _Point[0];
// Get the collision radius.
   _Radius = _SizeUp();
}

// class Rock: public methods
// ──────────────────────────
// Make a new Rock object.
Rock::Rock(Engine &Owner): Thing(Owner) { }

// Is it a rock?
bool Rock::Rocky() const { return true; }

// Can it occupy the Kuypier region?
bool Rock::Kuypier() const { return true; }

// Would a collision with Other be fatal?
// Collisions in the Kuypier region are never fatal,
// otherwise a collision with Other rocks is fatal if the combined speed is large and the Other mass is greater than this one.
// Collisions with Lance are always fatal in the game area.
bool Rock::Lethal(const Thing &Other) const {
   if (Other.GetDead()) return false;
   int Xs, Ys; _Owner->GetPlayDims(&Xs, &Ys);
   return
      Other.Type() == LanceOT && _Ticks > 2? true:
      _Pos.real() >= 0 && _Pos.real() <= Xs && _Pos.imag() >= 0 && _Pos.imag() <= Ys &&
      abs(_Dir - Other._Dir) > MaxShipSpeed/5.0 && (Other.Mass() > Mass() || (Other.Mass() == Mass() && Thing::RandB()));
}

// Move and rotate, with a random end of life after a preset time period.
void Rock::Tick() {
   if (!_Dead) {
      _Tick();
   // Random end of life after a preset time period.
      if (Type() != PebbleOT && time(0) - _Now > RockLifeTicks && Thing::RandB(RockBreakProb)) Boom();
   }
}

// class Boulder: public methods
// ─────────────────────────────
// Make a new Boulder object; endowed with a rotation speed of approximately 1 degree per tick.
Boulder::Boulder(Engine &Owner): Rock(Owner) {
   _Twist = TwoPi*1.0/360.0; if (Thing::RandB()) _Twist = -_Twist;
// Create the points.
   _Sculpt(1.0);
}

// The object's score, type, mass and termination routine.
int Boulder::Score() const { return 100; }
TypeT Boulder::Type() const { return BoulderOT; }
double Boulder::Mass() const { return 300; }
void Boulder::Boom() { _Dead = true, _Replicate(StoneOT, 2), _Replicate(SparkOT, 5, 4.0); }

// class Stone: public methods
// ───────────────────────────
// Make a new Stone object; endowed with a rotation speed of approximately 2 degrees per tick.
Stone::Stone(Engine &Owner): Rock(Owner) {
   _Twist = TwoPi*2.0/360.0; if (Thing::RandB()) _Twist = -_Twist;
// Create the points.
   _Sculpt(0.71);
}

// The object's score, type, mass and termination routine.
int Stone::Score() const { return 50; }
TypeT Stone::Type() const { return StoneOT; }
double Stone::Mass() const { return 200; }
void Stone::Boom() { _Dead = true, _Replicate(PebbleOT, 2), _Replicate(SparkOT, 3, 2.0); }

// class Pebble: public methods
// ────────────────────────────
// Make a new Pebble object; endowed with a rotation speed of approximately 4 degrees per tick.
Pebble::Pebble(Engine &Owner): Rock(Owner) {
   _Twist = TwoPi*4.0/360.0; if (Thing::RandB()) _Twist = -_Twist;
// Create the points.
   _Sculpt(0.4);
}

// The object's score, type, mass and termination routine.
int Pebble::Score() const { return 25; }
TypeT Pebble::Type() const { return PebbleOT; }
double Pebble::Mass() const { return 125; }
void Pebble::Boom() { _Dead = true, _Replicate(DebrisOT, 5); }

// class Ship: private methods
// ───────────────────────────
// Reset the points; used in rotation to avoid a build-up of rounding errors.
void Ship::_ResetPoints() {
// Must be _Points number of points.
   _Point[0] = ObjPos(0.0, -10.0), _Point[1] = ObjPos(7.0, 10.0);
   _Point[2] = ObjPos(0.0, 7.0), _Point[3] = ObjPos(-7.0, 10.0), _Point[4] = ObjPos(0.0, -10.0);
// Gun and thrust postions.
   _NosePos = ObjPos(0.0, -14.0); // Forward facing tip.
   _ThrustPos = ObjPos(3.0, 10.0); // A bottom corner.
   _ThrustPlane = ObjPos(-6.0, 0.0); // The line from one bottom point to the other.
}

// class Ship: public methods
// ──────────────────────────
// Make a new Ship object.
Ship::Ship(Engine &Owner): Thing(Owner) {
   _Firing = false, _FireLock = false, _JustFired = false, _FireCharge = MaxCharge;
   _Spin = 0, _Pushing = false, _Orient = TwoPi/8.0;
// Create the points.
   _Points = 5, _Point = new ObjPos[_Points];
// The points are set here.
   _ResetPoints(), Rotate(_Orient);
   Thing::RotateVector(_NosePos, _Orient);
   Thing::RotateVector(_ThrustPos, _Orient);
   Thing::RotateVector(_ThrustPlane, _Orient);
// Get the collision radius.
   _Radius = _SizeUp();
}

// Is it a rock?
bool Ship::Rocky() const { return false; }

// Can it occupy the Kuypier region?
bool Ship::Kuypier() const { return false; }

// Would a collision with Other be fatal?
bool Ship::Lethal(const Thing &Other) const { return !Other.GetDead() && Other.Mass() > Mass(); }

// Move the object, recharge, rotate, thrust and fire.
void Ship::Tick() {
   if (!_Dead) {
      _Tick();
   // Increment the fire charge.
      if (_FireCharge < MaxCharge && _Ticks%ReChargeTicks == 0) _FireCharge++;
   // Spin.
      if (_Spin != 0) {
         if (_Spin == -1) _Orient -= ShipRotateRate*TwoPi/360.0; else _Orient += ShipRotateRate*TwoPi/360.0;
      // Rotate the superstructure, then the nose and thrust plane assemblies.
         _ResetPoints(), Rotate(_Orient);
         Thing::RotateVector(_NosePos, _Orient);
         Thing::RotateVector(_ThrustPos, _Orient);
         Thing::RotateVector(_ThrustPlane, _Orient);
      }
   // Push.
      if (_Pushing) {
      // Thrust vector.
         ObjPos Thrust(sin(_Orient), -cos(_Orient)); Thrust *= ShipPushMult;
      // Exhaust vector.
         ObjPos Expel(Thrust*(-MaxShipSpeed/2.0) + _Dir);
      // Add thrust particles.
         for (int n = 0; n < 2; n++) {
         // Random exhaust exit position.
            ObjPos Smoke(_ThrustPlane*Thing::RandR()); Smoke += _ThrustPos + _Pos;
            Thing *Obj = _Owner->AddThing(ThrustOT, Smoke, Expel); Obj->Rotate(_Orient);
         }
      // Add thrust to the direction, and limit to the maximum speed, so as to avoid catching up with friendly fire.
         _Dir += Thrust, Thing::LimitAbs(_Dir, MaxShipSpeed);
      }
   // Fire (suppress, if not charged).
      if (!_Firing || _FireLock || _FireCharge <= 0) _Firing = false, _JustFired = false;
      else {
      // Create a fire object (initially heading toward the ship).
         ObjPos Fire(sin(_Orient), -cos(_Orient)); Fire *= MaxShipSpeed, Fire += _Dir;
      // Recoil.
         _Dir -= Fire*FireRecoilMult;
      // Bombs away!
         Thing *Obj = _Owner->AddThing(LanceOT, _NosePos + _Pos, Fire); Obj->Rotate(_Orient);
      // Suppress repeated firing.
         _FireLock = true, _JustFired = true, _FireCharge--;
      }
   }
}

// The object's score, type, mass and termination routine.
int Ship::Score() const { return 0; }
TypeT Ship::Type() const { return ShipOT; }
double Ship::Mass() const { return 10; }
void Ship::Boom() { _Dead = true, _Replicate(DebrisOT, 5); }

// Set the rotate state on the next tick: Spin == -1: left, Spin == +1: right, Spin == 0: stop.
void Ship::SetSpin(int Spin) { _Spin = Spin; }

// Get/set the thrust state.
bool Ship::GetPushing() const { return _Pushing; }
void Ship::SetPushing(bool Pushing) { _Pushing = Pushing; }

// Fire on the next tick.
// Each call to Fire() must be followed by a call to ReLoad() in order to release the fire lock.
// This is done to prevent rapid ‟machine gun” firing action, which would make the game too easy.
void Ship::Fire() { _Firing |= !_FireLock; }

// Call to release the fire lock.
// Typically this should be called on a key up action.
void Ship::ReLoad(bool ReSet) {
   _FireLock = false;
   if (ReSet) _Firing = false;
}

// The recent fire state.
bool Ship::JustFired() const { return _JustFired; }

// The fire charge (0 to CHARGEMAX); 0 means the ship cannot fire.
int Ship::FireCharge() const { return _FireCharge; }

// class Alien: private methods
// ────────────────────────────
// The nearest fatal object to the alien or nullptr, if there are no objects.
Thing *Alien::_Neighbor() const {
   Thing *NearO = nullptr; double NearD = -1.0;
// Find the nearest heavier object or ship (avoid the ship).
   for (size_t n = 0; n < _Owner->ObjN(); n++) {
      Thing *Obj = _Owner->ObjAtN(n); double D = abs(_Pos - Obj->_Pos);
      if ((Obj->Mass() > Mass() || Obj->Type() == ShipOT) && (NearD < 0.0 || D < NearD)) NearO = Obj, NearD = D;
   }
   return NearO;
}

// Aim the thrust away from the nearest object.
ObjPos Alien::Push() const {
   ObjPos Pos; Thing *Obj = _Neighbor(); if (Obj != nullptr) Pos = _Pos - Obj->_Pos;
// Thrust more, if there's anything nearby.
   double D = abs(Pos);
   if (D > 0.0) Pos *= 20.0*_Radius/(D*D);
   return AlienPushMult*Pos;
}

// class Alien: public methods
// ───────────────────────────
// Make a new Alien object.
Alien::Alien(Engine &Owner): Thing(Owner) {
// Create the points.
   _Points = 20, _Point = new ObjPos[_Points];
   _Point[0] = ObjPos(5.0, -5.0), _Point[1] = ObjPos(10.0, -2.0);
   _Point[2] = ObjPos(10.0, 2.0), _Point[3] = ObjPos(8.0, 4.0);
   _Point[4] = ObjPos(-2.0, 4.0), _Point[5] = ObjPos(-2.0, 2.0);
   _Point[6] = ObjPos(2.0, 2.0), _Point[7] = ObjPos(2.0, 4.0);
   _Point[8] = ObjPos(-8.0, 4.0), _Point[9] = ObjPos(-10.0, 2.0);
   _Point[10] = ObjPos(-10.0, -2.0), _Point[11] = ObjPos(10.0, -2.0);
   _Point[12] = ObjPos(10.0, 2.0), _Point[13] = ObjPos(-10.0, 2.0);
   _Point[14] = ObjPos(-10.0, -2.0), _Point[15] = ObjPos(-5.0, -5.0);
   _Point[16] = ObjPos(0.0, -5.0), _Point[17] = ObjPos(-7.0, -2.0);
   _Point[18] = ObjPos(-5.0, -5.0), _Point[19] = ObjPos(5.0, -5.0);
// Get the collision radius.
   _Radius = _SizeUp();
}

// Is it a rock?
bool Alien::Rocky() const { return false; }

// Can it occupy the Kuypier region?
bool Alien::Kuypier() const { return true; }

// Would a collision with Other be fatal?
bool Alien::Lethal(const Thing &Other) const {
   if (Other.GetDead()) return false;
   int Xs, Ys; _Owner->GetPlayDims(&Xs, &Ys);
   return
      _Pos.real() >= 0 && _Pos.real() <= Xs && _Pos.imag() >= 0 && _Pos.imag() <= Ys &&
      (Other.Type() == LanceOT || Other.Mass() > Mass());
}

// Move the object.
void Alien::Tick() {
   if (!_Dead) {
      _Tick();
   // Adjust the thrust.
      _Dir += Push(), Thing::LimitAbs(_Dir, MaxAlienSpeed);
   }
}

// The object's score, type, mass and termination routine.
// A big score or an extra life, for the alien.
int Alien::Score() const { return 500; }
TypeT Alien::Type() const { return AlienOT; }
double Alien::Mass() const { return 15; }
void Alien::Boom() { _Dead = true, _Replicate(DebrisOT, 5); }

// class Lance: public methods
// ───────────────────────────
// Make a new Lance object.
Lance::Lance(Engine &Owner): Thing(Owner) {
// Create the points.
   _Points = 2, _Point = new ObjPos[_Points];
   _Point[0] = ObjPos(0.0, -2.0), _Point[1] = ObjPos(0.0, 2.0);
// Get the collision radius.
   _Radius = _SizeUp();
}

// Is it a rock?
bool Lance::Rocky() const { return false; }

// Can it occupy the Kuypier region?
bool Lance::Kuypier() const { return false; }

// Would a collision with Other be fatal?
bool Lance::Lethal(const Thing &Other) const { return !Other.GetDead() && Other.Mass() > 0; }

// Move the object.
void Lance::Tick() {
   if (!_Dead) {
      _Tick();
   // Allow for it to get as much as 3/4 of the way across the screen.
      double Speed = abs(_Dir);
      _Dead = Speed == 0.0 || Speed*_Ticks > 3.0/4.0*_Owner->MinDim();
   }
}

// The object's score, type, mass and termination routine.
// (No explosion on termination: just die.)
int Lance::Score() const { return 0; }
TypeT Lance::Type() const { return LanceOT; }
double Lance::Mass() const { return 1; }
void Lance::Boom() { _Dead = true; }

// class Debris: public methods
// ────────────────────────────
// Make a new Debris object.
Debris::Debris(Engine &Owner): Thing(Owner) {
// Create the points.
   _Points = 4, _Point = new ObjPos[_Points];
   _Point[0] = ObjPos(0.0, 3.0), _Point[1] = ObjPos(3.0, 0.0), _Point[2] = ObjPos(-2.0, -3.0);
// Rumple it.
   const double Vf = 0.2;
   for (int n = 0; n < _Points - 1; n++)
      _Point[n] += ObjPos(Vf*_Point[n].real()*(2.0*Thing::RandR() - 1.0), Vf*_Point[n].imag()*(2.0*Thing::RandR() - 1.0));
// Connect the final point and randomly orient it.
   _Point[_Points - 1] = _Point[0], Rotate(TwoPi*Thing::RandR());
// Endow it with a rotation speed of approximately 8 degrees per tick.
   _Twist = TwoPi*8.0/360.0; if (Thing::RandB()) _Twist = -_Twist;
// Get the collision radius.
   _Radius = _SizeUp();
}

// Is it a rock?
bool Debris::Rocky() const { return false; }

// Can it occupy the Kuypier region?
bool Debris::Kuypier() const { return true; }

// Would a collision with Other be fatal?
bool Debris::Lethal(const Thing &Other) const { return !Other.GetDead() && Other.Type() == LanceOT; }

// Move the object for a short randomly-determined lifetime.
void Debris::Tick() {
   if (!_Dead) {
      _Tick();
      if (time(0) - _Now > 2 && Thing::RandB(0.1)) Boom();
   }
}

// The object's score, type, mass and termination routine.
// Mass 2 is just enough to bounce a ship.
// (No explosion on termination: just die.)
int Debris::Score() const { return 0; }
TypeT Debris::Type() const { return DebrisOT; }
double Debris::Mass() const { return 2; }
void Debris::Boom() { _Dead = true; }

// class Spark: public methods
// ───────────────────────────
// Make a new Spark object: Debris with a much shorter life.
Spark::Spark(Engine &Owner): Debris(Owner) { }

// Move the object for a very short randomly-determined lifetime.
void Spark::Tick() {
   if (!_Dead) {
      _Tick();
      double R = Thing::RandR(); if (R < 0.1 || (time(0) - _Now > 1 && R < 0.5)) Boom();
   }
}

// The object's type.
TypeT Spark::Type() const { return SparkOT; }

// class Thrust: public methods
// ────────────────────────────
// Make a new Thrust object.
Thrust::Thrust(Engine &Owner): Thing(Owner) {
// Create the points.
   _Points = 2, _Point = new ObjPos[_Points];
   _Point[0] = ObjPos(0.0, +1.0), _Point[1] = ObjPos(0.0, -1.0);
// Get the collision radius.
   _Radius = _SizeUp();
}

// Is it a rock?
bool Thrust::Rocky() const { return false; }

// Can it occupy the Kuypier region?
bool Thrust::Kuypier() const { return true; }

// Would a collision with Other be fatal?
bool Thrust::Lethal(const Thing &/*Other*/) const { return false; }

// Move the object for a very limited time, so as to avoid long thrust trails across the screen.
void Thrust::Tick() {
   if (!_Dead) _Tick(), _Dead = _Ticks > 1;
}

// The object's score, type, mass and termination routine.
// Zero mass means transparent.
// (No explosion on termination: just die.)
int Thrust::Score() const { return 0; }
TypeT Thrust::Type() const { return ThrustOT; }
double Thrust::Mass() const { return 0; }
void Thrust::Boom() { _Dead = true; }

// class Label: public methods
// ───────────────────────────
// Make a new Label object: endowed with a lifespan Life in seconds.
Label::Label(Engine &Owner, int Life/* = 2*/): Thing(Owner) { _Life = Life; }

// Get/set the lifespan.
int Label::GetLife() const { return _Life; }
void Label::SetLife(int Life) { _Life = Life; }

// Is it a rock?
bool Label::Rocky() const { return false; }

// Can it occupy the Kuypier region?
bool Label::Kuypier() const { return false; }

// Would a collision with Other be fatal?
bool Label::Lethal(const Thing &/*Other*/) const { return false; }

// Move the object for a limited time.
void Label::Tick() {
   if (!_Dead) _Tick(), _Dead = time(0) - _Now > _Life;
}

// The object's score, type, mass and termination routine.
// Zero mass means transparent.
// (No explosion on termination: just die.)
int Label::Score() const { return 0; }
TypeT Label::Type() const { return LabelOT; }
double Label::Mass() const { return 0; }
void Label::Boom() { _Dead = true; }
