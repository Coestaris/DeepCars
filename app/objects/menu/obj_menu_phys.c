//
// Created by maxim on 1/28/20.
//

#ifdef __GNUC__
#pragma implementation "obj_menu_phys.h"
#endif
#include "obj_menu_phys.h"

#include <chipmunk/chipmunk.h>

cpBody* spheres[SPHERES_COUNT];
object_t* render_spheres[SPHERES_COUNT];
object_t* render_car1;
object_t* render_car2;

cpBody* car1;
cpBody* car2;

cpSpace * space;
cpFloat time_step = 1.0 / FPS_TO_LOCK;

void move(cpBody* car, cpVect dest)
{
   cpVect pos = cpBodyGetPosition(car);

   cpVect diff = cpv(pos.x - dest.x, pos.y - dest.y);
   cpvnormalize(diff);
   cpvmult(diff, -1.5);

   cpVect mouseDelta = cpvsub(dest, cpBodyGetPosition(car));
   cpFloat turn = cpvtoangle(cpvunrotate(cpBodyGetRotation(car), mouseDelta));

   if(cpvnear(dest, cpBodyGetPosition(car), 30.0)){
      //cpBodySetVelocity(car1, cpvzero); // stop
   } else {
      cpFloat direction = (cpvdot(mouseDelta, cpBodyGetRotation(car)) > 0.0 ? 1.0 : -1.0);
      cpVect v = cpvrotate(cpBodyGetRotation(car), cpv(30.0f*direction, 0.0f));
      v = cpvmult(v, 30);

      cpBodyApplyForceAtWorldPoint(car, v,  pos);
   }
}

void update_menu_phys(object_t* this)
{
   if(u_get_frames() == 1)
   {
      u_push_object(render_car1);
      u_push_object(render_car2);
      for(size_t i = 0; i < SPHERES_COUNT; i++)
         u_push_object(render_spheres[i]);
   }

   for(size_t i = 0; i < SPHERES_COUNT; i++)
   {
      cpVect pos = cpBodyGetPosition(spheres[i]);
      render_spheres[i]->position = vec3f(pos.x, 0, pos.y);
   }

   cpVect pos1 = cpBodyGetPosition(car1);
   cpVect pos2 = cpBodyGetPosition(car2);

   render_car1->position = vec3f(pos1.x, 0, pos1.y);
   render_car1->rotation = vec3f(0, cpBodyGetAngle(car1), 0);
   render_car2->position = vec3f(pos2.x, 0, pos2.y);
   render_car2->rotation = vec3f(0, cpBodyGetAngle(car2), 0);

   move(car1, cpv(drand48() * 70, drand48() * 70));
   move(car2, pos1);

   cpSpaceStep(space, time_step);
}

cpBody* createCar(cpVect pos)
{
   cpFloat mass = 5;
   cpFloat moment = cpMomentForBox(mass, CAR_SIZE, CAR_SIZE / 2.0f);
   cpBody* car = cpSpaceAddBody(space, cpBodyNew(mass, moment));

   cpBodySetPosition(car, pos);

   cpShape* shape = cpSpaceAddShape(space, cpBoxShapeNew(car, CAR_SIZE, CAR_SIZE / 2.0f, 0));
   cpShapeSetFriction(shape, 2);
   return car;
}

object_t* create_menu_phys(void)
{
  space = cpSpaceNew();

   cpVect gravity = cpv(0, 0);
   cpSpaceSetGravity(space, gravity);
   cpSpaceSetSleepTimeThreshold(space, 0.5f);

   // creating bounds
   float bound_r = 65.0f;
   float diff = 2.0f * M_PI / 100.0f;
   for(int i = 0; i < 100; i++)
   {
      float angle = diff * (float)i;
      float x1 = bound_r * sinf(angle);
      float x2 = bound_r * sinf(angle + diff);
      float y1 = bound_r * cosf(angle);
      float y2 = bound_r * cosf(angle + diff);

      cpShape* shape = cpSpaceAddShape(space, cpSegmentShapeNew(cpSpaceGetStaticBody(space),
                                                                cpv(x1, y1), cpv(x2, y2), 0.0f));

      cpShapeSetElasticity(shape, 1.0f);
      cpShapeSetFriction(shape, 1.0f);
   }

   //creating spheres
   for(int i = 0; i < SPHERES_COUNT; i++)
   {
      cpFloat mass = 1;
      cpFloat moment = cpMomentForCircle(mass, 0, SPHERE_RADIUS, cpvzero);
      cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
      cpBodySetPosition(ballBody, cpv(drand48() * 100 - 50,drand48() * 100 - 50));
      cpShape *ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, SPHERE_RADIUS / 2.0, cpvzero));
      cpShapeSetFriction(ballShape, 0.4);
      spheres[i] = ballBody;
   }

   car1 = createCar(cpv(5, 5));
   car2 = createCar(cpv(-5, -5));

   object_t* this = o_create();
   this->update_func = update_menu_phys;

   return this;
}
