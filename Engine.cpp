// Asteroid Style Game: The engine for holding and updating the game artifacts.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <math.h>
#include <stdlib.h>
#include "Engine.h"

using namespace std;
using namespace asteroid;

// class Engine: private methods
// ─────────────────────────────
// Empty the game of objects.
void Engine::m_empty(bool killNow) {
   size_t sz = m_objects.size();

   if (killNow) { // Kill all life in space.
      for (size_t n = 0; n < sz; ++n) {
         delete m_objects[n];
      }

      m_objects.clear();
   } else { // Condemn them for deletion on the next tick.
      for (size_t n = 0; n < sz; ++n) {
         m_objects[n]->die();
      }
   }
}

// Collision-test for o1 and o2: are both alive, with positive mass and closer to each other than their respective sizes?
bool Engine::m_collision(const Obj &o1, const Obj &o2) const {
   return !o1.dead() && !o2.dead() && o1.mass() > 0 && o2.mass() > 0 && abs(o1.pos - o2.pos) <= o1.radius() + o2.radius();
}

// Rebound these objects.
void Engine::m_rebound(const Obj &o1, const Obj &o2, ObjPos &nd1, ObjPos &nd2) const {
// Intentionally copy the directions, as the result reference may be object directions.
   ObjPos d1(o1.dir);
   double m1(o1.mass());
   ObjPos d2(o2.dir);
   double m2(o2.mass());
   double m12(m1 + m2);

   if (m12 > 0.0 && abs(o1.pos - o2.pos) > abs(o1.pos - o2.pos + (d1 - d2)*0.1)) {
      nd1 = (d1*(m1 - m2)/m12) + (d2*2.0*m2/m12);
      Obj::limitAbs(nd1, MAX_SPEED);
      nd2 = (d2*(m2 - m1)/m12) + (d1*2.0*m1/m12);
      Obj::limitAbs(nd2, MAX_SPEED);
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
void Engine::m_stateTick() {
// Increment the counter.
   m_tickCnt++;
   if (m_tickCnt >= 0x7fffffff)
      m_tickCnt = 1000;

// Set the sound flags to zero; they will be set back to true below, if required.
   m_explosionSnd = otNone;
   m_diedSnd = false;
   m_alienSnd = false;

// Free and remove objects which are now dead from the previous tick.
   for (size_t rn = 0; rn < m_objects.size(); )
      if (m_objects[rn]->dead()) {
         delete m_objects[rn];
         m_objects.erase(m_objects.begin() + rn);
      } else {
         ++rn;
      }

// Hold the count, because other objects will be added as parts of explosions.
   size_t sz = m_objects.size();

// Tick each object: get them each to do their thing in the next state tick.
   for (size_t n = 0; n < sz; ++n) {
      m_objects[n]->tick();
   }

// Collisions: see who collided and sound out their explosions.
// Note that the tick() calls above may have added objects after the size sz count, but this is OK.
   for (size_t n = 0; n < sz; ++n) for (size_t e = n + 1; e < sz && !m_objects[n]->dead(); ++e)
   if (m_collision(*m_objects[n], *m_objects[e])) {
   // When worlds collide!
   // Set rebound in motion.
      m_rebound(*m_objects[n], *m_objects[e], m_objects[n]->dir, m_objects[e]->dir);

   // Was this fatal?
      bool fn = m_objects[n]->fatal(*m_objects[e]);
      bool fe = m_objects[e]->fatal(*m_objects[n]);

   // Blow them up.
      if (fn) m_objects[n]->explode();
      if (fe) m_objects[e]->explode();

      if (fn || fe) {
      // Something exploded, was it a rock?
      // Set the largest explosion sound, if true.
         if (fn && m_objects[n]->rock())
            m_explosionSnd = m_objects[n]->type();
         if (fe && m_objects[e]->rock() && (m_explosionSnd == otNone || m_objects[e]->mass() > m_objects[n]->mass()))
            m_explosionSnd = m_objects[e]->type();

      // Did our ship explode yet?
         if ((fn && m_objects[n]->type() == otShip) || (fe && m_objects[e]->type() == otShip)) {
         // Oh dear, lost a ship.
            --m_lives;
            m_diedSnd = true;

         // Wait for another ship to arrive or time out at the end of the game.
            m_newLifeWait = time(0) + NEW_LIFE_PAUSE;
         }
      // Set the score and pointer to whatever object may have been shot.
         int as = 0;
         Obj *ak = nullptr;

         if (fe && m_objects[n]->type() == otFire) {
            as = m_objects[e]->score();
            if (m_objects[e]->type() == otAlien)
               ak = m_objects[e];
         } else if (fn && m_objects[e]->type() == otFire) {
            as = m_objects[n]->score();
            if (m_objects[n]->type() == otAlien)
               ak = m_objects[n];
         }
      // Have we shot an alien?
         if (ak != nullptr) {
         // Alien kill: label it.
            Obj *l = add(otLabel, ak->pos, ak->dir);
            l->fontSize(lfSmall);

         // Alien sound.
            m_alienSnd = true;

         // Label an extra life or score.
         // A score label generates a warning, when compiled under VC2005.
         // This is OK.
         //(@) Side note: itoa(), which was in the original, is not part of C++, and so has been replaced.
            if (Obj::randFloat() < 0.5) {
               ++m_lives;
               l->caption("EXTRA LIFE");
            } else {
               char s[100];
               ItoA(as, s, 10);
               l->caption(s);
            }
         }
      // Score the kill.
         m_score += as;
      }
   }
// Add a rock to the game, with probability prob.
   double prob = 2.0*m_diff*ROCK_PROB*(1.0 - 1.0/(1.0 + (double)m_tickCnt/HALF_MAX_TICK));

   if (Obj::randFloat() < prob)
      addKuypier(otBigRock, m_tickCnt);

// Add an alien to the game, with conditional probability prob.
// (Only one alien at a time may be present.)
   prob = ALIEN_PROB*(1.0 - 1.0/(1.0 + (double)m_tickCnt/HALF_MAX_TICK));

   if (Obj::randFloat() < prob && m_typeCnt(otAlien) == 0)
      addKuypier(otAlien, 0);

// Wrap the game space: update the size, as it may have changed.
   sz = m_objects.size();

// Check for strays outside the game space.
   for (size_t n = 0; n < sz; ++n) {
   // The Kuypier extra space (rocks and aliens may roam well off the screen).
      int kh = m_width/KUYP_DIV;
      int kv = m_height/KUYP_DIV;

   // Players are stuck on the screen.
      if (!m_objects[n]->kuypier()) {
         kh = 0;
         kv = 0;
      }
   // Reset the new position on the other side of the play area.
      ObjPos p(m_objects[n]->pos);

      if (p.real() < -kh) p = ObjPos(m_width + kh, p.imag());
      if (p.imag() < -kv) p = ObjPos(p.real(), m_height + kv);
      if (p.real() > m_width + kh) p = ObjPos(-kh, p.imag());
      if (p.imag() > m_height + kv) p = ObjPos(p.real(), -kv);

      m_objects[n]->pos = p;
   }
}

// The number of type t objects.
int Engine::m_typeCnt(OType t) const {
   int rslt = 0;
   size_t sz = m_objects.size();

   for (size_t n = 0; n < sz; ++n) {
      if (m_objects[n]->type() == t) {
         ++rslt;
      }
   }

   return rslt;
}

// A pointer to the ship.
Ship *Engine::m_getShip() const {
// We hold the index of the ship for fast lookup as we expect our routines to access the ship object many times.
// We check that our saved indexed indeed holds the ship index, and if not, we search for it.
   size_t sz = m_objects.size();

   if (m_shipIdx >= 0 && static_cast<size_t>(m_shipIdx) < sz && m_objects[m_shipIdx]->type() == otShip) {
   // Here we hold an index to it.
      return static_cast<Ship *>(m_objects[m_shipIdx]);
   } else for (size_t n = 0; n < sz; ++n) if (m_objects[n]->type() == otShip) {
   // Allow const method to set the property.
   // This is for optimization only, and I don't regard it as modifying the state of the object (much).
      *const_cast<int *>(&m_shipIdx) = n;
      return static_cast<Ship *>(m_objects[n]);
   }

   return nullptr;
}

// class Engine: public methods
// ────────────────────────────
// Make a new Engine object.
Engine::Engine() {
// The default playing area.
// See the playing area accessors for more information.
   m_width = 535;
   m_height = 400;

   m_shipIdx = -1;
   m_diff = 0.5;
   m_highScore = 0;
   m_score = 0;
   m_lastScore = 0;
   m_lives = 0;
   m_active = false;
   m_tickCnt = 0;
   m_newLifeWait = 0;
}

// Free an Engine object.
Engine::~Engine() {
   try {
      size_t sz = m_objects.size();
      for (size_t n = 0; n < sz; ++n) {
         delete m_objects[n];
      }
   } catch(...) { }
}

// Add a type-ot object.
Obj *Engine::add(OType ot) {
   Obj *newObj = nullptr;

   switch (ot) {
      case otBigRock: newObj = new BigRock(*this); break;
      case otMedRock: newObj = new MedRock(*this); break;
      case otSmallRock: newObj = new SmallRock(*this); break;
      case otShip: newObj = new Ship(*this); break;
      case otAlien: newObj = new Alien(*this); break;
      case otFire: newObj = new Fire(*this); break;
      case otDebris: newObj = new Debris(*this); break;
      case otSpark: newObj = new Spark(*this); break;
      case otThrust: newObj = new Thrust(*this); break;
      case otLabel: newObj = new Label(*this, DEF_TEXT_SECS); break;
      default:
#if 0
         assert(false); // Error.
#endif
      break;
   }

   if (newObj != nullptr) {
      m_objects.push_back(newObj);
   }

   return newObj;
}

// Add a type-ot object at pos with orientation dir.
Obj *Engine::add(OType ot, const ObjPos &pos, const ObjPos &dir) {
   Obj *ob = add(ot);
   ob->pos = pos;
   ob->dir = dir;
   return ob;
}

// Add a randomly-located object into the Kuypier region.
// The velocity may increase statistically according to the difficulty level and value of tick as the game goes on.
Obj *Engine::addKuypier(OType ot, int tick) {
   Obj *ob = add(ot);

// The orientation.
   double sc = 0.5 + MAX_SPEED*m_diff*(1.0 - 1.0/(1.0 + (double)tick/HALF_MAX_TICK));
   sc *= ROCK_SPEED_MULT;

   double x = sc*(-1.0 + 2.0*Obj::randFloat());
   double y = sc*(-1.0 + 2.0*Obj::randFloat());
   ob->dir = ObjPos(x, y);

// The position.
   if (Obj::randFloat() < 0.5) {
   // Place it either left or right.
      y = Obj::randFloat()*(m_height + 2*m_height/KUYP_DIV) - m_height/KUYP_DIV;
      x = Obj::randFloat() < 0.5? -m_width/KUYP_DIV/2: m_width + m_width/KUYP_DIV/2;
   } else {
   // Place it either top or bottom.
      x = Obj::randFloat()*(m_width + 2*m_width/KUYP_DIV) - m_width/KUYP_DIV;
      y = Obj::randFloat() < 0.5? -m_height/KUYP_DIV/2: m_height + m_height/KUYP_DIV/2;
   }

   ob->pos = ObjPos(x, y);

   return ob;
}

// The object count.
size_t Engine::objCnt() const {
   return m_objects.size();
}

// The object at index n.
Obj *Engine::objAtIdx(size_t n) const {
   return m_objects[n];
}

// The game mode: active versus demo.
bool Engine::active() const {
   return m_active;
}

// Is the game in demo?
bool Engine::demo() const {
   return (m_active && m_demoEndMark > 0);
}

// Is the game in play?
bool Engine::playing() const {
   return (active() && !demo());
}

// Test for ‟GAME OVER” after a short pause of its being set, to allow time for the label to be seen.
bool Engine::gameOver() const {
   return (!m_active || (m_gameOverMark > 0 && time(0) > m_gameOverMark));
}

// Start a new game.
void Engine::startGame(int rocks) {
   m_empty(true);

   m_tickCnt = 0;
   m_score = 0;
   m_gameOverMark = 0;
   m_active = true;
   m_demoEndMark = 0;
   m_initRocks = rocks;

// Add the start-up label.
   Obj *l = add(otLabel, ObjPos(m_width/2, m_height/2));
   l->fontSize(lfMedium);

   l->caption("NEW GAME");
   m_lives = 3;

// Setting this to non-zero will create new rocks and a ship after a short interval.
   m_newLifeWait = time(0) + NEW_LIFE_PAUSE;
}

// Start a demo game for secs seconds.
// An earlier version had an ‟Aliens” flag, to permit a demo with only flocking aliens.
void Engine::startDemo(time_t secs, int rocks) {
   m_empty(true);

   m_tickCnt = 0;
   m_score = 0;
   m_gameOverMark = 0;
   m_active = true;
   m_demoEndMark = time(0) + secs;
   m_initRocks = rocks;
   m_lives = 1;

// Add the start-up label.
   Obj *l = add(otLabel, ObjPos(m_width/2, m_height/2));
   l->fontSize(lfMedium);
   l->caption("DEMO");

// Setting this to non-zero will create new rocks and a ship after a short interval.
   m_newLifeWait = time(0) + NEW_LIFE_PAUSE;
}

// Clear: stop the game.
void Engine::stop() {
   m_active = false;
   m_lives = 0;
   m_empty(true);
}

// The Game State Machine, itself.
// This is meant to be called on the clock, so as to advance the game in 1/10 second increments.
// Graphics should be rendered between calls to tick().
void Engine::tick() {
   if (!m_active) return;
   if (demo()) {
   // Demo Mode: control the ship randomly.
      Ship *s = m_getShip();

      if (s != nullptr) {
      // Reset the fire lock.
         s->reload(true);

      // One in 3 chance of firing.
         if (Obj::randFloat() < 0.3) s->fire();

         if (Obj::randFloat() < 0.1) {
         // One in 10 chance of changing what ship was doing on last tick.
            s->rot(0);
            s->thrust(false);

         // New random action: 1/5 thrust, 3/10 rotate left, 3/10 rotate right, 1/5 do nothing.
            double ra = Obj::randFloat();

            if (ra < 0.2)
               s->thrust(true);
            else if (ra < 0.5)
               s->rot(-1);
            else if (ra < 0.8)
               s->rot(1);
         }
      }
   }
// Update the game objects.
   m_stateTick();

// Deal with high-score and restart events.
   if (!demo()) {
   // Update the score records.
      m_lastScore = m_score;

      if (m_score > m_highScore)
         m_highScore = m_score;
   }
// Implement the game start/end events.
   bool geflag = false;

// Do we need a new ship or to create initial rocks, etc.
   if (m_newLifeWait > 0 && time(0) > m_newLifeWait) {
   // Avoid repetitions.
      m_newLifeWait = 0;

   // Add a new ship and re-create the initial rocks or end the game if there are no lives left.
      if (m_lives > 0) {
         m_empty(false);

         for (int n = 0; n < m_initRocks; ++n)
            addKuypier(otBigRock, 0);

         add(otShip, ObjPos(m_width/2, m_height/2));
      } else {
         geflag = true;
      }
   }
// Demo timeout.
   geflag |= (demo() && time(0) > m_demoEndMark);

   if (geflag && m_gameOverMark == 0) {
   // Add the End-Of-Game label.
      m_gameOverMark = time(0) + END_GAME_PAUSE;

      Obj *l = add(otLabel, ObjPos(m_width/2, m_height/2));
      l->fontSize(lfLarge);
      l->caption("GAME OVER");
   }
}

// Get the number of lives.
int Engine::lives() const {
   return m_lives;
}

// Get the current score.
int Engine::score() const {
   return m_score;
}

// Get the last game score.
int Engine::lastScore() const {
   return m_lastScore;
}

// Get/set the high score.
// This can be set if the high score is stored between application launches.
int Engine::hiscore() const {
   return m_highScore;
}

void Engine::hiscore(int hs) {
   m_highScore = hs;
}

// Get/set the game level; higher = more difficult games.
double Engine::difficulty() const {
   return m_diff;
}

void Engine::difficulty(const double &dif) {
   if (dif > 0.0 && dif <= 1.0)
      m_diff = dif;
}

// Cheat: add an alien to the game.
void Engine::addAlienCheat() {
   if (m_active && m_typeCnt(otAlien) < 15)
      addKuypier(otAlien, 0);
}

// Rotate the ship (-1: left, 0: stop, +1: right).
void Engine::rotate(int r) {
   Ship *s = m_getShip();
   if (s != nullptr && !demo()) s->rot(r);
}

// Thrust-switcher.
void Engine::thrust(bool on) {
   Ship *s = m_getShip();
   if (s != nullptr && !demo()) s->thrust(on);
}

// Fire, release fire and ship fire charge.
void Engine::fire() {
   Ship *s = m_getShip();
   if (s != nullptr && !demo()) s->fire();
}

void Engine::reload() {
   Ship *s = m_getShip();
   if (s != nullptr && !demo()) s->reload(false);
}

int Engine::charge() const {
   Ship *s = m_getShip();
   return s != nullptr? s->fireCharge(): 0;
}

// Get the latest rock explosion (otNone = silent), fire, thrust, alien and death sounds.
OType Engine::rockExplodeSnd() const {
   return m_explosionSnd;
}

bool Engine::fireSnd() const {
   Ship *ship = m_getShip();
   return ship != nullptr && ship->justFired();
}

bool Engine::thrustSnd() const {
   Ship *ship = m_getShip();
   return ship != nullptr && ship->thrusting();
}

bool Engine::alienSnd() const {
   return m_alienSnd;
}

bool Engine::diedSnd() const {
   return m_diedSnd;
}

// Get/set the playing width and height.
// Note:
// ∙	Objects may occupy positions outside this region, and if so, should not be rendered (or DC clipped).
// ∙	The region outside *w and *h is known as the Kuypier area
//	and allows for asteroids to wander naturally into the game area.
// ∙	The ship cannot go into this region.
void Engine::getPlayDims(int *w, int *h) const {
   if (w != nullptr) *w = m_width;
   if (h != nullptr) *h = m_height;
}

void Engine::setPlayDims(int w, int h) {
   m_width = w;
   m_height = h;
}

// The minimum dimension.
int Engine::minDim() const {
   return m_height < m_width? m_height: m_width;
}
