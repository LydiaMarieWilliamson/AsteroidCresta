// Asteroid Style Game: The objects to hold the state of the game artifacts.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Objects.h"
#include "Engine.h"

using namespace std;
using namespace asteroid;

#ifdef M_PI
  const double TPI = 2.0 * M_PI; // 2*PI
#else
  const double TPI = 6.28318530717958648;
#endif

// class Obj: protected methods
// ────────────────────────────
double Obj::m_calcRad() const
{
  // Determine (mean) radius for this object
  double rslt = 0.0;

  for(int n = 0; n < m_pointCnt; ++n)
    rslt += abs(m_points[n]) / m_pointCnt;

  return rslt;
}

void Obj::m_fragment(OType t, int cnt, const double& sf)
{
  // Add cnt copies of type t to list
  // Speed factor sf.
  if (cnt > 0)
  {
    // Space away main from origin
    // moving in oposite directions
    ObjPos ndir(dir), npos(dir), bdir(dir/2.0);

    Obj::rotateVector(ndir, TPI / 4.0);
    if (ndir != 0.0) ndir *= sf / abs(ndir);

    if (npos != 0.0)
    {
      npos *= m_radius / abs(npos);
      Obj::rotateVector(npos, TPI/4.0);
    }

    double dr = TPI / cnt;
    for(int n = 0; n < cnt; ++n)
    {
      mp_owner->add(t, pos + npos, bdir + ndir);
      Obj::rotateVector(ndir, dr);
      Obj::rotateVector(npos, dr);
    }
  }
}

void Obj::m_internalTick()
{
  // Should always be called by tick() to
  // perform default repositioning & updates
  if (!m_dead)
  {
    // Increment internal tick counter
    ++m_tickCnt;

    // Check range just in case game
    // is left running for several years
    if (m_tickCnt == 0x7FFFFFFF)
      m_tickCnt = 1000;

    // New position
    pos += dir;

    // Rotate if m_rotDelta not zero
    rotate(m_rotDelta);
  }
}

// class Obj: public methods
// ─────────────────────────
Obj::Obj(Engine& owner)
{
  // Base Constructor
  // Should be called by derived classes in constructor
  m_points = 0;
  m_pointCnt = 0;
  m_dead = false;
  m_radius = 0.0;
  m_rotDelta = 0.0;
  m_tickCnt = 0;
  mp_owner = &owner;
  m_fontSize = lfSmall;

  // Creation time
  m_tm = time(0);
}

Obj::~Obj()
{
  // Destructor
  try
  {
    delete[] m_points;
  }
  catch(...)
  {
  }
}

void Obj::die()
{
  // I will soon be no longer
  m_dead = true;
}

bool Obj::dead() const
{
  // Am I lingering, waiting only to be drawn over
  return m_dead;
}

Engine* Obj::owner() const
{
  // Return owner
  return mp_owner;
}

double Obj::radius() const
{
  // Return radius
  return m_radius;
}

void Obj::rotate(const double& a)
{
  // Rotate the object around pos by a radians
  if (a != 0.0)
  {
    for(int n = 0; n < m_pointCnt; ++n)
      Obj::rotateVector(m_points[n], a);
  }
}

int Obj::pointCnt() const
{
  // How may points in my rendering
  return m_pointCnt;
}

ObjPos Obj::points(int idx) const
{
  // Get point idx (between 0 & pointCnt()).
  // Returned point value is always relative to current positioned.
  if (!m_dead && idx < m_pointCnt)
    return m_points[idx] + pos;
  else
    return ObjPos();
}

std::string Obj::caption() const
{
  // Label accessor
  return m_caption;
}

void Obj::caption(const std::string& s)
{
  // Label mutator
  m_caption = s;
}

LFSize Obj::fontSize() const
{
  // Label accessor
  return m_fontSize;
}

void Obj::fontSize(LFSize sz)
{
  // Label mutator
  m_fontSize = sz;
}

double Obj::randFloat()
{
  // Return float 0.0 <= rand < 1.0
  return (double)rand() / RAND_MAX;
}

