// Test appel de fontions avec et sans paramètre(s)
// Doit retourner 0
int foo() {
	return 4;
}

int bar(int i) {
	return foo() - i;
}

int main() {
	int j;
	j = 3;
	return bar(j) - 1;
}
