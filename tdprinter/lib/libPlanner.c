#include "libStepBlock.h"
#include <math.h>
#include "libPlanner.h"

#define CRITICAL_SECTION_START
#define CRITICAL_SECTION_END

// (mm/sec)
#define MINIMUM_PLANNER_SPEED 0.05f

static int8_t next_block_index(int8_t block_index) {
	block_index++;
	if (block_index == MOTION_BUFFER_SIZE) { 
		block_index = 0; 
	}
	return(block_index);
}


// Returns the index of the previous block in the ring buffer
static int8_t prev_block_index(int8_t block_index) {
	if (block_index == 0) { 
		block_index = MOTION_BUFFER_SIZE; 
	}
	block_index--;
	return(block_index);
}

static int8_t block_size(int8_t head,int8_t tail){
	return (head - tail + MOTION_BUFFER_SIZE) & (MOTION_BUFFER_SIZE - 1);
}

float estimate_accelSpeed_distance(float initial_rate, float target_rate, float accelSpeed) {
	if (accelSpeed!=0) {
		return((target_rate*target_rate-initial_rate*initial_rate)/(2.0f*accelSpeed));
	} else {
		return 0.0;  // accelSpeed was 0, set accelSpeed distance to 0
	}
}

float intersection_distance(float initial_rate, float final_rate, float accelSpeed, float distance) {
	if (accelSpeed!=0) {
		return((2.0f*accelSpeed*distance-initial_rate*initial_rate+final_rate*final_rate)/
		       (4.0f*accelSpeed) );
	} else {
		return 0.0;  // accelSpeed was 0, set intersection distance to 0
	}
}

#ifdef ADVANCE
float max_allowable_speed(float accelSpeed, float target_velocity, float distance) {
	return sqrtf(target_velocity*target_velocity-2.0f*accelSpeed*distance);
}
#endif
float speedAfterAccel(float curSpeed,float accelSpeed,float distance) {
	float dstSpeedSquared=curSpeed*curSpeed + 2.0f*accelSpeed*distance;
	float dstSpeed;
	if(dstSpeedSquared>0){
		dstSpeed=sqrtf(dstSpeedSquared);
	}else{
		dstSpeed=-sqrtf(-dstSpeedSquared);
	}
	return dstSpeed;
}

//block->planner_recalculate_param ==> block->trapezoid_accel_param
//input *block,block->nominalRate,block->accelRate,entryFactor,exitFactor
//output block->entryRate,block->exitRate,block->accelBefore,block->decelAfter
void calculate_trapezoid_for_block(motionLine_t *block, float entryFactor, float exitFactor) {
	float entryRate = block->nominalRate*entryFactor;	//steps/s
	float exitRate = block->nominalRate*exitFactor;
	float accelRate = block->accelRate;
	
	// Limit minimal step rate (Otherwise the timer will overflow.)
	if(entryRate <120) {
		entryRate=120;
	}
	if(exitRate < 120) {
		exitRate=120;
	}

	int32_t accelerate_steps = ceil(estimate_accelSpeed_distance(entryRate, block->nominalRate, accelRate));
	int32_t decelerate_steps = floor(estimate_accelSpeed_distance(block->nominalRate, exitRate, -accelRate));

	// Calculate the size of Plateau of Nominal Rate.
	int32_t plateau_steps = block->masterStep - accelerate_steps - decelerate_steps;

	// Is the Plateau of Nominal Rate smaller than nothing? That means no cruising, and we will
	// have to use intersection_distance() to calculate when to abort accelSpeed and start braking
	// in order to reach the final_rate exactly at the end of this block.
	if (plateau_steps < 0) {
		accelerate_steps = ceil(intersection_distance(entryRate, exitRate, accelRate, block->masterStep));
		accelerate_steps = MAX(accelerate_steps,0); // Check limits due to numerical round-off
		accelerate_steps = MIN((uint32_t)accelerate_steps,block->masterStep);//(We can cast here to unsigned, because the above line ensures that we are above zero)
		plateau_steps = 0;
	}

#ifdef ADVANCE
	volatile long initial_advance = block->advance*entryFactor*entryFactor;
	volatile long final_advance = block->advance*exitFactor*exitFactor;
#endif // ADVANCE

	CRITICAL_SECTION_START;  // Fill variables used by the stepper in a critical section
	if(block->busy == false) { // Don't update variables if block is busy.
		block->accelBefore = accelerate_steps;
		block->decelAfter  = accelerate_steps + plateau_steps;
		block->entryRate = entryRate;
		block->exitRate = exitRate;
#ifdef ADVANCE
		block->initial_advance = initial_advance;
		block->final_advance = final_advance;
#endif //ADVANCE
	}
	CRITICAL_SECTION_END;
}

// The kernel called by planner_recalculate() when scanning the plan from last to first entry.
void planner_reverse_pass_kernel(motionLine_t *previous, motionLine_t *current, motionLine_t *next) {
	if(!current) {
		return;
	}

	if (next) {
		// If entry speed is already at the maximum entry speed, no need to recheck. Block is cruising.
		// If not, block in state of accelSpeed or deceleration. Reset entry speed to maximum and
		// check for maximum allowable speed reductions to ensure maximum possible planned speed.
		//entrySpeed<jerkEntrySpeed的情况,是由于entrySpeed>speedReadyForStop@<libStepPlanner.c line243>
		if (current->entrySpeed != current->jerkEntrySpeed) {
			// If nominal length true, max junction speed is guaranteed to be reached. Only compute
			// for max allowable speed if block is decelerating and nominal length is false.
			if ((!current->nominal_length_flag) && (current->jerkEntrySpeed > next->entrySpeed)) {
				current->entrySpeed = MIN( current->jerkEntrySpeed,
				                            speedAfterAccel(next->entrySpeed,current->accelSpeed,current->distance)	);
				if(current->entrySpeed==current->jerkEntrySpeed){
					current->entrySetBy+=100000;
				}else{
					current->entrySetBy+=1000000;
				}
			} else {
				current->entrySpeed = current->jerkEntrySpeed;
				current->entrySetBy=10000000;
			}
			current->recalculate_flag = true;
		}else{
			current->nominal_length_flag=true;
		}
	} // Skip last block. Already initialized and set for recalculation.
}

