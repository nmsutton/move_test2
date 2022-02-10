/*
	Author: Nate Sutton 2021

	References:
	https://en.wikipedia.org/wiki/Gompertz_function
*/
#include <vector>
#include <math.h> // for sqrt() and other functions
// for to_string()
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include "general_funct.cpp"
#include "addit_funct.cpp"
#include "move_path.cpp"
//#include "boundary_cells.cpp"
//#include "place_cells.cpp"
// for file out
#include <iostream>
#include <fstream>
#include <string> // for filename
using namespace std;

void ext_input(char direction, double *gc_firing, G* g) {
	/*
		Apply external input
	*/	
	double new_firing, new_weight, weight_sum, pd_fac, mex_hat, new_sig, pdx, pdy, gcx, gcy, d;
	int pd_i, gc_i; double pd_facs[g->layer_size]; double new_firing_group[g->layer_size];
	double firing_inhib[g->layer_size];
	for (int i = 0; i < g->layer_size; i++) {pd_facs[i] = 0.00001; new_firing_group[i] = 0.00001;
	firing_inhib[i] = 0.00001;}	

	set_pos(g, direction); if (g->print_move) {cout << "\n";}

	/* apply ext input first */
	for (int gc_i = 0; gc_i < g->layer_size; gc_i++) {
		if (get_pd(gc_i, g) == direction) {pd_fac = 1.0;}
		else {pd_fac = 0.0;}
		if (g->base_input) {gc_firing[gc_i] = gc_firing[gc_i] + pd_fac;}
	}

	//if (g->pc_to_gc) {place_cell_firing(gc_firing, g);}
	//if (g->bc_to_gc) {boundary_cell_firing(gc_firing, g);}
	/* grid cell and interneuron synapse connections */
	for (int pdy = 0; pdy < g->layer_y; pdy++) {
		for (int pdx = 0; pdx < g->layer_x; pdx++) {
			if (direction == get_pd(pdx, pdy) || direction == 'n') {
				for (int gcy = 0; gcy < g->layer_y; gcy++) {
					for (int gcx = 0; gcx < g->layer_x; gcx++) {			
						pd_i = (pdy * g->layer_x) + pdx;						
						gc_i = (gcy * g->layer_x) + gcx;
						d = get_distance(pdx, pdy, gcx, gcy, direction, g);
						if (d < g->dist_thresh) { 
							mex_hat = get_mex_hat(d, g);
							gc_firing[pd_i] = floor(gc_firing[pd_i]); // carlsim model only reports whole number gc firing
							//new_firing = (gc_firing[pd_i] * mex_hat) - ((1/pow(g->dist_thresh,1.75))*8);
							//new_firing = gc_firing[pd_i] * (mex_hat - 0.3889);
							//new_firing = gc_firing[pd_i] * mex_hat;
							//new_firing = (gc_firing[pd_i] * mex_hat) - (0.47851);
							//new_firing = (gc_firing[pd_i] * mex_hat) - (0.4);
							new_firing = gc_firing[pd_i] * mex_hat;
							new_firing_group[gc_i] = new_firing_group[gc_i] + new_firing;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < g->layer_size; i++) {
		double highest_firing = 5.0;
		if (new_firing_group[i] > 0 && new_firing_group[i] <= highest_firing) {
			firing_inhib[i] = -new_firing_group[i];
		}
		else if (new_firing_group[i] <= 0) {
			firing_inhib[i] = -highest_firing + new_firing_group[i];
		}
		if (firing_inhib[i] > 0) {
			firing_inhib[i] = 0;
		}
	}
	//print_firing(firing_inhib, g->t, g);
	//print_firing(new_firing_group, g->t, g);

	for (int i = 0; i < g->layer_size; i++) {
		if (g->gc_to_gc) {
			/*if (new_firing_group[i] > 0) {
				new_firing_group[i] = 0; // only negative values for IN weights
			}
			gc_firing[i] = new_firing_group[i] + (g->dist_thresh*1.7);*/
			//gc_firing[i] = (gc_firing[i] + firing_inhib[i]) + (g->dist_thresh*1.7);
			gc_firing[i] = firing_inhib[i] + (g->dist_thresh*1.3);
			//gc_firing[i] = new_firing_group[i];
		}

		// original tau derivative
		gc_firing[i] = g->asig_a * exp(-1*(gc_firing[i]/g->asig_b))+g->asig_c;
		// non zero firing rectifier
		if (gc_firing[i] < 0) {gc_firing[i] = 0;}
		gc_firing[i] = floor(gc_firing[i]);
	}
}

int main() {
	struct G g;	double gc_firing[g.layer_size];
	
	init_firing(gc_firing, &g);
	//print_firing(gc_firing, 0, &g);
	if (g.print_time == true) {printf("time processed:\n");}

	for (int t = 1; t <= g.run_time; t++) {
		g.t = t;
		move_path_bound_test(gc_firing, t, &g);
		print_firing(gc_firing, t, &g); //cout << g.pos[0] << " " << g.pos[1] << "\n";
		if (g.print_time == true && t % 200 == 0) {printf("t: %d\n",t);}
		write_firing(gc_firing, t, &g);		
	}

	return 0;
}