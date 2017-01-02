// Test variables globals
// Doit retourner 0
int i;
int j;

void foo() {
	i = 4;
	j = 7;
}

int main() {
	foo();
	return j - i -3;
}
