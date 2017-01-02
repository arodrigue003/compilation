// Test simple de condition
// Doit retourner 0
int main()
{
	int ret;
	ret = 2;
	if ((3 > 2) && (2 > 1)) {
		ret  = 0;
	} else {
		ret = 1;
	}

	return ret;
}
