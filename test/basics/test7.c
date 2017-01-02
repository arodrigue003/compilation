// Test incrementations/decrementations post/pre-fix
// Test opposé
// Doit retourner 0
int main(){
	int i,j,k,l;
	i = 1;
	j = 2;
	k = 3;
	l = i++ + ++j; //4
	l = l + -j + 2 - k; //0
	return l;
}