void Obj::rotateVector(ObjPos& p, double a)
{
  // Rotate vector around origin by a radians,
  // where positive a is counter-clockwise
  if (p != 0.0)
  {
    a += arg(p);
    double h = abs(p);
    p = ObjPos(h * cos(a), h * sin(a));
  }
}

void Obj::limitAbs(ObjPos& p, const double a)
{
  // Limit absolute value
  double b = abs(p);
  if (b > a && b != 0.0) p *= a / b;
}

// class RockBase: Protected Methods
// ─────────────────────────────────
void RockBase::m_createPoints(const double& scale)
{
  // Create rock points. This is a circle
  // with random variation made to points.
  // Rock can be made larger or smaller
  // with scale value.
  m_pointCnt = 21;
  m_points = new ObjPos[m_pointCnt];

  const double vf = 0.25;
  double x, y, alpha = 0.0;
  for(int n = 0; n < m_pointCnt - 1; ++n)
  {
    m_points[n] = ObjPos(20.0 * sin(alpha), 20.0 * cos(alpha));
    alpha += TPI / (m_pointCnt - 1);
    m_points[n] *= scale;

    // Add some variation
    x = m_points[n].real();
    x = vf * x * (2.0 * Obj::randFloat() - 1.0);
    y = m_points[n].imag();
    y = vf * y * (2.0 * Obj::randFloat() - 1.0);
    m_points[n] += ObjPos(x, y);
  }

  // Connect final point
  m_points[m_pointCnt - 1] = m_points[0];

  // Determine collision radius
  m_radius = m_calcRad();
}

// class RockBase: public methods
// ──────────────────────────────
RockBase::RockBase(Engine& owner)
  : Obj(owner)
{
  // Constructor
}

bool RockBase::rock() const
{
  // Is a rock?
  return true;
}

bool RockBase::kuypier() const
{
  // Can occupy Kuypier region?
  return true;
}

bool RockBase::fatal(const Obj& other) const
{
  // Would a collision with other be fatal?
  // Collisions in Kuypier region are never
  // fatal, otherwise collision with other
  // rocks is fatal if combined speed is
  // large & other mass is greater than this
  // one. Fire is always a killer.
  bool rslt = false;

  if (!other.dead())
  {
    int w, h;
    mp_owner->getPlayDims(&w, &h);

    if (other.type() == otFire && m_tickCnt > 2)
    {
      rslt = true;
    }
    else
    if (pos.real() < 0 || pos.real() > w || pos.real() < 0 || pos.imag() > h)
    {
      rslt = false;
    }
    else
    {
      rslt = (abs(dir - other.dir) > MAX_SPEED / 5.0 &&
        (other.mass() > mass() || (other.mass() == mass() &&
        Obj::randFloat() < 0.5)));
    }
  }

  return rslt;
}

void RockBase::tick()
{
  // Update position & rotate
  if (!m_dead)
  {
    m_internalTick();

    // Random end of life after time period
    if (type() != otSmallRock &&
      time(0) - m_tm > ROCK_LIFE_LIMIT && Obj::randFloat() < ROCK_EXPLODE_PROB)
    {
      explode();
    }
  }
}

// class BigRock: public methods
// ─────────────────────────────
BigRock::BigRock(Engine& owner)
  : RockBase(owner)
{
  // I am a rotator - rotation speed
  // between approx 1 deg per tick
  m_rotDelta = TPI * 1.0 / 360.0;
  if (Obj::randFloat() < 0.5) m_rotDelta = -m_rotDelta;

  // Create points
  m_createPoints(1.0);
}

int BigRock::score() const
{
  // What's my score
  return 100;
}

OType BigRock::type() const
{
  // What am I
  return otBigRock;
}

double BigRock::mass() const
{
  // How much do I weigh
  return 300;
}

void BigRock::explode()
{
  // Explode
  m_dead = true;
  m_fragment(otMedRock, 2);
  m_fragment(otSpark, 5, 4.0);
}

