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
	while (*dst) ++ dst;
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
		ss[i] = c;
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
		if(ss1[i] != ss2[i])
			return ss1[i] - ss2[i];
		if(ss1[i] == 0)
			break;
	}
	return 0;
}
