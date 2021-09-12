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
const double TwoPi = 2.0*M_PI; // 2π
#else
const double TwoPi = 6.28318530717958648;
#endif

// class Obj: protected methods
// ────────────────────────────
// The (mean) radius of this object.
double Obj::m_calcRad() const {
   double rslt = 0.0;

   for (int n = 0; n < m_pointCnt; ++n)
      rslt += abs(m_points[n])/m_pointCnt;

   return rslt;
}

// Add cnt copies of type t to the list, with the given speed factor sf.
void Obj::m_fragment(OType t, int cnt, const double &sf) {
   if (cnt > 0) {
   // Space away the main from the origin moving them in oposite directions.
      ObjPos ndir(dir), npos(dir), bdir(dir/2.0);

      Obj::rotateVector(ndir, TwoPi/4.0);
      if (ndir != 0.0) ndir *= sf/abs(ndir);

      if (npos != 0.0) {
         npos *= m_radius/abs(npos);
         Obj::rotateVector(npos, TwoPi/4.0);
      }

      double dr = TwoPi/cnt;
      for (int n = 0; n < cnt; ++n) {
         mp_owner->AddThing(t, pos + npos, bdir + ndir);
         Obj::rotateVector(ndir, dr);
         Obj::rotateVector(npos, dr);
      }
   }
}

// The Tick()-handler, for default motion and updates.
void Obj::m_internalTick() {
   if (!m_dead) {
   // Increment the internal tick counter.
      ++_Ticks;

   // Check the range just in case the game was left running for several years.
      if (_Ticks == 0x7FFFFFFF)
         _Ticks = 1000;

   // Move and rotate the object.
      pos += dir;

      SetSpin(m_rotDelta);
   }
}

// class Obj: public methods
// ─────────────────────────
// Make a new Obj object; the base constructor is to be called by derived classes in their constructors.
Obj::Obj(Engine &owner) {
   m_points = nullptr;
   m_pointCnt = 0;
   m_dead = false;
   m_radius = 0.0;
   m_rotDelta = 0.0;
   _Ticks = 0;
   mp_owner = &owner;
   m_fontSize = lfSmall;

// The creation time.
   m_tm = time(0);
}

// Free the Obj object.
Obj::~Obj() {
   try {
      delete[] m_points;
   } catch(...) { }
}

// Get/set the dead state.
bool Obj::dead() const {
   return m_dead;
}

void Obj::die() {
   m_dead = true;
}

// Get the owner.
Engine *Obj::owner() const {
   return mp_owner;
}

// Get the radius.
double Obj::radius() const {
   return m_radius;
}

// Rotate the object by a radians.
void Obj::SetSpin(const double &a) {
   if (a != 0.0) {
      for (int n = 0; n < m_pointCnt; ++n)
         Obj::rotateVector(m_points[n], a);
   }
}

// The point count in the rendering.
int Obj::pointCnt() const {
   return m_pointCnt;
}

// The point score: always relative to the current positione; idx ∈ [0, pointCnt()).
ObjPos Obj::points(int idx) const {
   if (!m_dead && idx < m_pointCnt)
      return m_points[idx] + pos;
   else
      return ObjPos();
}

// Get/set the caption.
std::string Obj::caption() const {
   return m_caption;
}

void Obj::caption(const std::string &s) {
   m_caption = s;
}

// Get/set the font size.
LFSize Obj::fontSize() const {
   return m_fontSize;
}

void Obj::fontSize(LFSize sz) {
   m_fontSize = sz;
}

// A uniformly-distributed random number over [0, 1).
double Obj::randFloat() {
   return (double)rand()/RAND_MAX;
}

// Rotate the vector p around the origin by a radians, where a > 0 means counter-clockwise and a < 0 means clockwize.
void Obj::rotateVector(ObjPos &p, double a) {
   if (p != 0.0) {
      a += arg(p);
      double h = abs(p);
      p = ObjPos(h*cos(a), h*sin(a));
   }
}

// Limit the absolute value.
void Obj::limitAbs(ObjPos &p, const double a) {
   double b = abs(p);
   if (b > a && b != 0.0) p *= a/b;
}

