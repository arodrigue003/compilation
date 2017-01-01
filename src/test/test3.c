// Test condition avec paramètres
// Doit retourner 0 lorsque argc > 1 soit lorsqu'un argument est passé au programme
int main(int argc)
{
	int ret;
	ret = 2;
	if (argc > 1) {
		ret  = 0;
	} else {
		ret = 1;
	}

	return ret;
}

