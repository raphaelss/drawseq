#ifndef OPERATIONS_H_INCLUDED
#define OPERATIONS_H_INCLUDED

//Operations
typedef int (*operation)(struct position *p, struct position_stack *st,
                         struct draw_dev *dr, unsigned d_count,
                         unsigned repeat_count);

void update_position_draw(struct position_stack *st, struct draw_dev *dr,
                          unsigned d_count);
int do_char(int ch, struct position_stack *st, struct draw_dev *dr,
            unsigned *d_count, unsigned *repeat_count);

operation op_line;
operation op_move;
operation op_reset;
operation op_move_to_origin;
operation op_push_stack;
operation op_pop_stack;
operation op_15deg_counterclockwise;
operation op_15deg_clockwise;

#endif