// class MedRock: public methods
// ─────────────────────────────
MedRock::MedRock(Engine& owner)
  : RockBase(owner)
{
  // Constructor
  // I am a rotator - rotation speed
  // between approx 2 deg per tick
  m_rotDelta = TPI * 2.0 / 360.0;
  if (Obj::randFloat() < 0.5) m_rotDelta = -m_rotDelta;

  // Create points
  m_createPoints(0.71);
}

int MedRock::score() const
{
  // What's my score
  return 50;
}

OType MedRock::type() const
{
  // What am I
  return otMedRock;
}

double MedRock::mass() const
{
  // How much do I weigh
  return 200;
}

void MedRock::explode()
{
  // Explode
  m_dead = true;
  m_fragment(otSmallRock, 2);
  m_fragment(otSpark, 3, 2.0);
}

// class SmallRock: public methods
// ───────────────────────────────
SmallRock::SmallRock(Engine& owner)
  : RockBase(owner)
{
  // Constructor
  // I am a rotator - rotation speed
  // between approx 4 deg per tick
  m_rotDelta = TPI * 4.0 / 360.0;
  if (Obj::randFloat() < 0.5) m_rotDelta = -m_rotDelta;

  // Create points
  m_createPoints(0.4);
}

int SmallRock::score() const
{
  // What's my score
  return 25;
}

OType SmallRock::type() const
{
  // What am I
  return otSmallRock;
}

double SmallRock::mass() const
{
  // How much do I weigh
  return 125;
}

void SmallRock::explode()
{
  // Explode
  m_dead = true;
  m_fragment(otDebris, 5);
}

// class Ship: private methods
// ───────────────────────────
void Ship::m_resetPoints()
{
  // Reset points - used in rotation to
  // avoid build-up of rounding error

  // Must be m_pointCnt number of points
  m_points[0] = ObjPos(0.0, -10.0);
  m_points[1] = ObjPos(7.0, 10.0);
  m_points[2] = ObjPos(0, 7.0);
  m_points[3] = ObjPos(-7, 10.0);
  m_points[4] = ObjPos(0.0, -10.0);

  // Gun & thrust postions
  m_nosePos = ObjPos(0.0, -14.0); // Forward facing tip
  m_thrustPos = ObjPos(3.0, 10.0); // A bottom corner
  m_thrustPlane = ObjPos(-6.0, 0.0); // The line from one bottom point to the other
}

// class Ship: public methods
// ──────────────────────────
Ship::Ship(Engine& owner)
  : Obj(owner)
{
  // Constructor
  m_fire = false;
  m_fireLock = false;
  m_justFired = false;
  m_fireCharge = CHARGE_MAX;
  m_rotDir = 0;
  m_thrust = false;
  m_alpha = TPI / 8.0;

  // Create points
  m_pointCnt = 5;
  m_points = new ObjPos[m_pointCnt];

  // Points set here
  m_resetPoints();
  rotate(m_alpha);
  Obj::rotateVector(m_nosePos, m_alpha);
  Obj::rotateVector(m_thrustPos, m_alpha);
  Obj::rotateVector(m_thrustPlane, m_alpha);

  // Determine collision radius
  m_radius = m_calcRad();
}

bool Ship::rock() const
{
  // Is a rock?
  return false;
}

bool Ship::kuypier() const
{
  // Can occupy Kuypier region?
  return false;
}

bool Ship::fatal(const Obj& other) const
{
  // Would a collision with other be fatal
  return (!other.dead() && other.mass() > mass());
}