// class RockBase: Protected Methods
// ─────────────────────────────────
// Create rock points.
// This is a circle with random variation made to the points, sized up/down by scale.
void RockBase::m_createPoints(const double &scale) {
   m_pointCnt = 21;
   m_points = new ObjPos[m_pointCnt];

   const double vf = 0.25;
   double x, y, alpha = 0.0;
// Rumple it.
   for (int n = 0; n < m_pointCnt - 1; ++n) {
      m_points[n] = ObjPos(20.0*sin(alpha), 20.0*cos(alpha));
      alpha += TwoPi/(m_pointCnt - 1);
      m_points[n] *= scale;

      x = m_points[n].real();
      x = vf*x*(2.0*Obj::randFloat() - 1.0);
      y = m_points[n].imag();
      y = vf*y*(2.0*Obj::randFloat() - 1.0);
      m_points[n] += ObjPos(x, y);
   }

// Connect the final point.
   m_points[m_pointCnt - 1] = m_points[0];

// Get the collision radius.
   m_radius = m_calcRad();
}

// class RockBase: public methods
// ──────────────────────────────
// Make a new RockBase object.
RockBase::RockBase(Engine &owner): Obj(owner) {
}

// Is it a rock?
bool RockBase::rock() const {
   return true;
}

// Can it occupy the Kuypier region?
bool RockBase::kuypier() const {
   return true;
}

// Would a collision with other be fatal?
// Collisions in the Kuypier region are never fatal,
// otherwise a collision with other rocks is fatal if the combined speed is large and the other mass is greater than this one.
// Collisions with fire are always fatal in the game area.
bool RockBase::fatal(const Obj &other) const {
   if (other.dead()) return false;
   int w, h;
   mp_owner->GetPlayDims(&w, &h);
   return
      other.type() == otFire && _Ticks > 2? true:
      pos.real() < 0 || pos.real() > w || pos.imag() < 0 || pos.imag() > h? false:
      abs(dir - other.dir) > MaxShipSpeed/5.0 && (other.mass() > mass() || (other.mass() == mass() && Obj::randFloat() < 0.5));
}

// Move and rotate, with a random end of life after a preset time period.
void RockBase::Tick() {
   if (!m_dead) {
      m_internalTick();

   // Random end of life after a preset time period.
      if (type() != otSmallRock && time(0) - m_tm > RockLifeTicks && Obj::randFloat() < RockBreakProb) {
         explode();
      }
   }
}

// class BigRock: public methods
// ─────────────────────────────
// Make a new BigRock object; endowed with a rotation speed of approximately 1 degree per tick.
BigRock::BigRock(Engine &owner): RockBase(owner) {
   m_rotDelta = TwoPi*1.0/360.0;
   if (Obj::randFloat() < 0.5) m_rotDelta = -m_rotDelta;

// Create the points.
   m_createPoints(1.0);
}

// The object's score, type, mass and termination routine.
int BigRock::Score() const {
   return 100;
}

OType BigRock::type() const {
   return otBigRock;
}

double BigRock::mass() const {
   return 300;
}

void BigRock::explode() {
   m_dead = true;
   m_fragment(otMedRock, 2);
   m_fragment(otSpark, 5, 4.0);
}

// class MedRock: public methods
// ─────────────────────────────
// Make a new MedRock object; endowed with a rotation speed of approximately 2 degrees per tick.
MedRock::MedRock(Engine &owner): RockBase(owner) {
   m_rotDelta = TwoPi*2.0/360.0;
   if (Obj::randFloat() < 0.5) m_rotDelta = -m_rotDelta;

// Create the points.
   m_createPoints(0.71);
}

// The object's score, type, mass and termination routine.
int MedRock::Score() const {
   return 50;
}

OType MedRock::type() const {
   return otMedRock;
}

double MedRock::mass() const {
   return 200;
}

void MedRock::explode() {
   m_dead = true;
   m_fragment(otSmallRock, 2);
   m_fragment(otSpark, 3, 2.0);
}

