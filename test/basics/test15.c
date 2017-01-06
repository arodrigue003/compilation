// Verification pas de clash de variables entre plusieurs fonctions
// Doit retourner 0
void foo() {
	int i;
	i = 2;
}

int main() {
	int i;
	i = 0;
	foo();
	return i;
}
