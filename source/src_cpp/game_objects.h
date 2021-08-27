//---------------------------------------------------------------------------
// PROJECT     : Asteroid Style Game
// FILE NAME   : game_objects.h
// DESCRIPTION : Objects to hold state of game artifacts
// COPYRIGHT   : Big Angry Dog (C) 2009
// This file is part of the "Asteroid Cresta" program.
// Asteroid Cresta is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Asteroid Cresta is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with Asteroid Cresta.  If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// HEADER GUARD
//---------------------------------------------------------------------------
#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

//---------------------------------------------------------------------------
// INCLUDES
//---------------------------------------------------------------------------
#include <string>
#include <complex>
#include <time.h>
namespace asteroid {

//---------------------------------------------------------------------------
// NON-CLASS MEMBERS
//---------------------------------------------------------------------------

 // Forward declaration
class Engine;

// Object position type, borrow complex
// type and user real as x, and imag as y.
typedef std::complex<double> ObjPos;

// Object type IDs
enum OType {otNone = 0, otBigRock, otMedRock, otSmallRock, otShip,
  otAlien, otFire, otDebris, otSpark, otThrust, otLabel};

// Label font size
enum LFSize { lfSmall = 0, lfMedium, lfLarge, lfHugeBold };
//---------------------------------------------------------------------------
// CLASS Obj (abstract base class for game objects)
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
// CLASS RockBase (abstract base rock class)
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
// CLASS BigRock
//---------------------------------------------------------------------------
class BigRock : public RockBase
{
public:
  BigRock(Engine& owner);
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};
//---------------------------------------------------------------------------
// CLASS MedRock
//---------------------------------------------------------------------------
class MedRock : public RockBase
{
public:
  MedRock(Engine& owner);
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};
//---------------------------------------------------------------------------
// CLASS SmallRock
//---------------------------------------------------------------------------
class SmallRock : public RockBase
{
public:
  SmallRock(Engine& owner);
  virtual int score() const;
  virtual OType type() const;
  virtual double mass() const;
  virtual void explode();
};
//---------------------------------------------------------------------------
// CLASS Ship
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
// CLASS Alien
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
// CLASS Fire
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
// CLASS Debris
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
// CLASS Spark
//---------------------------------------------------------------------------
class Spark : public Debris
{
public:
  Spark(Engine& owner);
  virtual void tick();
  virtual OType type() const;
};
//---------------------------------------------------------------------------
// CLASS Thrust
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
// CLASS Label
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
}
#endif