// class SmallRock: public methods
// ───────────────────────────────
// Make a new SmallRock object; endowed with a rotation speed of approximately 4 degrees per tick.
SmallRock::SmallRock(Engine &owner): RockBase(owner) {
   m_rotDelta = TwoPi*4.0/360.0;
   if (Obj::randFloat() < 0.5) m_rotDelta = -m_rotDelta;

// Create the points.
   m_createPoints(0.4);
}

// The object's score, type, mass and termination routine.
int SmallRock::Score() const {
   return 25;
}

OType SmallRock::type() const {
   return otSmallRock;
}

double SmallRock::mass() const {
   return 125;
}

void SmallRock::explode() {
   m_dead = true;
   m_fragment(otDebris, 5);
}

// class Ship: private methods
// ───────────────────────────
// Reset the points; used in rotation to avoid a build-up of rounding errors.
void Ship::m_resetPoints() {

// Must be m_pointCnt number of points.
   m_points[0] = ObjPos(0.0, -10.0);
   m_points[1] = ObjPos(7.0, 10.0);
   m_points[2] = ObjPos(0, 7.0);
   m_points[3] = ObjPos(-7, 10.0);
   m_points[4] = ObjPos(0.0, -10.0);

// Gun and thrust postions.
   m_nosePos = ObjPos(0.0, -14.0); // Forward facing tip.
   m_thrustPos = ObjPos(3.0, 10.0); // A bottom corner
   m_thrustPlane = ObjPos(-6.0, 0.0); // The line from one bottom point to the other.
}

// class Ship: public methods
// ──────────────────────────
// Make a new Ship object.
Ship::Ship(Engine &owner): Obj(owner) {
   m_fire = false;
   m_fireLock = false;
   m_justFired = false;
   m_fireCharge = MaxCharge;
   m_rotDir = 0;
   m_thrust = false;
   m_alpha = TwoPi/8.0;

// Create the points.
   m_pointCnt = 5;
   m_points = new ObjPos[m_pointCnt];

// The points are set here.
   m_resetPoints();
   SetSpin(m_alpha);
   Obj::rotateVector(m_nosePos, m_alpha);
   Obj::rotateVector(m_thrustPos, m_alpha);
   Obj::rotateVector(m_thrustPlane, m_alpha);

// Get the collision radius.
   m_radius = m_calcRad();
}

// Is it a rock?
bool Ship::rock() const {
   return false;
}

// Can it occupy the Kuypier region?
bool Ship::kuypier() const {
   return false;
}

// Would a collision with other be fatal?
bool Ship::fatal(const Obj &other) const {
   return (!other.dead() && other.mass() > mass());
}

// Move the object, recharge, rotate, thrust and fire.
void Ship::Tick() {
   if (!m_dead) {
      m_internalTick();

   // Increment the fire charge.
      if (m_fireCharge < MaxCharge && _Ticks%ReChargeTicks == 0) {
         ++m_fireCharge;
      }
   // Spin.
      if (m_rotDir != 0) {
         if (m_rotDir == -1)
            m_alpha -= ShipRotateRate*TwoPi/360.0;
         else
            m_alpha += ShipRotateRate*TwoPi/360.0;

      // Rotate the superstructure, then the nose and thrust plane assemblies.
         m_resetPoints();
         SetSpin(m_alpha);

         Obj::rotateVector(m_nosePos, m_alpha);
         Obj::rotateVector(m_thrustPos, m_alpha);
         Obj::rotateVector(m_thrustPlane, m_alpha);
      }
   // Push.
      if (m_thrust) {
      // Thrust vector.
         ObjPos tvect(sin(m_alpha), -cos(m_alpha));
         tvect *= ShipPushMult;

      // Exhaust vector.
         ObjPos exv(tvect*(-MaxShipSpeed/2.0) + dir);

      // Add thrust particles.
         for (int n = 0; n < 2; ++n) {
         // Random exhaust exit position.
            ObjPos tpos(m_thrustPlane*Obj::randFloat());
            tpos += m_thrustPos + pos;

            Obj *f = mp_owner->AddThing(otThrust, tpos, exv);
            f->SetSpin(m_alpha);
         }

      // Add thrust to the direction, and limit to the maximum speed, so as to avoid catching up with friendly fire.
         dir += tvect;
         Obj::limitAbs(dir, MaxShipSpeed);
      }
   // Fire (suppress, if not charged).
      if (m_fire && !m_fireLock && m_fireCharge > 0) {
      // Create a fire object (initially heading toward the ship).
         ObjPos fvect(sin(m_alpha), -cos(m_alpha));
         fvect *= MaxShipSpeed;
         fvect += dir;

      // Recoil.
         dir -= fvect*FireRecoilMult;

      // Bombs away!
         Obj *f = mp_owner->AddThing(otFire, m_nosePos + pos, fvect);
         f->SetSpin(m_alpha);

      // Suppress repeated firing.
         m_fireLock = true;
         m_justFired = true;
         m_fireCharge--;
      } else {
         m_fire = false;
         m_justFired = false;
      }
   }
}

