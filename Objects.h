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
   void _Replicate(TypeT T, int N, const double &SpeedUp = 1.0);
   void _Tick();
public:
   ObjPos _Pos, _Dir; // The position and orientation vectors.
   Thing(Engine &Owner);
   virtual ~Thing();
   bool GetDead() const;
   void SetDead();
   Engine *GetOwner() const;
   double GetRadius() const;
   void Rotate(const double &Rad);
   int GetPoints() const;
   ObjPos PosPoints(int Points) const;
   std::string GetCaption() const;
   void SetCaption(const std::string &Caption);
   FontT GetPts() const;
   void SetPts(FontT Pts);
// Pure virtual methods which define the behavior of the object in derived classes.
   virtual bool Rocky() const = 0;
   virtual bool Kuypier() const = 0;
   virtual bool Lethal(const Thing &Other) const = 0;
   virtual void Tick() = 0;
   virtual int Score() const = 0;
   virtual TypeT Type() const = 0;
   virtual double Mass() const = 0;
   virtual void Boom() = 0;
   static double RandR();
   static bool RandB(double Prob = 0.5);
   static void RotateVector(ObjPos &Pos, double Rad);
   static void LimitAbs(ObjPos &Pos, const double A);
};

// The rock object abstract base class
// ───────────────────────────────────
class Rock: public Thing { // → Boulder, Stone, Pebble.
protected:
   void _Sculpt(const double &Scale);
public:
   Rock(Engine &Owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &Other) const;
   virtual void Tick();
};

class Boulder: public Rock {
public:
   Boulder(Engine &Owner);
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Stone: public Rock {
public:
   Stone(Engine &Owner);
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Pebble: public Rock {
public:
   Pebble(Engine &Owner);
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
   Ship(Engine &Owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &Other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
   void SetSpin(int Spin);
   bool GetPushing() const;
   void SetPushing(bool Pushing);
   void Fire();
   void ReLoad(bool ReSet);
   bool JustFired() const;
   int FireCharge() const;
};

class Alien: public Thing {
private:
   Thing *_Neighbor() const;
   ObjPos Push() const;
public:
   Alien(Engine &Owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &Other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Lance: public Thing {
public:
   Lance(Engine &Owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &Other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Debris: public Thing { // → Spark
public:
   Debris(Engine &Owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &Other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};

class Spark: public Debris {
public:
   Spark(Engine &Owner);
   virtual void Tick();
   virtual TypeT Type() const;
};

class Thrust: public Thing {
public:
   Thrust(Engine &Owner);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &Other) const;
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
   Label(Engine &Owner, int Life = 2);
   int GetLife() const;
   void SetLife(int Life);
   virtual bool Rocky() const;
   virtual bool Kuypier() const;
   virtual bool Lethal(const Thing &Other) const;
   virtual void Tick();
   virtual int Score() const;
   virtual TypeT Type() const;
   virtual double Mass() const;
   virtual void Boom();
};
} // end of namespace Asteroid

#endif // OnceOnly
