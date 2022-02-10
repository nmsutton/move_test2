/*
	Additional general use functions
*/
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iostream>
#include <fstream>
#include <string> // for filename
using namespace std;

string to_string(double x);

string to_string(double x)
{
  ostringstream ss;
  ss << x;
  return ss.str();
}

string int_to_string(int x)
{
  ostringstream ss;
  ss << x;
  return ss.str();
}

char get_pd(int x, int y) {
	char pd;

	if (y % 2 == 0) {
		if (x % 2 == 0) {
			pd = 'd';
		}
		else {
			pd = 'r';
		}
	}
	else {
		if (x % 2 == 0) {
			pd = 'l';
		}
		else {
			pd = 'u';
		}		
	}

	return pd;
}

char get_pd(int i, G *g) {	
	char pd;
	int x = i % g->layer_x;
	int y = i / g->layer_x;

	if (y % 2 == 0) {
		if (x % 2 == 0) {
			pd = 'd';
		}
		else {
			pd = 'r';
		}
	}
	else {
		if (x % 2 == 0) {
			pd = 'l';
		}
		else {
			pd = 'u';
		}		
	}

	return pd;
}

char get_pd_opp(int i, G *g) {	
	char pd;
	int x = i % g->layer_x;
	int y = i / g->layer_x;

	if (y % 2 == 0) {
		if (x % 2 == 0) {
			pd = 'u';
		}
		else {
			pd = 'l';
		}
	}
	else {
		if (x % 2 == 0) {
			pd = 'r';
		}
		else {
			pd = 'd';
		}		
	}

	return pd;
}

void write_firing(double *gc_firing, int t, G* g) {
	ofstream output_file;
	string filename = "output/firing_t" + int_to_string(t) + ".csv";
	output_file.open(filename);

	int layer_x = g->layer_x;
	int layer_y = g->layer_y;
	int layer_size = g->layer_size;
	int gc_ind = 0; // grid cell index
	double temp;

	if (t != 0) {
		for (int i = (layer_x - 1); i >= 0; i--) {
			for (int j = 0; j < layer_y; j++) {
				gc_ind = (i * layer_x) + j;

				output_file << gc_firing[gc_ind];

				if (j != (layer_y -1)) {
					output_file << ",";
				}
			}
			if (i != 0) {
				output_file << "\n";
			}
		}
	}

  output_file.close();
}

/*void print_weights(int t, G* g) {
	cout << "\ntime: " << t << " sec\n";

	for (int i = (layer_y - 1); i >= 0; i--) {
		for (int j = 0; j < layer_x; j++) {
			gc_ind = (i * layer_x) + j;
			printf("|");
			if (gc_firing[gc_ind] >= 0) {
				printf(" ");
			}
			printf("%.2f",gc_firing[gc_ind]);
			if (g->pos[0] == j && g->pos[1] == i && g->print_move) {
				printf("(%c)",g->last_dir);
			}
		}
		cout << "\n";
	}
}*/

void print_firing(double *gc_firing, int t, G* g) {
	int layer_x = g->layer_x;
	int layer_y = g->layer_y;
	int layer_size = g->layer_size;
	int gc_ind = 0; // grid cell index
	double temp;

	//if (t != 0) {
	if (true) {
		cout << "\ntime: " << t << " sec\n";
		for (int i = (layer_y - 1); i >= 0; i--) {
			for (int j = 0; j < layer_x; j++) {
				gc_ind = (i * layer_x) + j;
				printf("|");
				if (gc_firing[gc_ind] >= 0) {
					//printf("+");
					printf(" ");
				}
				//printf("%.2f %c",abs(gc_firing[gc_ind]),get_pd(i,j));
				printf("%.2f",gc_firing[gc_ind]);
				//printf("%.2f",abs(gc_firing[gc_ind]));
				if (g->pos[0] == j && g->pos[1] == i && g->print_move) {
					printf("(%c)",g->last_dir);
				}
			}
			cout << "\n";
		}
	}
}