// The object's score, type, mass and termination routine.
int Ship::Score() const {
   return 0;
}

OType Ship::type() const {
   return otShip;
}

double Ship::mass() const {
   return 10;
}

void Ship::explode() {
   m_dead = true;
   m_fragment(otDebris, 5);
}

// Set the rotate state on the next tick: r == -1 left, r == +1 right, r == 0 stop.
void Ship::rot(int r) {
   m_rotDir = r;
}

// Set/get the thrust state.
void Ship::SetPushing(bool on) {
   m_thrust = on;
}

bool Ship::thrusting() const {
   return m_thrust;
}

// Fire on the next tick.
// Each call to fire() must be followed by a call to ReLoad() in order to release the fire lock.
// This is done to prevent rapid ‟machine gun” firing action, which would make the game too easy.
void Ship::Fire() {
   m_fire |= !m_fireLock;
}

// Call to release the fire lock.
// Typically this should be called on a key up action.
void Ship::ReLoad(bool reset) {
   m_fireLock = false;
   if (reset) m_fire = false;
}

// The recent fire state.
bool Ship::justFired() const {
   return m_justFired;
}

// The fire charge (0 to CHARGEMAX); 0 means the ship cannot fire.
int Ship::fireCharge() const {
   return m_fireCharge;
}

// class Alien: private methods
// ────────────────────────────
// The nearest fatal object to the alien or nullptr, if there are no objects.
Obj *Alien::m_nearObj() const {
   Obj *rslt = nullptr;
   ObjPos ndv;
   double oabs, nabs = -1.0;

// Find the nearest heavier object or ship (avoid the ship).
   for (size_t n = 0; n < mp_owner->ObjN(); ++n) {
      Obj *objn = mp_owner->ObjAtN(n);
      OType ot = objn->type();
      oabs = abs(pos - objn->pos);

      if ((objn->mass() > mass() || ot == otShip) && (nabs < 0.0 || oabs < nabs)) {
         rslt = objn;
         nabs = oabs;
      }
   }

   return rslt;
}

// Aim the thrust away from the nearest object.
ObjPos Alien::Push() const {
   ObjPos rslt;
   Obj *obj = m_nearObj();
   if (obj != nullptr) rslt = pos - obj->pos;

// Thrust more, if there's anything nearby.
   double nabs = abs(rslt);

   if (nabs > 0.0) {
      rslt *= 20.0*m_radius/(nabs*nabs);
   }

   rslt *= AlienPushMult;

   return rslt;
}

