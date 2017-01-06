// Test porté des variables
int a; //0

void foo() {
	a = 5;
}

int main(){
	int a;
	a = 0;
	foo(); // Le a local n'est pas changé
	return a; // donc ici a vaut encore 0
}
