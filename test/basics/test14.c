// Test foncion reccursive
// fibo(10) == 55 donc doit renvoyer 0
int fibo(int n) {
	if (n == 0)
	{
		return 0;
	}
	if (n == 1) {
		return 1;
	}
	return fibo(n - 1) + fibo(n - 2);
}

int main() {
	return fibo(10) - 55;
}
