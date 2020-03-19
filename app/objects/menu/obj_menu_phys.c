//
// Created by maxim on 1/28/20.
//

#ifdef __GNUC__
#pragma implementation "obj_menu_phys.h"
#endif
#include "obj_menu_phys.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

#include <chipmunk/chipmunk.h>

static bool freed_phys;
static cpBody* spheres[SPHERES_COUNT];
static cpShape* sphere_shapes[SPHERES_COUNT];

static cpBody* car1 = NULL;
static cpBody* car2 = NULL;

static cpShape* car1_shape = NULL;
static cpShape* car2_shape = NULL;
static cpShape* bound1_shape = NULL;
static cpShape* bound2_shape = NULL;

static cpSpace * space = NULL;
static const cpFloat time_step = 1.0 / FPS_TO_LOCK;

object_t* render_spheres[SPHERES_COUNT];
object_t* render_car1 = NULL;
object_t* render_car2 = NULL;

static void move_car(cpBody* car, cpVect dest)
{
   cpVect pos = cpBodyGetPosition(car);
   //cpVect diff = cpv(pos.x - dest.x, pos.y - dest.y);
   //cpFloat turn = cpvtoangle(cpvunrotate(cpBodyGetRotation(car), mouseDelta));

   cpVect mouseDelta = cpvsub(dest, cpBodyGetPosition(car));

   if(cpvnear(dest, cpBodyGetPosition(car), 0.0)){
      //cpBodySetVelocity(car1, cpvzero); // stop
   } else {
      cpFloat direction = (cpvdot(mouseDelta, cpBodyGetRotation(car)) > 0.0 ? 1.0 : -1.0);
      cpVect v = cpvrotate(cpBodyGetRotation(car), cpv(40.0f * direction, 0.0f));

      float delta = (float)u_get_delta();
      v = cpvmult(v, rand_range(20, 40) / delta);



      cpBodyApplyForceAtWorldPoint(car, v,  pos);
   }
}

static void update_menu_phys(object_t* this)
{
   float delta = (float)u_get_delta();

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
      cpVect speed = cpBodyGetVelocity(spheres[i]);
      render_spheres[i]->position = vec3f(pos.x, SPHERE_RADIUS / 2.0f, pos.y);
      render_spheres[i]->rotation.x += -speed.y / 120;
      render_spheres[i]->rotation.y += -speed.x / 120;
   }

   cpVect pos1 = cpBodyGetPosition(car1);
   cpVect pos2 = cpBodyGetPosition(car2);

   render_car1->position = vec3f(pos1.x, 0, pos1.y);
   render_car1->rotation = vec3f(0, cpBodyGetAngle(car1), 0);
   render_car2->position = vec3f(pos2.x, 0, pos2.y);
   render_car2->rotation = vec3f(0, cpBodyGetAngle(car2), 0);

   move_car(car1, cpv(rand_range(2, 70), rand_range(2, 70)));
   move_car(car2, pos1);

   cpSpaceStep(space, time_step * delta);
}

static cpBody* createCar(cpVect pos, cpShape** shape)
{
   cpFloat mass = 5;
   cpFloat moment = cpMomentForBox(mass, CAR_SIZE, CAR_SIZE / 2.0f);
   cpBody* car = cpSpaceAddBody(space, cpBodyNew(mass, moment));

   cpBodySetPosition(car, pos);

   *shape = cpSpaceAddShape(space, cpBoxShapeNew(car, CAR_SIZE, CAR_SIZE / 2.0f, 0));
   cpShapeSetFriction(*shape, 2);
   return car;
}

static void free_menu_phys(object_t* object)
{
   if(!freed_phys)
   {
      cpSpaceFree(space);

      for (size_t i = 0; i < SPHERES_COUNT; i++)
         cpShapeFree(sphere_shapes[i]);

      cpShapeFree(car1_shape);
      cpShapeFree(car2_shape);

      cpShapeFree(bound1_shape);
      cpShapeFree(bound2_shape);

      freed_phys = true;
   }
}

object_t* create_menu_phys(void)
{
   freed_phys = false;
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

      bound1_shape = cpSpaceAddShape(space, cpSegmentShapeNew(cpSpaceGetStaticBody(space),
                                                                cpv(x1, y1), cpv(x2, y2), 0.0f));
      cpShapeSetElasticity(bound1_shape, 1.0f);
      cpShapeSetFriction(bound1_shape, 1.0f);
   }

   bound_r = 13;
   for(int i = 0; i < 100; i++)
   {
      float angle = diff * (float)i;
      float x1 = bound_r * sinf(angle);
      float x2 = bound_r * sinf(angle + diff);
      float y1 = bound_r * cosf(angle);
      float y2 = bound_r * cosf(angle + diff);

      bound2_shape = cpSpaceAddShape(space, cpSegmentShapeNew(cpSpaceGetStaticBody(space),
                                                                cpv(x1, y1), cpv(x2, y2), 0.0f));

      cpShapeSetElasticity(bound2_shape, 1.0f);
      cpShapeSetFriction(bound2_shape, 1.0f);
   }

   //creating spheres
   for(int i = 0; i < SPHERES_COUNT; i++)
   {
      cpFloat mass = 1;
      cpFloat moment = cpMomentForCircle(mass, 0, SPHERE_RADIUS, cpvzero);
      cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));

      float xoff = rand_b() ? rand_range(20, 40) : -rand_range(20, 40);
      float yoff = rand_b() ? rand_range(20, 40) : -rand_range(20, 40);

      cpBodySetPosition(ballBody, cpv(xoff,yoff));
      sphere_shapes[i] = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, SPHERE_RADIUS / 2.0, cpvzero));
      cpShapeSetFriction(sphere_shapes[i], 0.4);
      spheres[i] = ballBody;
   }

   car1 = createCar(cpv(25, 25), &car1_shape);
   car2 = createCar(cpv(-25, -25), &car2_shape);

   object_t* this = o_create();
   this->update_func = update_menu_phys;
   this->destroy_func = free_menu_phys;

   return this;
}

#pragma clang diagnostic pop