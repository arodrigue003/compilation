// Test opérateurs d'asignation
// Doit retourner  0
int main() {
	int i;

	i = 100;
	i %= 3; //1
	i += 17; //18
	i /= 5; //3
	i <<= 2; //12
	i -= 2; //10
	i >>=1; //5
	i -= 5; //0

	return i;
}