void Ship::tick()
{
  // Update position
  if (!m_dead)
  {
    m_internalTick();

    // Increment fire charge
    if (m_fireCharge < CHARGE_MAX && (m_tickCnt % CHARGE_MOD) == 0)
    {
      ++m_fireCharge;
    }

    // ROTATE ACTION
    if (m_rotDir != 0)
    {
      if (m_rotDir == -1)
        m_alpha -= SHIP_ROTATE_DELTA * TPI / 360.0;
      else
        m_alpha += SHIP_ROTATE_DELTA * TPI / 360.0;

      // Rotate superstructure
      m_resetPoints();
      rotate(m_alpha);

      // Rotate nose & thrust plane assemblies
      Obj::rotateVector(m_nosePos, m_alpha);
      Obj::rotateVector(m_thrustPos, m_alpha);
      Obj::rotateVector(m_thrustPlane, m_alpha);
    }

    // THRUST ACTION
    if (m_thrust)
    {
      // Thrust vector
      ObjPos tvect(sin(m_alpha), -cos(m_alpha));
      tvect *= SHIP_THRUST_MULT;

      // Exhaust vector
      ObjPos exv(tvect * (-MAX_SPEED / 2.0) + dir);

      // Add thrust particles
      for(int n = 0; n < 2; ++n)
      {
        // Random exhaust exit position
        ObjPos tpos(m_thrustPlane * Obj::randFloat());
        tpos += m_thrustPos + pos;

        Obj* f = mp_owner->add(otThrust, tpos, exv);
        f->rotate(m_alpha);
      }

      // Add thrust to direction, and limit
      // to max speed, i.e. we don't want
      // to catch up with friendly fire
      dir += tvect;
      Obj::limitAbs(dir, MAX_SPEED);
    }

    // FIRE ACTION
    if (m_fire && !m_fireLock && m_fireCharge > 0)
    {
      // Create fire object (initially heading toward ship)
      ObjPos fvect(sin(m_alpha), -cos(m_alpha));
      fvect *= MAX_SPEED;
      fvect += dir;

      // Recoil
      dir -= fvect * FIRE_RECOIL_MULT;

      // Bombs away!
      Obj* f = mp_owner->add(otFire, m_nosePos + pos, fvect);
      f->rotate(m_alpha);

      // Stop repeated firing
      m_fireLock = true;
      m_justFired = true;
      m_fireCharge--;
    }
    else
    {
      m_fire = false;
      m_justFired = false;
    }
  }
}

int Ship::score() const
{
  // No points for getting blown up
  return 0;
}

OType Ship::type() const
{
  // What am I
  return otShip;
}

double Ship::mass() const
{
  // How much do I weigh
  return 10;
}

void Ship::explode()
{
  // Explode
  m_dead = true;
  m_fragment(otDebris, 5);
}

void Ship::rot(int r)
{
  // Rotate on next tick
  // -1 left, 0 stop rotate, +1 right
  m_rotDir = r;
}

void Ship::thrust(bool on)
{
  // Thrust
  m_thrust = on;
}

bool Ship::thrusting() const
{
  // Is thrusting
  return m_thrust;
}

void Ship::fire()
{
  // Fire on next tick. Each
  // call to fire() must be followed by
  // call to reload() in order to release
  // fire lock. This is done to prevent
  // rapid "machine gun" firing action
  // making the game too easy.
  m_fire |= !m_fireLock;
}

void Ship::reload(bool reset)
{
  // Call to release fire lock. Typically
  // this should be called on a key up action.
  m_fireLock = false;
  if (reset) m_fire = false;
}

bool Ship::justFired() const
{
  // Did the ship fire on last tick?
  return m_justFired;
}

int Ship::fireCharge() const
{
    // Return ship fire charge,
    // a value between 0 & CHARGEMAX.
    // When reaches zero, ship cannot fire
    return m_fireCharge;
}

// class Alien: private methods
// ────────────────────────────
Obj* Alien::m_nearObj() const
{
  // Return nearest fatal object to alien
  // or return null if no objects
  Obj *rslt = 0;
  ObjPos ndv;
  double oabs, nabs = -1.0;

  for(size_t n = 0; n < mp_owner->objCnt(); ++n)
  {
    Obj* objn = mp_owner->objAtIdx(n);
    OType ot = objn->type();
    oabs = abs(pos - objn->pos);

    if ( (objn->mass() > mass() || ot == otShip) &&
      (nabs < 0.0 || oabs < nabs) )
    {
      // Hold nearest heavier object
      // or ship (avoid the ship)
      rslt = objn;
      nabs = oabs;
    }
  }

  return rslt;
}

