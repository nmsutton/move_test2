/*
	Place cell functions

	These methods are modified versions of ones used in (solanka et al., 2015)
	One modification is that boundary cell input to place cells is limited to
	a local region around the active place cell. This prevents boundary cell
	signal from simply making an increase in signal of all place cells 
	indiscriminately.
*/

double pc_rate(int p_x, int p_y, int b_x, int b_y, G *g) {
	double d = get_distance(p_x, p_y, b_x, b_y, 'n', g);

	double rate = g->pc_level * exp(-((pow(d,2))/(2*pow(g->pc_sig,2))));

	//if(p_y==11&&p_x==1) {printf("%d %d %d %d %f %f\n",p_x,p_y,b_x,b_y,d,rate);}	

	return rate;
}

void place_cell_firing(double *gc_firing, G *g) {
	/*
		generate place cell firing
	*/

	int bump_x, bump_y, cb_x, cb_y, gc_i; 
	int closest_bump = -1; 
	double cb_dist = -1;
	double cb_dist_new = -1;
	double pc_firing, bc_firing;

	for (int p_y = 0; p_y < g->layer_y; p_y++) {
		for (int p_x = 0; p_x < g->layer_x; p_x++) {

			// find closest bump
			closest_bump = -1;
			for (int b_i = 0; b_i < g->num_bumps; b_i++) {				
				bump_x = g->pos[0] + ((b_i % g->bumps_x) * g->bump_dist); // closest bump x
				bump_y = g->pos[1] + ((b_i / g->bumps_y) * g->bump_dist);
				// adjust for staggering bump pattern
				if (((b_i / g->bumps_y)+1) % 2 == 0) {
					bump_x = bump_x + (g->bump_dist/2);
				}
				cb_dist_new = get_distance(p_x, p_y, bump_x, bump_y, 'n', g);

				if (closest_bump == -1) {
					closest_bump = b_i;
					cb_dist = cb_dist_new;
					cb_x = bump_x;
					cb_y = bump_y;
				}
				else if (cb_dist_new < cb_dist) {
					closest_bump = b_i;
					cb_dist = cb_dist_new;	
					cb_x = bump_x;
					cb_y = bump_y;			
				}
			}		

			// find firing
			pc_firing = 0.0;
			if (cb_dist < g->dist_thresh) {
				if (g->pc_active) {
					pc_firing = pc_rate(p_x, p_y, cb_x, cb_y, g);
				}

				// add boundary cell input
				if (g->bc_to_pc) {
					bc_firing = bc_for_pc(p_x, p_y, cb_x, cb_y, g);
					pc_firing = pc_firing + bc_firing;
				}
			}

			gc_i = (p_y * g->layer_x) + p_x;
			gc_firing[gc_i] = gc_firing[gc_i] + pc_firing;
		}
	}
}