// class Alien: public methods
// ───────────────────────────
// Make a new Alien object.
Alien::Alien(Engine &owner): Obj(owner) {

// Create the points.
   m_pointCnt = 20;
   m_points = new ObjPos[m_pointCnt];

   m_points[0] = ObjPos(5.0, -5.0);
   m_points[1] = ObjPos(10.0, -2.0);
   m_points[2] = ObjPos(10.0, 2.0);
   m_points[3] = ObjPos(8.0, 4.0);
   m_points[4] = ObjPos(-2.0, 4.0);
   m_points[5] = ObjPos(-2.0, 2.0);
   m_points[6] = ObjPos(2.0, 2.0);
   m_points[7] = ObjPos(2.0, 4.0);
   m_points[8] = ObjPos(-8.0, 4.0);
   m_points[9] = ObjPos(-10.0, 2.0);
   m_points[10] = ObjPos(-10.0, -2.0);
   m_points[11] = ObjPos(10.0, -2.0);
   m_points[12] = ObjPos(10.0, 2.0);
   m_points[13] = ObjPos(-10.0, 2.0);
   m_points[14] = ObjPos(-10.0, -2.0);
   m_points[15] = ObjPos(-5.0, -5.0);
   m_points[16] = ObjPos(0.0, -5.0);
   m_points[17] = ObjPos(-7.0, -2.0);
   m_points[18] = ObjPos(-5.0, -5.0);
   m_points[19] = ObjPos(5.0, -5.0);

// Get the collision radius.
   m_radius = m_calcRad();
}

// Is it a rock?
bool Alien::rock() const {
   return false;
}

// Can it occupy the Kuypier region?
bool Alien::kuypier() const {
   return true;
}

// Would a collision with other be fatal?
bool Alien::fatal(const Obj &other) const {
   if (other.dead()) return false;
   int w, h;
   mp_owner->GetPlayDims(&w, &h);
   return
      pos.real() >= 0 && pos.real() <= w && pos.imag() >= 0 && pos.imag() <= h &&
      (other.type() == otFire || other.mass() > mass());
}

// Move the object.
void Alien::Tick() {
   if (!m_dead) {
      m_internalTick();

   // Adjust the thrust.
      dir += Push();
      Obj::limitAbs(dir, MaxAlienSpeed);
   }
}

// The object's score, type, mass and termination routine.
// A big score or an extra life, for the alien.
int Alien::Score() const {
   return 500;
}

OType Alien::type() const {
   return otAlien;
}

double Alien::mass() const {
   return 15;
}

void Alien::explode() {
   m_dead = true;
   m_fragment(otDebris, 5);
}

// class Lance: public methods
// ───────────────────────────
// Make a new Lance object.
Lance::Lance(Engine &owner): Obj(owner) {
// Create the points.
   m_pointCnt = 2;
   m_points = new ObjPos[m_pointCnt];

   m_points[0] = ObjPos(0.0, -2.0);
   m_points[1] = ObjPos(0.0, 2.0);

// Get the collision radius.
   m_radius = m_calcRad();
}

// Is it a rock?
bool Lance::rock() const {
   return false;
}

// Can it occupy the Kuypier region?
bool Lance::kuypier() const {
   return false;
}

// Would a collision with other be fatal?
bool Lance::fatal(const Obj &other) const {
   return !other.dead() && other.mass() > 0;
}

// Move the object.
void Lance::Tick() {
   if (!m_dead) {
      m_internalTick();

   // Allow for it to get as much as 3/4 of the way across the screen.
      double speed = abs(dir);
      m_dead = speed == 0.0 || speed*_Ticks > 3.0/4.0*mp_owner->MinDim();
   }
}

// The object's score, type, mass and termination routine.
// (No explosion on termination: just die.)
int Lance::Score() const {
   return 0;
}

OType Lance::type() const {
   return otFire;
}

double Lance::mass() const {
   return 1;
}

void Lance::explode() {
   m_dead = true;
}

// class Debris: public methods
// ────────────────────────────
// Make a new Debris object.
Debris::Debris(Engine &owner): Obj(owner) {
// Create the points.
   m_pointCnt = 4;
   m_points = new ObjPos[m_pointCnt];

   m_points[0] = ObjPos(0.0, 3.0);
   m_points[1] = ObjPos(3.0, 0.0);
   m_points[2] = ObjPos(-2.0, -3.0);

// Rumple it.
   double x, y;
   const double vf = 0.2;

   for (int n = 0; n < m_pointCnt - 1; ++n) {
      x = m_points[n].real();
      x = vf*x*(2.0*Obj::randFloat() - 1.0);
      y = m_points[n].imag();
      y = vf*y*(2.0*Obj::randFloat() - 1.0);
      m_points[n] += ObjPos(x, y);
   }

// Connect the final point and randomly orient it.
   m_points[m_pointCnt - 1] = m_points[0];

   SetSpin(TwoPi*Obj::randFloat());

// Endow it with a rotation speed of approximately 8 degrees per tick.
   m_rotDelta = TwoPi*8.0/360.0;
   if (Obj::randFloat() < 0.5) m_rotDelta = -m_rotDelta;

// Get the collision radius.
   m_radius = m_calcRad();
}

