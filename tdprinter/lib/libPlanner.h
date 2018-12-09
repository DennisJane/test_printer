#pragma once

#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a<b)?a:b)


float speedAfterAccel(float curSpeed,float accelSpeed,float distance);
void calculate_trapezoid_for_block(motionLine_t *block, float entryFactor, float exitFactor);
void planner_recalculate(stepBlock_t *sb);

