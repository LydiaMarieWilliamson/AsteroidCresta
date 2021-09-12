#ifndef OnceOnlyObjects_h
#define OnceOnlyObjects_h

// Asteroid Style Game: The objects to hold the state of the game artifacts.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <string>
#include <complex>
#include <time.h>

namespace Asteroid {
class Engine;

// Object position type, borrow complex type and user real as x, and imag as y.
typedef std::complex<double> ObjPos;

// Object type IDs.
enum TypeT { NoOT = 0, BoulderOT, StoneOT, PebbleOT, ShipOT, AlienOT, LanceOT, DebrisOT, SparkOT, ThrustOT, LabelOT };

// Label font size.
enum FontT { SmallLF = 0, MediumLF, LargeLF, HugeBoldLF };

// The game object abstract base class
// ───────────────────────────────────
class Thing { // → Rock, Ship, Alien, Lance, Debris, Thrust, Label
protected:
   bool _Dead;
   double _Radius, _Twist;
   Engine *_Owner;
   int _Points, _Ticks;
   time_t _Now;
   ObjPos *_Point;
   std::string _Caption;
   FontT _Pts;
   double _SizeUp() const;
   void _Replicate(TypeT t, int cnt, const double &sf = 1.0);
   void _Tick();
public:
   ObjPos _Pos, _Dir; // The position and orientation vectors.
   Thing(Engine &owner);
   virtual ~Thing();
   bool GetDead() const;
   void SetDead();
   Engine *GetOwner() const;
   double GetRadius() const;
   void Rotate(const double &a);
   int GetPoints() const;
   ObjPos PosPoints(int idx) const;
   std::string GetCaption() const;
   void SetCaption(const std::string &s);
   FontT GetPts() const;
   void SetPts(FontT sz);
// Pure virtual methods which define the behavior of the object in derived classes.
   virtual bool Rocky() const = 0;
   virtual bool Kuypier() const = 0;
   virtual bool Lethal(const Thing &other) const = 0;
   virtual void Tick() = 0;
   virtual int Score() const = 0;
   virtual TypeT Type() const = 0;
   virtual double Mass() const = 0;
   virtual void Boom() = 0;
   static double RandR();
   static bool RandB(double Prob = 0.5);
   static void RotateVector(ObjPos &p, double a);
   static void LimitAbs(ObjPos &p, const double a);
};

// The rock object abstract base class
// ───────────────────────────────────
class Rock: public Thing { // → Boulder, Stone, Pebble.
protected:
   void _Sculpt(const double &scale);
public:
   Rock(Engine &owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &other) const;
   virtual void Tick();
};

class Boulder: public Rock {
public:
   Boulder(Engine &owner);
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Stone: public Rock {
public:
   Stone(Engine &owner);
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Pebble: public Rock {
public:
   Pebble(Engine &owner);
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Ship: public Thing {
private:
   ObjPos _NosePos, _ThrustPos, _ThrustPlane;
   double _Orient;
   int _Spin, _FireCharge;
   bool _Pushing, _Firing, _FireLock, _JustFired;
   void _ResetPoints();
public:
   Ship(Engine &owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
   void SetSpin(int r);
   bool GetPushing() const;
   void SetPushing(bool on);
   void Fire();
   void ReLoad(bool reset);
   bool JustFired() const;
   int FireCharge() const;
};

class Alien: public Thing {
private:
   Thing *_Neighbor() const;
   ObjPos Push() const;
public:
   Alien(Engine &owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Lance: public Thing {
public:
   Lance(Engine &owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Debris: public Thing { // → Spark
public:
   Debris(Engine &owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Spark: public Debris {
public:
   Spark(Engine &owner);
   virtual void Tick();
   virtual TypeT Type() const;
};

class Thrust: public Thing {
public:
   Thrust(Engine &owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Label: public Thing {
private:
   int _Life; // Lifetime (in seconds).
public:
   Label(Engine &owner, int life = 2);
   int GetLife() const;
   void SetLife(int sec);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};
} // end of namespace Asteroid

#endif // OnceOnly
