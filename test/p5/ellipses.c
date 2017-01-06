int height;
int width;

void drawTarget(double xloc, double yloc, double size, double num) {
	int grayvalues, steps, i;
	grayvalues = 255/num;
	steps = size/num;
	for (i = 0; i < num; i++) {
		fill(i*grayvalues);
		ellipse(xloc, yloc, size - i*steps, size - i*steps);
	}
}

void my_setup() {
	width = 720;
	height = 320;
	createCanvas(width, height);
	background(51);
	noStroke();
	noLoop();
}

void my_draw() {
	drawTarget(width*0.25, height*0.4, 200, 4);
	drawTarget(width*0.5, height*0.5, 300, 10);
	drawTarget(width*0.75, height*0.3, 120, 6);
}
