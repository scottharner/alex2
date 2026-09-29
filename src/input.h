/*
 * Saturn project utilities
 * 2026 (c) Scott Harner
 */

 #ifndef INPUT_H
 #define INPUT_H

 #include <jo/jo.h>
 
 typedef enum
{
	INPUT_TYPE_NOTHING = 0, 
	INPUT_TYPE_LEFT, 
	INPUT_TYPE_RIGHT, 
	INPUT_TYPE_UP, 
	INPUT_TYPE_DOWN, 
	INPUT_TYPE_START, 
	INPUT_TYPE_UP_LEFT, 
	INPUT_TYPE_UP_RIGHT, 
	INPUT_TYPE_DOWN_LEFT, 
	INPUT_TYPE_DOWN_RIGHT, 
	INPUT_TYPE_A,
	INPUT_TYPE_C,
	INPUT_TYPE_Z,
	INPUT_TYPE_COUNT
}input_type;

bool pad_input_pressed(int pad, input_type candidate_input);
void reset_pad_input_states(int pad);
void save_previous_pad_inputstates(int pad);
void update_pad_input_states(int pad);
bool is_pad_key_pressed(int pad, int key);
bool is_pad_available(int pad);

#endif