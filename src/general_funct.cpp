/*
	General functions and parameters
*/

#define PI 3.14159265

struct G {
	// general parameters
	int t; // time
	static const int bump_init_x = 1.0; // initial bump x
	static const int bump_init_y = 1.0; // initial bump y
	static const int bump_dist = 7.0; // inter-bump distance
	int bumps_x = 2; // number of bumps on x axis
	int bumps_y = 2; // number of bumps on y axis
	int num_bumps = bumps_x * bumps_y; // number of initial bumps
	double pos[2] = {1,1}; // starting position
	char last_dir; // last direction command
	static const int layer_x = 20;//26;
	static const int layer_y = 20;//26;
	static const int layer_size = layer_x * layer_y;
	int start_t = -1; // beginning time of move command
	int mi = 0; // move list index
	double run_time = 500;//50; // sim run time
	bool print_move = 0; // print each move's direction
	bool print_time = 1; // print time after processing
	bool init_bumps = 1; // inital bumps present
	bool base_input = 1; // baseline external signal input
	bool gc_to_gc = 1; // grid cell to grid cell signaling
	bool bc_to_gc = 0; // boundary cells to grid cells signaling
	bool pc_to_gc = 0; // place cells to grid cells signaling
	bool bc_to_pc = 0; // boundary cells to place cells signaling
	bool pc_active = 0; // pc signaling active. bc->pc->gc can still work even if this is disabled.

	// noise parameters
	bool noise_active = 0; // activate noise
	double noise_rand_max = 100; // 0 - rand_max is range of random number gen
	double noise_scale = 0.01; // scale to desired size for firing

	// values for synapse activites
	bool speed_adjustable = 0;
	double speed = 1.0; // starting grid cell input speed level
	double base_ext = 1.0;//2.5; // baseline ext input speed level
	double min_speed = 0.25; // minimum speed for random speed generator. note: signal applied even when stopped.
	double max_speed = 1.0; // maximum speed for random speed generator
	double tau_syn = .6;
	double y_inter_syn = 0.2;//-1;//-0.03;//-0.05;//0.15;//-.5;//1.044;//1.055; // y intercept
	double scale_syn = 2.5;//0.25;//1.0; // multiple synaptic connections scaling factor
	double m_syn = 0.3; // magnitude variable for mex hat f1
	double m_syn2 = 2.5; // f2 f3
	double m_syn3 = 0.3; // f4
	double m_syn4 = 1.1; // f2 f3
	double s_1_syn = 0.6; // f1
	double s_2_syn = 0.35; // f2 f3
	double s_3_syn = .4; // f4
	double s_4_syn = 1.5; 
	double s_5_syn = 1.0;
	double a_syn = 4.4; // add f2 f3
	double dist_thresh = 5; // distance threshold for only local connections

	// initial values
	double y_inter_init = 0.4;//y_inter_syn; // y intercept
	double scale_init=3.0;//scale_syn;	
	double s_1_init = s_1_syn; // sigma_1. Note: specific value used for equalibrium of weights over time.
	double s_2_init = s_2_syn;
	double s_3_init = s_3_syn;
	double s_4_init = s_4_syn;
	double s_5_init = s_5_syn;
	double m_init=m_syn;
	double m_init2=m_syn2;
	double m_init3=m_syn3;
	double m_init4=m_syn4;
	double a_init=a_syn;
	double tau, y_inter, scale, s_1, s_2, s_3, s_4, s_5, m, m2, m3, m4, a;

	// tau time constant and asymmetric sigmoid parameters. https://en.wikipedia.org/wiki/Gompertz_function
	double asig_a = -5;//-10;//-8.0;//0.6;//0.45;//2.0;//0.45;
	double asig_b = 3.5;//9.5;//2.2;//9.89493996719;//0.6;//2.15;//0.6;
	double asig_c = 5;//10.0;//4.9898;//3.0;//0.457921;//1.0;//0.5;
	double asig_yi = 0.0;//-0.9;//0.0;//-0.9;
	double asig_scale = 1.0;//2.0;//-0.9;

	// place cell parameters
	double pc_sig = 0.25; // sigma symbol; width of the place feild
	double pc_level = 14.0; // place cell firing level

	// boundary cell parameters
	double r_d = 1.0; // boundary cell active region width
	double bc_firing_scale = 0.1; // amount of boundary cell firing when activated
	double bc_pd = 5.0; // boundary cell prefered distance
	static const int b_num = 4.0; // number of borders
	double bc_distances[b_num];
	// response curve factors
	//double bc_level = 14.0 * 0.25; // level of bc firing
	double bc_a0 = 2.0; // boundary cell A_0 factor for response curve
	double bc_b = 0.25;
	double bc_sig0 = 0.0;
	double bc_y = 0.05;//0.25;
	double bc_a = 0.25;
	double bc_sig = 1.0;
};

double get_mex_hat(double d, G *g) {
	double y_inter = g->y_inter;
	double s1 = g->s_1;
	double s2 = g->s_2;
	double s3 = g->s_3;
	double s4 = g->s_4;
	double s5 = g->s_5;
	double m1 = g->m;
	double m2 = g->m2;
	double m3 = g->m3;
	double m4 = g->m4;
	double a = g->a;
	double scale = g->scale;
	double mex_hat;

	mex_hat = y_inter + scale * 
	((1-(pow((m1*d)/s1,2))) *
	(exp(-1*(m3*pow(d,2))/(12*pow(s3,2)))));

	return mex_hat;
}

double get_distance(int x1, int y1, int x2, int y2, char pd, G *g) {
	double x2_x1 = (x2 - x1);
	double y2_y1 = (y2 - y1);
	double half_point = g->layer_x / 2; // layer length divided by 2

	// preferred direction bias
	if (pd == 'u') {
		y2_y1 = y2_y1 - 1;
	}
	if (pd == 'd') {
		y2_y1 = y2_y1 + 1;
	}
	if (pd == 'r') {
		x2_x1 = x2_x1 - 1;
	}
	if (pd == 'l') {
		x2_x1 = x2_x1 + 1;
	}	

	// torus wrap around
	if (abs(x2_x1) >= half_point) {
		// distance wraps half way around
		x2_x1 = (g->layer_x - abs(x2_x1));
	}
	if (abs(y2_y1) >= half_point) {
		y2_y1 = (g->layer_y - abs(y2_y1));
	}

	double d = sqrt(pow(x2_x1,2)+pow(y2_y1,2));

	return d;
}