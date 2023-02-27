#include <blib.h>

size_t strlen(const char *s) {
	size_t num = 0;
	while(*s) {
		++ num;
		s += sizeof(char);
	}
	return num;
}

char *strcpy(char *dst, const char *src) {
	char *res = dst;
	while(*src) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
	char *res = dst;
	while (*src && n--) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strcat(char *dst, const char *src) {
	char *res = dst;
	while(*dst) ++ dst;
	while (*src) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

int strcmp(const char *s1, const char *s2) {
	while (1) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			break;
		}
		s1++;
		s2++;
	}
	return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			break;
		}
		s1++;
		s2++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	char *ss = (char *)s;
	for(int i = 0; i < n; ++ i) {
		*ss = c;
		++ ss;
	}
}

void *memcpy(void *out, const void *in, size_t n) {
	char *csrc = (char *)in;
	char *cdest = (char *)out;
	for (int i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	unsigned char *ss1 = (unsigned char *)s1;
	unsigned char *ss2 = (unsigned char *)s2;
	for (int i = 0; i < n; i++) {
		if(*ss1 != *ss2) return *ss1 - *ss2;
		else if(*ss1 == 0) break;
		++ ss1;
		++ ss2;
	}
	return 0;
}
