#ifndef OnceOnlyObjects_h
#define OnceOnlyObjects_h

// Asteroid Style Game: The objects to hold the state of the game artifacts.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <string>
#include <complex>
#include <time.h>

namespace asteroid {
class Engine;

// Object position type, borrow complex
// type and user real as x, and imag as y.
typedef std::complex<double> ObjPos;

// Object type IDs
enum OType {otNone = 0, otBigRock, otMedRock, otSmallRock, otShip,
  otAlien, otFire, otDebris, otSpark, otThrust, otLabel};

// Label font size.
enum LFSize { lfSmall = 0, lfMedium, lfLarge, lfHugeBold };

// The game object abstract base class
// ───────────────────────────────────
class Obj
{
protected:
  bool m_dead;
  double m_radius, m_rotDelta;
  Engine* mp_owner;
  int m_pointCnt, m_tickCnt;
  time_t m_tm;
  ObjPos* m_points;
  std::string m_caption;
  LFSize m_fontSize;
  double m_calcRad() const;
  void m_fragment(OType t, int cnt, const double& sf = 1.0);
  void m_internalTick();
public:
  ObjPos pos; // Position
  ObjPos dir; // Direction vector

  Obj(Engine& owner);
  virtual ~Obj();
  void die();
  bool dead() const;
  Engine* owner() const;
  double radius() const;
  void rotate(const double& a);
  int pointCnt() const;
  ObjPos points(int idx) const;
  std::string caption() const;
  void caption(const std::string& s);
  LFSize fontSize() const;
  void fontSize(LFSize sz);

  // Pure virtual methods which define
  // the behaviour of the object in derived classes
  virtual bool rock() const = 0;
  virtual bool kuypier() const = 0;
  virtual bool fatal(const Obj& other) const = 0;
  virtual void tick() = 0;
  virtual int score() const = 0;
  virtual OType type() const = 0;
  virtual double mass() const = 0;
  virtual void explode() = 0;

  // Static functions
  static double randFloat();
  static void rotateVector(ObjPos& p, double a);
  static void limitAbs(ObjPos& p, const double a);
};

// The rock object abstract base class
// ───────────────────────────────────
class RockBase : public Obj
{
protected:
  void m_createPoints(const double& scale);
public:
  RockBase(Engine& owner);
  virtual bool rock() const;
  virtual bool kuypier() const;
  virtual bool fatal(const Obj& other) const;
  virtual void tick();
};

class BigRock : public RockBase
{
public:
  BigRock(Engine& owner);
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};

class MedRock : public RockBase
{
public:
  MedRock(Engine& owner);
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};

class SmallRock : public RockBase
{
public:
  SmallRock(Engine& owner);
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};

class Ship : public Obj
{
private:
  ObjPos m_nosePos;
  ObjPos m_thrustPos;
  ObjPos m_thrustPlane;
  double m_alpha;
  int m_rotDir, m_fireCharge;
  bool m_thrust, m_fire, m_fireLock, m_justFired;
  void m_resetPoints();
public:
  Ship(Engine& owner);
  virtual bool rock() const;
  virtual bool kuypier() const;
  virtual bool fatal(const Obj& other) const;
  virtual void tick();
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
  void rot(int r);
  void thrust(bool on);
  bool thrusting() const;
  void fire();
  void reload(bool reset);
  bool justFired() const;
  int fireCharge() const;
};

class Alien : public Obj
{
private:
  Obj* m_nearObj() const;
  ObjPos thrust() const;
public:
  Alien(Engine& owner);
  virtual bool rock() const;
  virtual bool kuypier() const;
  virtual bool fatal(const Obj& other) const;
  virtual void tick();
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};

class Fire : public Obj
{
public:
  Fire(Engine& owner);
  virtual bool rock() const;
  virtual bool kuypier() const;
  virtual bool fatal(const Obj& other) const;
  virtual void tick();
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};

class Debris : public Obj
{
public:
  Debris(Engine& owner);
  virtual bool rock() const;
  virtual bool kuypier() const;
  virtual bool fatal(const Obj& other) const;
  virtual void tick();
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};

class Spark : public Debris
{
public:
  Spark(Engine& owner);
  virtual void tick();
  virtual OType type() const;
};

class Thrust : public Obj
{
public:
  Thrust(Engine& owner);
  virtual bool rock() const;
  virtual bool kuypier() const;
  virtual bool fatal(const Obj& other) const;
  virtual void tick();
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};

class Label : public Obj
{
private:

  // Limited lifetime
  int m_life;

public:
  Label(Engine& owner, int life = 2);
  int life() const;
  void life(int sec);
  virtual bool rock() const;
  virtual bool kuypier() const;
  virtual bool fatal(const Obj& other) const;
  virtual void tick();
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};
} // end of namespace Asteroid

#endif // OnceOnly