// planner_recalculate() needs to go over the current plan twice. Once in reverse and once forward.
// This implements the reverse pass.
void planner_reverse_pass(stepBlock_t *sb) {
	int8_t head=sb->head;
	//Make a local copy of block_buffer_tail, because the interrupt can alter it
	CRITICAL_SECTION_START;
	int8_t tail = sb->tail;
	CRITICAL_SECTION_END

	if( block_size(head,tail) > 2 ){
		int8_t block_index=head;
		motionLine_t *block[3] = {0,0,0};
		while(block_index != tail) {
			block_index = prev_block_index(block_index);
			block[2]= block[1];
			block[1]= block[0];
			block[0] = &sb->buffer[block_index];
			planner_reverse_pass_kernel(block[0], block[1], block[2]);
		}
	}
}

// The kernel called by planner_recalculate() when scanning the plan from first to last entry.
void planner_forward_pass_kernel(motionLine_t *previous, motionLine_t *current, motionLine_t *next) {
	if(!previous || !current) {
		return;
	}

	// If the previous block is an accelSpeed block, but it is not long enough to complete the
	// full speed change within the block, we need to adjust the entry speed accordingly.
	// Entry speeds have already been reset, maximized, and reverse planned by reverse planner.
	// If nominal length is true, max junction speed is guaranteed to be reached. No need to recheck.
	if (!previous->nominal_length_flag) {
		if (previous->entrySpeed < current->entrySpeed){
			float entrySpeed = MIN( current->entrySpeed,
			                          //max_allowable_speed(-previous->accelSpeed,previous->entrySpeed,previous->distance) 
										speedAfterAccel(previous->entrySpeed,previous->accelSpeed,previous->distance) );

			// Check for junction speed change
			if (current->entrySpeed != entrySpeed){
				current->entrySpeed = entrySpeed;
				current->entrySetBy+=100000000;
				current->recalculate_flag = true;
			}
		}
	}
}

// planner_recalculate() needs to go over the current plan twice. Once in reverse and once forward. This
// implements the forward pass.
void planner_forward_pass(stepBlock_t *sb) {
	int8_t head=sb->head;
	//Make a local copy of block_buffer_tail, because the interrupt can alter it
	CRITICAL_SECTION_START;
	int8_t tail = sb->tail;
	CRITICAL_SECTION_END
	int8_t block_index = tail;
	motionLine_t *block[3] = {
		0,0,0
	};

	while(block_index != head) {
		block[0] = block[1];
		block[1] = block[2];
		block[2] = &sb->buffer[block_index];
		planner_forward_pass_kernel(block[0],block[1],block[2]);
		block_index = next_block_index(block_index);
	}
	planner_forward_pass_kernel(block[1], block[2], 0);
}

// Recalculates the trapezoid speed profiles for all blocks in the plan according to the
// entryFactor for each junction. Must be called by planner_recalculate() after
// updating the blocks.
void planner_recalculate_trapezoids(stepBlock_t *sb) {
	int8_t block_index = sb->tail;
	motionLine_t *current;
	motionLine_t *next = 0;
	
	while(block_index != sb->head) {
		current = next;
		next = &sb->buffer[block_index];
		if (current) {
			// Recalculate if current block entry or exit junction speed has changed.
			if (current->recalculate_flag || next->recalculate_flag) {
				// NOTE: Entry and exit factors always > 0 by all previous logic operations.
				calculate_trapezoid_for_block(current, current->entrySpeed/current->nominalSpeed,
				                              next->entrySpeed/current->nominalSpeed);
				current->recalculate_flag = false; // Reset current only to ensure next trapezoid is computed
			}
		}
		block_index = next_block_index( block_index );
	}
	// Last/newest block in buffer. Exit speed is set with MINIMUM_PLANNER_SPEED. Always recalculated.
	if(next != 0) {
		calculate_trapezoid_for_block(next, next->entrySpeed/next->nominalSpeed,
		                              MINIMUM_PLANNER_SPEED/next->nominalSpeed);	//bug:MINIMUM_PLANNER_SPEED need replace by param
		next->recalculate_flag = false;
	}
}

// Recalculates the motion plan according to the following algorithm:
//
//   1. Go over every block in reverse order and calculate a junction speed reduction (i.e. motionLine_t.entryFactor)
//      so that:
//     a. The junction jerk is within the set limit
//     b. No speed reduction within one block requires faster deceleration than the one, true constant
//        accelSpeed.
//   2. Go over every block in chronological order and dial down junction speed reduction values if
//     a. The speed increase within one block would require faster accelleration than the one, true
//        constant accelSpeed.
//
// When these stages are complete all blocks have an entryFactor that will allow all speed changes to
// be performed using only the one, true constant accelSpeed, and where no junction jerk is jerkier than
// the set limit. Finally it will:
//
//   3. Recalculate trapezoids for all blocks.

void planner_recalculate(stepBlock_t *sb) {
	planner_reverse_pass(sb);
	planner_forward_pass(sb);
	planner_recalculate_trapezoids(sb);
}