// Is it a rock?
bool Debris::rock() const {
   return false;
}

// Can it occupy the Kuypier region?
bool Debris::kuypier() const {
   return true;
}

// Would a collision with other be fatal?
bool Debris::fatal(const Obj &other) const {
   return (!other.dead() && other.type() == otFire);
}

// Move the object for a short randomly-determined lifetime.
void Debris::Tick() {
   if (!m_dead) {
      m_internalTick();

      if (time(0) - m_tm > 2 && Obj::randFloat() < 0.1)
         explode();
   }
}

// The object's score, type, mass and termination routine.
// Mass 2 is just enough to bounce a ship.
// (No explosion on termination: just die.)
int Debris::Score() const {
   return 0;
}

OType Debris::type() const {
   return otDebris;
}

double Debris::mass() const {
   return 2;
}

void Debris::explode() {
   m_dead = true;
}

// class Spark: public methods
// ───────────────────────────
// Make a new Spark object: Debris with a much shorter life.
Spark::Spark(Engine &owner): Debris(owner) {
}

// Move the object for a very short randomly-determined lifetime.
void Spark::Tick() {
   if (!m_dead) {
      m_internalTick();

      double r = Obj::randFloat();
      if (r < 0.1 || (time(0) - m_tm > 1 && r < 0.5))
         explode();
   }
}

// The object's type.
OType Spark::type() const {
   return otSpark;
}

// class Thrust: public methods
// ────────────────────────────
// Make a new Thrust object.
Thrust::Thrust(Engine &owner): Obj(owner) {
// Create the points.
   m_pointCnt = 2;
   m_points = new ObjPos[m_pointCnt];

   m_points[0] = ObjPos(0.0, 1.0);
   m_points[1] = ObjPos(0.0, -1.0);

// Get the collision radius.
   m_radius = m_calcRad();
}

// Is it a rock?
bool Thrust::rock() const {
   return false;
}

// Can it occupy the Kuypier region?
bool Thrust::kuypier() const {
   return true;
}

// Would a collision with other be fatal?
bool Thrust::fatal(const Obj &/*other*/) const {
   return false;
}

// Move the object for a very limited time, so as to avoid long thrust trails across the screen.
void Thrust::Tick() {
   if (!m_dead) {
      m_internalTick();

      m_dead = (_Ticks > 1);
   }
}

// The object's score, type, mass and termination routine.
// Zero mass means transparent.
// (No explosion on termination: just die.)
int Thrust::Score() const {
   return 0;
}

OType Thrust::type() const {
   return otThrust;
}

double Thrust::mass() const {
   return 0;
}

void Thrust::explode() {
   m_dead = true;
}

// class Label: public methods
// ───────────────────────────
// Make a new Label object: endowed with a lifespan life in seconds.
Label::Label(Engine &owner, int life): Obj(owner) {
   m_life = life;
}

// Set/get the lifespan.
int Label::life() const {
   return m_life;
}

void Label::life(int sec) {
   m_life = sec;
}

// Is it a rock?
bool Label::rock() const {
   return false;
}

// Can it occupy the Kuypier region?
bool Label::kuypier() const {
   return false;
}

// Would a collision with other be fatal?
bool Label::fatal(const Obj &/*other*/) const {
   return false;
}

// Move the object for a limited time.
void Label::Tick() {
   if (!m_dead) {
      m_internalTick();

      m_dead = (time(0) - m_tm > m_life);
   }
}

// The object's score, type, mass and termination routine.
// Zero mass means transparent.
// (No explosion on termination: just die.)
int Label::Score() const {
   return 0;
}

OType Label::type() const {
   return otLabel;
}

double Label::mass() const {
   return 0;
}

void Label::explode() {
   m_dead = true;
}