ObjPos Alien::thrust() const
{
  // Determine thrust vector away from nearest object
  ObjPos rslt;
  Obj* obj = m_nearObj();
  if (obj != 0) rslt = pos - obj->pos;

  // Scale so that closer objects
  // yield higher thrusts
  double nabs = abs(rslt);

  if (nabs > 0.0)
  {
    rslt *= 20.0 * m_radius / (nabs * nabs);
  }

  rslt *= ALIEN_THRUST_MULT;

  return rslt;
}

// class Alien: public methods
// ───────────────────────────
Alien::Alien(Engine& owner)
  : Obj(owner)
{
  // Constructor

  // Create points
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

  // Determine collision radius
  m_radius = m_calcRad();
}

bool Alien::rock() const
{
  // Is a rock?
  return false;
}

bool Alien::kuypier() const
{
  // Can occupy Kuypier region?
  return true;
}

bool Alien::fatal(const Obj& other) const
{
  // Would a collision with other be fatal
  bool rslt = false;

  if (!other.dead())
  {
    int w, h;
    mp_owner->getPlayDims(&w, &h);
    if (pos.real() < 0 || pos.real() > w ||
      pos.real() < 0 || pos.imag() > h)
        rslt = false;
    else
      rslt = (other.type() == otFire || other.mass() > mass());
  }

  return rslt;
}

void Alien::tick()
{
  // Update position
  if (!m_dead)
  {
    m_internalTick();

    // Get new thrust vector
    dir += thrust();
    Obj::limitAbs(dir, MAX_ALIEN_SPEED);
  }
}

int Alien::score() const
{
  // Big score for alien or extra life
  return 500;
}

OType Alien::type() const
{
  // What am I
  return otAlien;
}

double Alien::mass() const
{
  // How much do I weigh
  return 15;
}

void Alien::explode()
{
  // Explode
  m_dead = true;
  m_fragment(otDebris, 5);
}

// class Fire: public methods
// ──────────────────────────
Fire::Fire(Engine& owner)
  : Obj(owner)
{
  // Constructor
  // Create points
  m_pointCnt = 2;
  m_points = new ObjPos[m_pointCnt];

  m_points[0] = ObjPos(0.0, -2.0);
  m_points[1] = ObjPos(0.0, 2.0);

  // Determine collision radius
  m_radius = m_calcRad();
}

bool Fire::rock() const
{
  // Is a rock?
  return false;
}

bool Fire::kuypier() const
{
  // Can occupy Kuypier region?
  return false;
}

bool Fire::fatal(const Obj& other) const
{
  // Would a collision with other be fatal
  return (!other.dead() && other.mass() > 0);
}

void Fire::tick()
{
  // Update position
  if (!m_dead)
  {
    m_internalTick();

    // Allow to get 3/4 across screen
    double speed = abs(dir);
    m_dead = (speed == 0.0 ||
      speed * m_tickCnt > 3.0/4.0 * mp_owner->minDim());
  }
}

int Fire::score() const
{
  // No points for shooting bullets
  return 0;
}

OType Fire::type() const
{
  // What am I
  return otFire;
}

double Fire::mass() const
{
  // How much do I weigh
  return 1;
}

void Fire::explode()
{
  // Explode (no explosion - just die)
  m_dead = true;
}

// class Debris: public methods
// ────────────────────────────
Debris::Debris(Engine& owner)
  : Obj(owner)
{
  // Constructor

  // Create points
  m_pointCnt = 4;
  m_points = new ObjPos[m_pointCnt];

  m_points[0] = ObjPos(0.0, 3.0);
  m_points[1] = ObjPos(3.0, 0.0);
  m_points[2] = ObjPos(-2.0, -3.0);

  // Add some variation
  double x, y;
  const double vf = 0.2;

  for(int n = 0; n < m_pointCnt - 1; ++n)
  {
    // Add some variation
    x = m_points[n].real();
    x = vf * x * (2.0 * Obj::randFloat() - 1.0);
    y = m_points[n].imag();
    y = vf * y * (2.0 * Obj::randFloat() - 1.0);
    m_points[n] += ObjPos(x, y);
  }

  // Connect final point
  m_points[m_pointCnt - 1] = m_points[0];

  // Rotate by random amount
  rotate(TPI * Obj::randFloat());

  // I am a rotator - rotation speed
  // approx 8 deg per tick
  m_rotDelta = TPI * 8.0 / 360.0;
  if (Obj::randFloat() < 0.5) m_rotDelta = -m_rotDelta;

  // Determine collision radius
  m_radius = m_calcRad();
}

