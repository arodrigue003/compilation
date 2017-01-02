int y;
int height;
int width;

// The statements in the setup() function 
// execute once when the program begins
void my_setup() {
	// createCanvas must be the first statement
	y = 100;
	width = 720;
	height = 400;
	createCanvas(width, height);
	stroke(255);     // Set line drawing color to white
	frameRate(30);
}

// The statements in draw() are executed until the 
// program is stopped. Each statement is executed in 
// sequence and after the last line is read, the first 
// line is executed again.
void my_draw() {
	background(0);   // Set the background to black
	y = y - 1;
	if (y < 0) {
		y = height;
	}
	line(0, y, width, y);
} 
