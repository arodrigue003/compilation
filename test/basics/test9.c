// Test opérateurs logiques
int main() {
	int i,j,k, l;
	i = 1;
	j = 1;
	k = 3;
	l = 4;
	if ((i == j) && (k < l) && !(k < j) && ((k < j) || i < l))
		return 0;
	else
		return 1;

	return 2; //Normalement ce point ne peut pas être atteind
}
