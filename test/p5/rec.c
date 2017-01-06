int height;
int width;

void drawCircle(double x, double radius, double level) {
	double tt;

	tt = 126 * level/4.0;
	fill(tt);
	ellipse(x, height/2, radius*2, radius*2);
	if(level > 1) {
		level = level - 1;
		drawCircle(x - radius/2, radius/2, level);
		drawCircle(x + radius/2, radius/2, level);
	}
}

void my_setup() {
	width = 720;
	height = 400;
	createCanvas(width, height);
	noStroke();
	noLoop();
}

void my_draw() {
	drawCircle(width/2, 280, 6);
}
