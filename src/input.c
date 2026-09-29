/*
 * Saturn project input utilities
 * 2026 (c) Scott Harner
 */

#include "input.h"
#include <jo/jo.h>

// track button changes for better title menu input handling
static bool current_pad1_input_states[INPUT_TYPE_COUNT];
static bool previous_pad1_input_states[INPUT_TYPE_COUNT];
static bool current_pad2_input_states[INPUT_TYPE_COUNT];
static bool previous_pad2_input_states[INPUT_TYPE_COUNT];

// check if input was newly pressed
bool pad_input_pressed(int pad, input_type candidate_input)
{
    switch (pad)
	{
		case 2:
			return current_pad2_input_states[candidate_input] && !previous_pad2_input_states[candidate_input];

			break;

		default:
			return current_pad1_input_states[candidate_input] && !previous_pad1_input_states[candidate_input];
		
			break;
	}
}

void reset_pad_input_states(int pad)
{
    switch (pad)
	{
		case 2:
			for (int i = 0; i < INPUT_TYPE_COUNT; i++)
			{
				previous_pad2_input_states[i] = false;
				current_pad2_input_states[i] = false;
			}

			break;

		default:
			for (int i = 0; i < INPUT_TYPE_COUNT; i++)
			{
				previous_pad1_input_states[i] = false;
				current_pad1_input_states[i] = false;
			}

			break;
	}
}

void save_previous_pad_inputstates(int pad)
{
    // save previous state
	switch (pad)
	{
		case 2: 
			for (int i = 0; i < INPUT_TYPE_COUNT; i++)
				previous_pad2_input_states[i] = current_pad2_input_states[i];

			break;

		default:
			for (int i = 0; i < INPUT_TYPE_COUNT; i++)
				previous_pad1_input_states[i] = current_pad1_input_states[i];

			break;
	}
}

// track all current and previous input states so we can check on input presses
void update_pad_input_states(int pad)
{
    save_previous_pad_inputstates(pad);

    // read current state
    switch (pad)
	{
		case 2:
			current_pad2_input_states[INPUT_TYPE_UP] = jo_is_input_key_pressed(6, JO_KEY_UP);
			current_pad2_input_states[INPUT_TYPE_DOWN] = jo_is_input_key_pressed(6,JO_KEY_DOWN);
			current_pad2_input_states[INPUT_TYPE_LEFT] = jo_is_input_key_pressed(6,JO_KEY_LEFT);
			current_pad2_input_states[INPUT_TYPE_RIGHT] = jo_is_input_key_pressed(6,JO_KEY_RIGHT);
			current_pad2_input_states[INPUT_TYPE_START] = jo_is_input_key_pressed(6,JO_KEY_START);    
			current_pad2_input_states[INPUT_TYPE_A] = jo_is_input_key_pressed(6,JO_KEY_A);
			current_pad2_input_states[INPUT_TYPE_C] = jo_is_input_key_pressed(6,JO_KEY_C);
			current_pad2_input_states[INPUT_TYPE_Z] = jo_is_input_key_pressed(6,JO_KEY_Z);

			break;

		default:
			current_pad1_input_states[INPUT_TYPE_UP] = jo_is_pad1_key_pressed(JO_KEY_UP);
			current_pad1_input_states[INPUT_TYPE_DOWN] = jo_is_pad1_key_pressed(JO_KEY_DOWN);
			current_pad1_input_states[INPUT_TYPE_LEFT] = jo_is_pad1_key_pressed(JO_KEY_LEFT);
			current_pad1_input_states[INPUT_TYPE_RIGHT] = jo_is_pad1_key_pressed(JO_KEY_RIGHT);
			current_pad1_input_states[INPUT_TYPE_START] = jo_is_pad1_key_pressed(JO_KEY_START);    
			current_pad1_input_states[INPUT_TYPE_A] = jo_is_pad1_key_pressed(JO_KEY_A);
			current_pad1_input_states[INPUT_TYPE_C] = jo_is_pad1_key_pressed(JO_KEY_C);
			current_pad1_input_states[INPUT_TYPE_Z] = jo_is_pad1_key_pressed(JO_KEY_Z);

			break;
	}
}

bool is_pad_key_pressed(int pad, int key)
{
	switch(pad)
	{
		case 2:
			return jo_is_input_key_pressed(6,key);
			break;

		default:
			return jo_is_pad1_key_pressed(key);
			break;
	}
}

bool is_pad_available(int pad)
{
	switch (pad)
	{
		case 2:
			return jo_is_input_available(6); // seems that port 1 is 0-5 and port 2 is probably 6-11
			break;

		default:
			return jo_is_pad1_available();
			break;
	}
}