void set_pos(G* g, char direction) {
	if (direction == 'u') {
		g->pos[1]++; 
	}
	else if (direction == 'd') {
		g->pos[1]--; 
	}
	else if (direction == 'r') {
		g->pos[0]++; 
	}
	else if (direction == 'l') {
		g->pos[0]--; 
	}

	if (g->pos[0] >= g->layer_x) {
		g->pos[0] = 0;
	}
	else if (g->pos[0] < 0) {
		g->pos[0] = (g->layer_x - 1);
	}
	if (g->pos[1] >= g->layer_y) {
		g->pos[1] = 0;
	}
	else if (g->pos[1] < 0) {
		g->pos[1] = (g->layer_y - 1);
	}

	if (g->print_move == true && (direction == 'u' || direction == 'd' || direction == 'l' || direction == 'r')) {
		cout << " move: " << direction;
	}

	g->last_dir=direction;
}

double get_noise(G *g) {
	int rand_max = g->noise_rand_max;
	double scale = g->noise_scale;

	double rand_val = rand() % rand_max; // rand number up to rand_max
	rand_val = rand_val * scale; // scale to desired size
	//rand_val = rand_val - (scale/2); // include negative values, comment out to only have positive values

	return rand_val;
}


void init_firing(double *gc_firing, G *g) {
	// initialize firing

	int i;
	int init_x = g->bump_init_x;
	int init_y = g->bump_init_y;
	int bump_d = g->bump_dist;
	double mex_hat, d;
	double firing_bumps[g->layer_size];
	for (int i = 0; i < g->layer_size; i++) {
		firing_bumps[g->layer_size] = 0.0;
	}
	int bump_pos[g->num_bumps][2] = {{init_x,init_y},{(init_x+(bump_d/2)),(init_y+bump_d)},{(init_x+bump_d),init_y},{(init_x+(bump_d+(bump_d/2))),(init_y+bump_d)}};
	//double bump_pos[num_bumps][2] = {{1,1}};
	//double bump_pos[num_bumps][2] = {{4,4},{4,14},{14,4},{14,14}};
	g->y_inter = g->y_inter_init; // y intercept
	g->s_1 = g->s_1_init; // sigma_1. Note: specific value used for equalibrium of weights over time.
	g->s_2 = g->s_2_init;
	g->s_3 = g->s_3_init;
	g->s_4 = g->s_4_init;
	g->s_5 = g->s_5_init;
	g->m = g->m_init;
	g->m2 = g->m_init2;
	g->m3 = g->m_init3;
	g->m4 = g->m_init4;
	g->a = g->a_init;
	g->scale = g->scale_init;

	// find weights for the starting bumps
	for (int y = 0; y < g->layer_y; y++) {
		for (int x = 0; x < g->layer_x; x++) {
			for (int b = 0; b < g->num_bumps; b++) {
				i = (y * g->layer_x) + x;

				d = get_distance(x, y, bump_pos[b][0], bump_pos[b][1], 'n', g);

				if (d < g->dist_thresh) {
				//if (true) {
					mex_hat = get_mex_hat(d, g);

					firing_bumps[i] = firing_bumps[i] + mex_hat;
				}
			}
		}
	}

	for (int i = 0; i < g->layer_size; i++) {
		if (firing_bumps[i] < 0) {
			firing_bumps[i] = 0; // no neg values rectifier
		}

		if (g->init_bumps) {
			gc_firing[i] = firing_bumps[i];
		}
	}

	// set parameters to non-initial values
	g->y_inter = g->y_inter_syn;
	g->s_1 = g->s_1_syn;
	g->s_2 = g->s_2_syn;
	g->s_3 = g->s_3_syn;
	g->s_4 = g->s_4_syn;
	g->s_5 = g->s_5_syn;
	g->m = g->m_syn;
	g->m2 = g->m_syn2;
	g->m3 = g->m_syn3;
	g->m4 = g->m_syn4;
	g->a = g->a_syn;
	g->scale = g->scale_syn;
}