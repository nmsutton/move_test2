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
#include "saved_weights.cpp"
#include "saved_dir_input.cpp"
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
	double in_firing, pd_fac, mex_hat;
	double pd_facs[g->layer_size]; double new_firing_group[g->layer_size]; 
	for (int i = 0; i < g->layer_size; i++) {pd_facs[i] = 0.00001; new_firing_group[i] = 0.00001;}

	set_pos(g, direction); if (g->print_move) {cout << "\n";}

	/* apply ext input first */
	/*for (int i_gc = 0; i_gc < g->layer_size; i_gc++) {
		if (get_pd(i_gc, g) == direction) {pd_fac = 1.0;}
		else {pd_fac = 0.0;}
		if (g->base_input) {gc_firing[i_gc] = gc_firing[i_gc] + pd_fac;}
	}*/

	//if (g->pc_to_gc) {place_cell_firing(gc_firing, g);}
	//if (g->bc_to_gc) {boundary_cell_firing(gc_firing, g);}

	for (int i = 0; i < g->layer_size; i++) {
		for (int j = 0; j < g->layer_size; j++) {
			mex_hat = saved_weights[i][j];
			in_firing = mex_hat * (floor(gc_firing[j])*g->in_scale); // carlsim model only reports whole number gc firing
			new_firing_group[i] = new_firing_group[i] + in_firing;
		}
		new_firing_group[i] = new_firing_group[i] + ext_dir_firing[i]*g->ext_scale; // dir_ext input
		if (new_firing_group[i] < 0) {new_firing_group[i] = 0;} // non zero firing rectifier
		gc_firing[i] = gc_firing[i] + (new_firing_group[i] - gc_firing[i])/g->tau_syn; // tau derivative
	}
}

int main() {
	struct G g;	double gc_firing[g.layer_size];
	
	init_firing2(gc_firing, &g);
	//print_firing(gc_firing, 0, &g);
	if (g.print_time == true) {printf("time processed:\n");}

	for (int t = 1; t <= g.run_time; t++) {
		g.t = t;
		move_path_bound_test(gc_firing, t, &g);
		//print_firing(gc_firing, t, &g); //cout << g.pos[0] << " " << g.pos[1] << "\n";
		if (g.print_time == true && t % 200 == 0) {printf("t: %d\n",t);}
		write_firing(gc_firing, t, &g);		
	}

	return 0;
}