int height;
int width;

void my_draw() {

}

void my_setup() {
	int x,y, gridSize;

	width = 720;
	height = 360;
	createCanvas(width, height);
	background(0);
	noStroke();

	gridSize = 40;
	for (x = gridSize; x <= width - gridSize; x += gridSize) {
		for (y = gridSize; y <= height - gridSize; y += gridSize) {
			noStroke();
			fill(255);
			rect(x-1, y-1, 3, 3);
			stroke(255);
			line(x, y, width/2, height/2);
		}
	}

}
