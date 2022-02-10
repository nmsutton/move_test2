/*
	movement sequences
*/

char rand_move() {
	int num_directions = 5;
	int rand_val = rand() % num_directions + 1;
	char direction;

	if (rand_val == 1) {
		direction = 'u';
	}
	else if (rand_val == 2) {
		direction = 'r';
	}
	else if (rand_val == 3) {
		direction = 'd';
	}
	else if (rand_val == 4) {
		direction = 'l';
	}
	else if (rand_val == 5) {
		direction = 'n'; // stopped
	}

	return direction;
}

double rand_speed(G *g) {
	double scale = 0.01;
	double max = g->max_speed;
	double min = g->min_speed;
	int rand_val = (min*(1/scale));
	int addit_sig = (max-min)*(1/scale); // additional speed signal
	if (addit_sig > 0) {
		rand_val = rand_val + (rand() % addit_sig);
	}

	return (double) rand_val * scale;
}

void ext_input(char direction, double *gc_firing, G* g);

void create_move(char dir, double *gc_firing, int t, G* g) {
	/*
		Create movement after delay controlled by a speed variable.
	*/
	if (g->start_t == -1) {
		g->start_t = t;
	}
	if (t < (g->start_t + round(1/g->speed)) && g->speed_adjustable == true) {
		ext_input('n', gc_firing, g);
	}
	else {
		ext_input(dir, gc_firing, g);
		g->start_t = -1;
		g->mi = g->mi + 1;
		//printf("%d\n",g->mi);
	}
}

void run_path(char *moves, double *speeds, int *speed_times, int num_moves, int num_speeds, double *gc_firing, int t, G *g) {
	/*
		Run movements through a path.
	*/

	if (g->mi < num_moves) {
		for (int i = 0; i < num_speeds; i++) {
			if (t == speed_times[i]) {
				g->speed = speeds[i];
			}
		}

		create_move(moves[g->mi], gc_firing, t, g);
	}
	else {
		if (t % 50 == 0) {
			g->speed = rand_speed(g);
			//printf("speed: %f nm: %d\n",g->speed,num_moves);
		}
		create_move(rand_move(), gc_firing, t, g);
	}
}

void straight_path(double *gc_firing, int t, G* g) {
	// stright line path
	create_move('u', gc_firing, t, g);
}

void rand_path(double *gc_firing, int t, G* g) {
	// random move

	if (t % 50 == 0) {
		g->speed = rand_speed(g);
		//printf("speed: %f\n",g->speed);
	}
	create_move(rand_move(), gc_firing, t, g);
}

void move_path_bound_test(double *gc_firing, int t, G* g) {
	// movement path

	char moves[] {'u','l','l','u','r','r','l','r','r','r','r','u','r','l','l','l','u','r','r','r','u','l',
	'd','r','d','r','l','l','u','r','r','r','l','l','l','l','u','r','l','d','r','d','d','u','d','u','d','u',
	'u','r','d','l','l','l','r','u','u','u','r','r','u','r','r','r','r','r','u','r','r','r','r','d','u','r',
	'l','u','r','u','d','r','l','l','r','u','r','r','r','l','d','r','d','r','l','d','r','d','d','u','r','d',
	'r','l','l','u','r','r','l','l','u','l','u','u','r','r','u','r','r','r','r','r','r','r','r','r','r','d',
	'u','r','l','u','r','u','d','r','l','l','u','r','r','r','l','d','r','d','r','l','d','r','d','d','u','d',
	'r','l','l','u','r','r','l','l','u','l','l','u','r','r','u','n','n','n','n','n','n','r','r','r','d','u',
	'r','l','u','r','u','d','r','l','l','r','u','r','r','r','l','d','r','d','r','l','d','r','d','d','u','d',
	'r','l','l','u','r','r','l','l','u','l','l','r','l','l','u','u','l','l'};
	double speeds[] {0.25,0.5,1.0,0.2,0.33,0.5,1.0,0.2,1.0,0.25,0.5,0.33,1.0,0.5,0.25};
	int speed_times[] {1,10,20,30,60,90,120,150,180,210,300,350,400,491,499};
	int num_moves = sizeof(moves);
	int num_speeds = sizeof(speeds) / sizeof(double);

	run_path(moves, speeds, speed_times, num_moves, num_speeds, gc_firing, t, g);
}

void move_path(double *gc_firing, int t, G* g) {
	// movement path

	char moves[] {'u','u','u','r','r','r','r','u','r','l','r','l','r','u','r','l','r','l','r','u','r','d',
	'r','u','r','r','r','d','d','l','u','u','r','r','u','u','l','l','d','r','r','d','d','r','r','l','l','d',
	'l','r','u','d','r','u','r','d','r','r','l','l','d','l','r','u','d','r','r','r','l','l','d','l','r','u',
	'd','r','d','r','u','r','d','r','r','l','l','d','l','r','u','d','r','r','r','l','d','l','l','u','u','l',
	'l','u','u','u','r','r','r','r','u','r','r','l','r','u','r','l','r','l','r','u','r','d','r','u','r','r',
	'r','d','d','l','u','d','r','r','l','l','u','u','r','u','u','r','d','r','r','l','l','d','l','r','u','d',
	'r','u','r','d','r','r','l','l','d','l','r','u','d','r','r','r','l','l','d','l','r','u','d','r','d','r',
	'u','r','d','r','r','l','l','d','l','r','u','d','r','r','r','l','d','l','l','u','u','l','l','u','u','u',
	'r','r','r','r','u','r','r','l','r','u','r','l','r','l','r','u','r','d','r','u','r','r','r','d','d','l',
	'u','d','r','r','l','l','u','u','r','u','u','r','d','r','r','l','l','d','l','r','u','d','r','u','r','d',
	'r','r','l','l','d','l','r','u','d','r','r','r','l','l','d','l','r','u','d','r','d','r','u','r','d','r',
	'r','l','l','d','l','r','u','d','r','r','r','l','d','l','l','u','u','l','l','u','u','u','r','r','r','r',
	'u','r','n','n','n','u','n','n','n','n','n','n','n','d','r','u','r','r','r','d','d','l','u','d','r','r',
	'l','l','u','u','r','u','u','r','d','r','r','l','l','d','l','r','u','d','r','u','r','d','r','r','l','l',
	'd','l','r','u','d','r','r','r','l','l','d','l','r','u','d','r','d','r','u','r','d','r','r','l','l','d',
	'l','r','u','d','r','r','r','l','d','l','l','u','u','l','l','r','r','l','d','l','l','u','u','l','l',};
	double speeds[] = {1.0};
	int speed_times[] = {1};
	int num_moves = sizeof(moves);
	int num_speeds = sizeof(speeds);

	run_path(moves, speeds, speed_times, num_moves, num_speeds, gc_firing, t, g);
}

void move_path2(double *gc_firing, int t, G* g) {
	// movement path

	char moves[] = {'n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n',
	'n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n',
	'n','n','n',};
	double speeds[] = {1.0};
	int speed_times[] = {1};
	int num_moves = sizeof(moves);
	int num_speeds = sizeof(speeds);

	run_path(moves, speeds, speed_times, num_moves, num_speeds, gc_firing, t, g);
}