bool Debris::rock() const
{
  // Is a rock?
  return false;
}

bool Debris::kuypier() const
{
  // Can occupy Kuypier region?
  return true;
}

bool Debris::fatal(const Obj& other) const
{
  // Would a collision with other be fatal
  return (!other.dead() && other.type() == otFire);
}

void Debris::tick()
{
  // Update position
  if (!m_dead)
  {
    m_internalTick();

    // Short randomized life
    if (time(0) - m_tm > 2 && Obj::randFloat() < 0.1)
      explode();
  }
}

int Debris::score() const
{
  // Nothing for shooting debris
  return 0;
}

OType Debris::type() const
{
  // What am I
  return otDebris;
}

double Debris::mass() const
{
  // How much do I weigh (just enought to bounce a ship)
  return 2;
}

void Debris::explode()
{
  // Explode (no explosion - just die)
  m_dead = true;
}

// class Spark: public methods
// ───────────────────────────
Spark::Spark(Engine& owner)
  : Debris(owner)
{
  // Same as debris, but much short life
}

void Spark::tick()
{
  // Update position
  if (!m_dead)
  {
    m_internalTick();

    // Very short randomized life
    double r = Obj::randFloat();
    if ( r < 0.1 || (time(0) - m_tm > 1 && r < 0.5) )
      explode();
  }
}

OType Spark::type() const
{
  // What am I
  return otSpark;
}

// class Thrust: public methods
// ────────────────────────────
Thrust::Thrust(Engine& owner)
  : Obj(owner)
{
  // Constructor

  // Create points
  m_pointCnt = 2;
  m_points = new ObjPos[m_pointCnt];

  m_points[0] = ObjPos(0.0, 1.0);
  m_points[1] = ObjPos(0.0, -1.0);

  // Determine collision radius
  m_radius = m_calcRad();
}

bool Thrust::rock() const
{
  // Is a rock?
  return false;
}

bool Thrust::kuypier() const
{
  // Can occupy Kuypier region?
  return true;
}

bool Thrust::fatal(const Obj& other) const
{
  // Would a collision with other be fatal
  return false;
}

void Thrust::tick()
{
  // Update position
  if (!m_dead)
  {
    m_internalTick();

    // Very limited life otherwise
    // the result is long thrust
    // trails across the screen
    m_dead = (m_tickCnt > 1);
  }
}

int Thrust::score() const
{
  // Nothing for shooting thust particles
  return 0;
}

OType Thrust::type() const
{
  // What am I
  return otThrust;
}

double Thrust::mass() const
{
  // How much do I weigh
  return 0;
}

void Thrust::explode()
{
  // Explode (no explosion - just die)
  m_dead = true;
}

// class Label: public methods
// ───────────────────────────
Label::Label(Engine& owner, int life)
  : Obj(owner)
{
  // Constructor
  m_life = life;
}

void Label::life(int sec)
{
  // Life mutator
  m_life = sec;
}

bool Label::kuypier() const
{
  // Can occupy Kuypier region?
  return false;
}

int Label::life() const
{
  // Limited life in seconds
  return m_life;
}

bool Label::rock() const
{
  // Is a rock?
  return false;
}

bool Label::fatal(const Obj& other) const
{
  // Would a collision with other be fatal
  return false;
}

void Label::tick()
{
  // Update position
  if (!m_dead)
  {
    m_internalTick();

    // Limited life
    m_dead = (time(0) - m_tm > m_life);
  }
}

int Label::score() const
{
  // Nothing for shooting labels
  return 0;
}

OType Label::type() const
{
  // What am I
  return otLabel;
}

double Label::mass() const
{
  // How much do I weigh
  // Zero is transparent
  return 0;
}

void Label::explode()
{
  // Explode (no explosion - just die)
  m_dead = true;
}
