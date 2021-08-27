//---------------------------------------------------------------------------
// PROJECT     : Asteroid Style Game
// FILE NAME   : game_engine.cpp
// DESCRIPTION : Engine holds & updates game artifacts
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
#include <math.h>
#include <stdlib.h>
#include "game_engine.h"

using namespace std;
using namespace asteroid;
//---------------------------------------------------------------------------
// CLASS Engine : PRIVATE METHODS
//---------------------------------------------------------------------------
void Engine::m_empty(bool killNow)
{
  // Empty the game of objects
  size_t sz = m_objects.size();

  if (killNow)
  {
    // Kill all life in space
    for(size_t n = 0; n < sz; ++n)
    {
      delete m_objects[n];
    }

    m_objects.clear();
  }
  else
  {
    // Mark as dead so they will
    // be deleted on next tick
    for(size_t n = 0; n < sz; ++n)
    {
      m_objects[n]->die();
    }
  }
}
//---------------------------------------------------------------------------
bool Engine::m_collision(const Obj& o1, const Obj& o2) const
{
  // Are these objects in collision?
  bool rslt = false;

    // Test that neither of objects is dead and
    // both have mass (massless objects are transparent)
  if (!o1.dead() && !o2.dead() &&
    o1.mass() > 0 && o2.mass() > 0)
  {
    // Are we residing in the same space (collision)?
    rslt = (abs(o1.pos - o2.pos) <= o1.radius() + o2.radius());
  }

  return rslt;
}
//---------------------------------------------------------------------------
void Engine::m_rebound(const Obj& o1, const Obj& o2,
  ObjPos& nd1, ObjPos& nd2) const
{
  // Rebound these objects
  // Intentionally take copies of directions,
  // as result reference maybe object directions.
  ObjPos d1(o1.dir);
  ObjPos d2(o2.dir);
  double m1(o1.mass());
  double m2(o2.mass());
  double m12(m1 + m2);

  if (m12 > 0.0 &&
    abs(o1.pos - o2.pos) > abs(o1.pos - o2.pos + (d1 - d2) * 0.1))
  {
    nd1 = (d1 * (m1 - m2) / m12) + (d2 * 2.0 * m2 / m12);
    nd2 = (d2 * (m2 - m1) / m12) + (d1 * 2.0 * m1 / m12);
    Obj::limitAbs(nd1, MAX_SPEED);
    Obj::limitAbs(nd2, MAX_SPEED);
  }
}
//---------------------------------------------------------------------------
void Engine::m_stateTick()
{
  // UPDATE INTERNAL
  // Updates object states according to their
  // positions & velocities. Also detects
  // collisions, handles rebounds & sets
  // sounds flags. The order in which events
  // are implemented is important.

  // Increment counter
  m_tickCnt++;
  if (m_tickCnt == 0x7FFFFFFF)
    m_tickCnt = 1000;

  // Set sound flags to zero will be set to
  // true below if required
  m_explosionSnd = otNone;
  m_diedSnd = false;
  m_alienSnd = false;

  // Destroy and remove objects which
  // are now dead from the previous tick
  size_t rn = 0;
  while(rn < m_objects.size())
  {
    if (m_objects[rn]->dead())
    {
      delete m_objects[rn];
      m_objects.erase(m_objects.begin() + rn);
    }
    else
    {
      ++rn;
    }
  }

  // Hold count because other objects
  // will be added as part of explosions
  size_t sz = m_objects.size();

  // TICK EACH OBJECT
  // Get objects to do their thing
  // in the next state tick.
  for(size_t n = 0; n < sz; ++n)
  {
    m_objects[n]->tick();
  }
  
  // COLLISIONS
  // See who has collided with who & set explosion sounds
  // Note that the tick() calls above may have added objects
  // after the size sz count, but this is ok.
  for(size_t n = 0; n < sz; ++n)
  {
    for(size_t e = n + 1; e < sz && !m_objects[n]->dead(); ++e)
    {
      if (m_collision(*m_objects[n], *m_objects[e]))
      {
        // When worlds collide!
        // Set rebound in motion
        m_rebound(*m_objects[n], *m_objects[e], m_objects[n]->dir, m_objects[e]->dir);

        // Was this fatal?
        bool fn = m_objects[n]->fatal(*m_objects[e]);
        bool fe = m_objects[e]->fatal(*m_objects[n]);

        // Blow them up
        if (fn) m_objects[n]->explode();
        if (fe) m_objects[e]->explode();

        if (fn || fe)
        {
          // Something exploded, was it a rock?
          // Set largest exposion sound if true
          if (fn && m_objects[n]->rock())
            m_explosionSnd = m_objects[n]->type();
          if ( fe && m_objects[e]->rock() &&
            (m_explosionSnd == otNone ||
              m_objects[e]->mass() > m_objects[n]->mass()) )
                m_explosionSnd = m_objects[e]->type();


          // Has out ship exploded?
          if ((fn && m_objects[n]->type() == otShip) ||
            (fe && m_objects[e]->type() == otShip))
          {
            // Oh dear, lost a ship
            --m_lives;
            m_diedSnd = true;

            // Wait for another shipt to arrive or time out at end of game
            m_newLifeWait = time(0) + NEW_LIFE_PAUSE;
          }

          // Did we shoot something?
          int as = 0;
          Obj* ak = 0;

          if (fe && m_objects[n]->type() == otFire)
          {
            // Hold exploded object score
            // and alien pointer if we have shot one
            as = m_objects[e]->score();
            if (m_objects[e]->type() == otAlien)
              ak = m_objects[e];
          }
          else
          if (fn && m_objects[e]->type() == otFire)
          {
            // Same as above, but for other object
            as = m_objects[n]->score();
            if (m_objects[n]->type() == otAlien)
              ak = m_objects[n];
          }

          // Have we shot an alien?
          if (ak != 0)
          {
            // Alien kill - add label
            Obj* l = add(otLabel, ak->pos, ak->dir);
            l->fontSize(lfSmall);

            // Alien sound
            m_alienSnd = true;

            // Extra life or score
            if (Obj::randFloat() < 0.5)
            {
              // Extra life label
              ++m_lives;
              l->caption("EXTRA LIFE");
            }
            else
            {
              // Show score label
              // Generates warning in VC2005. This is OK.
              // NB. GCC doesn't support _itoa_s().
              char s[100];
              itoa(as, s, 10);
              l->caption(s);
            }
          }

          // Increment score
          m_score += as;
        }
      }
    }
  }

  // ADD NEW OBJECTS TO GAME
  // Rock creation probability
  double prob = 2.0 * m_diff * ROCK_PROB *
    ( 1.0 - 1.0/(1.0 + (double)m_tickCnt/HALF_MAX_TICK) );

  // Add rock to game
  if (Obj::randFloat() < prob)
    addKuypier(otBigRock, m_tickCnt);

  // Alien creation probability
  prob = ALIEN_PROB * ( 1.0 - 1.0/(1.0 + (double)m_tickCnt/HALF_MAX_TICK) );

  // Add alien to game
  if (Obj::randFloat() < prob && m_typeCnt(otAlien) == 0)
    addKuypier(otAlien, 0);

  // WRAP GAME SPACE
  // Update size as it may have changed
  sz = m_objects.size();

  // Who's strayed outside of game space?
  for(size_t n = 0; n < sz; ++n)
  {
    // Kuypier extra space (let rocks & aliens roam well off screen)
    int kh = m_width / KUYP_DIV;
    int kv = m_height / KUYP_DIV;

    if (!m_objects[n]->kuypier())
    {
      // Mere humans have to live on screen
      kh = 0;
      kv = 0;
    }

    // Reset new position on other side of play area
    ObjPos p(m_objects[n]->pos);

    if (p.real() < -kh) p = ObjPos(m_width + kh, p.imag());
    if (p.imag() < -kv) p = ObjPos(p.real(), m_height + kv);
    if (p.real() > m_width + kh) p = ObjPos(-kh, p.imag());
    if (p.imag() > m_height + kv) p = ObjPos(p.real(), -kv);

    m_objects[n]->pos = p;
  }
}
//---------------------------------------------------------------------------
int Engine::m_typeCnt(OType t) const
{
  // Return count of particular object type
  int rslt = 0;
  size_t sz = m_objects.size();
  
  for(size_t n = 0; n < sz; ++n)
  {
    if (m_objects[n]->type() == t)
    {
      ++rslt;
    }
  }

  return rslt;
}
//---------------------------------------------------------------------------
Ship* Engine::m_getShip() const
{
  // Find the ship & return pointer to it.
  // We hold the index of the ship for fast
  // lookup as we expect our routines to access
  // the ship object many times. We check that
  // our saved indexed indeed holds the ship
  // index, and if not, we search for it.
  size_t sz = m_objects.size();
  
  if (m_shipIdx >= 0 && static_cast<size_t>(m_shipIdx) < sz &&
    m_objects[m_shipIdx]->type() == otShip)
  {
    // Here we hold an index to 
    return static_cast<Ship*>(m_objects[m_shipIdx]);
  }
  else
  {
    for(size_t n = 0; n < sz; ++n)
    {  
      if (m_objects[n]->type() == otShip)
      {
        // Allow const method to set property
        // This is for optimization only, and
        // I don't regard it as modifying the
        // state of the object (much).
        *const_cast<int*>(&m_shipIdx) = n;
        return static_cast<Ship*>(m_objects[n]);
      }
    }
  }
  
  return 0;
}
//---------------------------------------------------------------------------
// CLASS Engine : PUBLIC METHODS
//---------------------------------------------------------------------------
Engine::Engine()
{
  // Constructor

  // Default playing area
  // See playing area accessors for info
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
//---------------------------------------------------------------------------
Engine::~Engine()
{
  // Destructor
  try
  {
    size_t sz = m_objects.size();
    for(size_t n = 0; n < sz; ++n)
    {  
      delete m_objects[n];
    }
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
Obj* Engine::add(OType ot)
{
  // Add object
  Obj* newObj = 0;

  switch(ot)
  {
    case otBigRock:
      newObj = new BigRock(*this);
      break;
    case otMedRock:
      newObj = new MedRock(*this);
      break;
    case otSmallRock:
      newObj = new SmallRock(*this);
      break;
    case otShip:
      newObj = new Ship(*this);
      break;
    case otAlien:
      newObj = new Alien(*this);
      break;
    case otFire:
      newObj = new Fire(*this);
      break;
    case otDebris:
      newObj = new Debris(*this);
      break;
    case otSpark:
      newObj = new Spark(*this);
      break;
    case otThrust:
      newObj = new Thrust(*this);
      break;
    case otLabel:
      newObj = new Label(*this, DEF_TEXT_SECS);
      break;
    default: // Error
      break;
  }

  // Done
  if (newObj != 0)
  {
    m_objects.push_back(newObj);
  }
  
  return newObj;
}
//---------------------------------------------------------------------------
Obj* Engine::add(OType ot, const ObjPos& pos,
  const ObjPos& dir)
{
  // Add object & set properties from pos & dir
  Obj* ob = add(ot);
  ob->pos = pos;
  ob->dir = dir;
  return ob;
}
//---------------------------------------------------------------------------
Obj* Engine::addKuypier(OType ot, int tick)
{
  // Add object in to kuypier region & initialise with
  // random position. Velocity may increase statistically
  // according to difficulty level & value of "tick" as
  // the game goes on.
  Obj* ob = add(ot);

  // Direction
  double sc = 0.5 + MAX_SPEED * m_diff *
    ( 1.0 - 1.0/(1.0 + (double)tick/HALF_MAX_TICK) );
  sc *= ROCK_SPEED_MULT;

  double x = sc * (-1.0 + 2.0 * Obj::randFloat());
  double y = sc * (-1.0 + 2.0 * Obj::randFloat());
  ob->dir = ObjPos(x, y);

  // Position
  if (Obj::randFloat() < 0.5)
  {
    // Place either left or right
    y = Obj::randFloat() * (m_height + (2 * m_height / KUYP_DIV)) - m_height / KUYP_DIV;

    if (Obj::randFloat() < 0.5)
      x = -m_width / KUYP_DIV / 2;
    else
      x = m_width + m_width / KUYP_DIV / 2;
  }
  else
  {
    // Place either top or bottom
    x = Obj::randFloat() * (m_width + (2 * m_width / KUYP_DIV)) - m_width / KUYP_DIV;

    if (Obj::randFloat() < 0.5)
      y = -m_height / KUYP_DIV / 2;
    else
      y = m_height + m_height / KUYP_DIV / 2;
  }

  ob->pos = ObjPos(x, y);

  return ob;
}
//---------------------------------------------------------------------------
size_t Engine::objCnt() const
{
  // Return object count
  return m_objects.size();
}
//---------------------------------------------------------------------------
Obj* Engine::objAtIdx(size_t n) const
{
  // Get object at index n
  return m_objects[n];
}
//---------------------------------------------------------------------------
bool Engine::active() const
{
  // Is game active or in demo mode
  return m_active;
}
//---------------------------------------------------------------------------
bool Engine::demo() const
{
  // Accessor
  return (m_active && m_demoEndMark > 0);
}
//---------------------------------------------------------------------------
bool Engine::playing() const
{
  // Is game in play?
  return (active() && !demo());
}
//---------------------------------------------------------------------------
bool Engine::gameOver() const
{
  // Accessor - game over after short pause of being set
  // Allows time for "GAME OVER" label to be seen
  return (!m_active || (m_gameOverMark > 0 && time(0) > m_gameOverMark));
}
//---------------------------------------------------------------------------
void Engine::startGame(int rocks)
{
  // Start new game
  m_empty(true);

  m_tickCnt = 0;
  m_score = 0;
  m_gameOverMark = 0;
  m_active = true;
  m_demoEndMark = 0;
  m_initRocks = rocks;

  // Add start-up label
  Obj* l = add(otLabel, ObjPos(m_width / 2, m_height / 2));
  l->fontSize(lfMedium);

  l->caption("NEW GAME");
  m_lives = 3;

  // Setting this to non-zero will create
  // new rocks & ship in short interval
  m_newLifeWait = time(0) + NEW_LIFE_PAUSE;
}
//---------------------------------------------------------------------------
void Engine::startDemo(time_t secs, int rocks)
{
  // Start demo game
  // If aliens true, only flocking aliens are shown
  // secs is seconds before gameOver() becomes true
  m_empty(true);

  m_tickCnt = 0;
  m_score = 0;
  m_gameOverMark = 0;
  m_active = true;
  m_demoEndMark = time(0) + secs;
  m_initRocks = rocks;
  m_lives = 1;

  // Add start-up label
  Obj* l = add(otLabel, ObjPos(m_width / 2, m_height / 2));
  l->fontSize(lfMedium);
  l->caption("DEMO");

  // Setting this to non-zero will create
  // new rocks & ship in short interval
  m_newLifeWait = time(0) + NEW_LIFE_PAUSE;
}
//---------------------------------------------------------------------------
void Engine::stop()
{
  // Clear - stop the game
  m_active = false;
  m_lives = 0;
  m_empty(true);
}
//---------------------------------------------------------------------------
void Engine::tick()
{
  // GAME LOGIC IMPLEMENTATION. This method should be called
  // by a timer, around 10 times a second, in order to update
  // game state. Graphics should be rendered between calls to tick.
  if (m_active)
  {
    if (demo())
    {
      // DEMO MODE
      // Control ship in a randomised manner
      Ship* s = m_getShip();

      if (s != 0)
      {
        // Reset fire lock
        s->reload(true);

        // One in 3 chance of firing
        if (Obj::randFloat() < 0.3) s->fire();

        if (Obj::randFloat() < 0.1)
        {
          // One in 10 chance of changing what ship
          // was doing on last tick
          s->rot(0);
          s->thrust(false);

          // New random action
          double ra = Obj::randFloat();

          if (ra < 0.2)
            s->thrust(true);
          else
          if (ra < 0.5)
            s->rot(-1);
          else
          if (ra < 0.8)
            s->rot(1);
        }
      }
    }

    // UPDATE ALL GAME OBJECTS
    m_stateTick();

    // DEAL WITH HISCORES & RESTART EVENTS
    if (!demo())
    {
      // Update score records
      m_lastScore = m_score;

      if (m_score > m_highScore)
        m_highScore = m_score;
    }

    // Implement game start/end events
    bool geflag = false;

    // Do we need a new ship or to create initial rocks etc
    if (m_newLifeWait > 0 && time(0) > m_newLifeWait)
    {
      // Reset this so it doesn't re-occur
      m_newLifeWait = 0;

      if (m_lives > 0)
      {
        // Add new ship & recreate initial rocks
        m_empty(false);

        for(int n = 0; n < m_initRocks; ++n)
          addKuypier(otBigRock, 0);

        add(otShip, ObjPos(m_width / 2, m_height / 2));
      }
      else
      {
        // Game should end as no lives
        geflag = true;
      }
    }

    // Demo timeout
    geflag |= (demo() && time(0) > m_demoEndMark);

    if (geflag && m_gameOverMark == 0)
    {
      // End game label
      m_gameOverMark = time(0) + END_GAME_PAUSE;

      Obj* l = add(otLabel, ObjPos(m_width / 2, m_height / 2));
      l->fontSize(lfLarge);
      l->caption("GAME OVER");
    }
  }
}
//---------------------------------------------------------------------------
int Engine::lives() const
{
  // Lives accessor
  return m_lives;
}
//---------------------------------------------------------------------------
int Engine::score() const
{
  // Score accessor
  return m_score;
}
//---------------------------------------------------------------------------
int Engine::lastScore() const
{
  // Last game score accessor
  return m_lastScore;
}
//---------------------------------------------------------------------------
int Engine::hiscore() const
{
  // High score accessor
  return m_highScore;
}
//---------------------------------------------------------------------------
void Engine::hiscore(int hs)
{
  // High score mutator, this can be
  // set if high score is stored between application launches
  m_highScore = hs;
}
//---------------------------------------------------------------------------
double Engine::difficulty() const
{
  // Higher values result in more difficult games
  return m_diff;
}
//---------------------------------------------------------------------------
void Engine::difficulty(const double& dif)
{
  // Higher values result in more difficult games
  if (dif > 0.0 && dif <= 1.0)
    m_diff = dif;
}
//---------------------------------------------------------------------------
void Engine::addAlienCheat()
{
  // Cheat - add alien to game
  if (m_active && m_typeCnt(otAlien) < 15)
    addKuypier(otAlien, 0);
}
//---------------------------------------------------------------------------
void Engine::rotate(int r)
{
  // Rotate ship (-1 left, 0 stop, +1 right)
  Ship* s = m_getShip();
  if (s != 0 && !demo()) s->rot(r);
}
//---------------------------------------------------------------------------
void Engine::thrust(bool on)
{
  // Thrust on or off
  Ship* s = m_getShip();
  if (s != 0 && !demo()) s->thrust(on);
}
//---------------------------------------------------------------------------
void Engine::fire()
{
  // Fire
  Ship* s = m_getShip();
  if (s != 0 && !demo()) s->fire();
}
//---------------------------------------------------------------------------
void Engine::reload()
{
  // Release fire
  Ship* s = m_getShip();
  if (s != 0 && !demo()) s->reload(false);
}
//---------------------------------------------------------------------------
int Engine::charge() const
{
  // Ship fire charge
  Ship* s = m_getShip();
  if (s != 0)
    return s->fireCharge();
  else
    return 0;
}
//---------------------------------------------------------------------------
OType Engine::rockExplodeSnd() const
{
  // Access latest rock explosion sound
  // Return otNone for silent
  return m_explosionSnd;
}
//---------------------------------------------------------------------------
bool Engine::fireSnd() const
{
  // Access latest sound
  Ship* ship = m_getShip();

  if (ship != 0)
    return ship->justFired();
  else
    return false;
}
//---------------------------------------------------------------------------
bool Engine::thrustSnd() const
{
  // Access latest sound
  Ship* ship = m_getShip();

  if (ship != 0)
    return ship->thrusting();
  else
    return false;
}
//---------------------------------------------------------------------------
bool Engine::alienSnd() const
{
  // Access latest sound
  return m_alienSnd;
}
//---------------------------------------------------------------------------
bool Engine::diedSnd() const
{
  // Access latest sound
  return m_diedSnd;
}
//---------------------------------------------------------------------------
void Engine::getPlayDims(int* w, int* h) const
{
  // Get playing width & height.
  // Note. Objects may occupy positions
  // outside this region, and if so, should
  // not be rendered (or DC clipped). Region
  // outside w & h is known as Kuypier area and
  // allows for asteroids to wander naturally
  // into game area. Ship cannot occupy not
  // region outside this.
  if (w != 0) *w = m_width;
  if (h != 0) *h = m_height;
}
//---------------------------------------------------------------------------
void Engine::setPlayDims(int w, int h)
{
  // Set playing width & height.
  m_width = w;
  m_height = h;
}
//---------------------------------------------------------------------------
int Engine::minDim() const
{
  // Return minum dimension
  if (m_height < m_width)
    return m_height;
  else
    return m_width;
}
//---------------------------------------------------------------------------
