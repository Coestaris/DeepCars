//
// Created by maxim on 3/26/20.
//
#include "obj_editor_map.h"

bool get_intersection(double ray_x1, double ray_y1, double ray_x2, double ray_y2, vec2 seg1, vec2  seg2,
                      double* rx, double* ry, double* dist)
{
   double r_px = ray_x1;
   double r_py = ray_y1;
   double r_dx = ray_x2 - ray_x1;
   double r_dy = ray_y2 - ray_y1;

   double s_px = seg1.x;
   double s_py = seg1.y;
   double s_dx = seg2.x - seg1.x;
   double s_dy = seg2.y - seg1.y;

   double r_mag = sqrt(r_dx * r_dx + r_dy * r_dy);
   double s_mag = sqrt(s_dx * s_dx + s_dy * s_dy);

   if (fabs(r_dx / r_mag - s_dx / s_mag) < GET_INTERSECTION_COMP &&
       fabs(r_dy / r_mag - s_dy / s_mag) < GET_INTERSECTION_COMP)
   {
      return false;
   }

   double T2 = (r_dx * (s_py - r_py) + r_dy * (r_px - s_px)) / (s_dx * r_dy - s_dy * r_dx);
   double T1 = (s_px + s_dx * T2 - r_px) / r_dx;

   if (T1 < 0) return false;
   if (T2 < 0 || T2 > 1) return false;

   *rx = r_px + r_dx * T1;
   *ry = r_py + r_dy * T1;
   *dist = T1;

   return true;
}


// Он какбы пока не нужен, но удалять слишком жалко(((
bool check_intersection(double ray_x1, double ray_y1, double ray_x2, double ray_y2, vec2 seg1, vec2 seg2)
{
   double r_px = ray_x1;
   double r_py = ray_y1;
   double r_dx = ray_x2 - ray_x1;
   double r_dy = ray_y2 - ray_y1;

   double s_px = seg1.x;
   double s_py = seg1.y;
   double s_dx = seg2.x - seg1.x;
   double s_dy = seg2.y - seg1.y;

   if ((fabs(r_dx)) < GET_INTERSECTION_COMP) // Vertical vector
   {
      double min_sx = fmin(seg1.x, seg2.x);
      double min_sy = fmin(seg1.y, seg2.y);
      double min_ry = fmin(ray_y1, ray_y2);
      double max_sx = fmax(seg1.x, seg2.x);
      double max_sy = fmax(seg1.y, seg2.y);
      double max_ry = fmax(ray_y1, ray_y2);

      if (fabs(s_dx) < GET_INTERSECTION_COMP) return false; // Parallel
      else if(fabs(s_dy) < GET_INTERSECTION_COMP)
         return max_sx > r_px && r_px > min_sx && max_ry > s_py && s_py > min_ry; // Normal
      else // Other
      {
         vec2 norm = vec2_normalize(vec2f(s_dx, s_dy));
         double y = (min_sx - norm.x * r_px) / norm.y;
         return (max_sy > y && y > min_sy && max_ry > y && y > min_ry);
      }
   }

   if ((fabs(r_dy)) < GET_INTERSECTION_COMP) // Horizontal vector
   {
      double min_sx = fmin(seg1.x, seg2.x);
      double min_sy = fmin(seg1.y, seg2.y);
      double min_rx = fmin(ray_x1, ray_x2);
      double max_sx = fmax(seg1.x, seg2.x);
      double max_sy = fmax(seg1.y, seg2.y);
      double max_rx = fmax(ray_x1, ray_x2);

      if (fabs(s_dy) < GET_INTERSECTION_COMP) return false; // Parallel
      else if(fabs(s_dx) < GET_INTERSECTION_COMP)
         return max_sy > r_py && r_py > min_sy && max_rx > s_px && s_px > min_rx; // Normal
      else // Other
      {
         vec2 norm = vec2_normalize(vec2f(s_dx, s_dy));
         double x = (min_sy - norm.y * r_py) / norm.x;
         return (max_sx > x && x > min_sx && max_rx > x && x > min_rx);
      }
   }

   return get_intersection(ray_x1, ray_y1, ray_x2, ray_y2, seg1, seg2, &r_px, &r_py, &r_dx);
}
