int mystrcmp(const char *a, const char *b)
{
	while (*a != '\0' && *a == *b) {
		++a; ++b;
	}
	return *a - *b;
}
