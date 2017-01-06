// Test d'optimisation
// Doit renvoyer 0
int main() {
	int ret;
	ret = 2;

	if( (1 + 2 + 3 + 5 * 17) << 2 < 365)
		ret = 0;
	else
		ret = 1;

	return ret;
}
