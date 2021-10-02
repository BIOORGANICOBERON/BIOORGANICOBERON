#ifndef COMMON_H_
#define COMMON_H_

void abortIfNull(void* ptr);
static int mywstrlen(const wchar_t* p){
	int len = 0;
	while(*p++)len++;
	return len;
}
wchar_t* wstrconcat(const wchar_t* a, int alenchars, const wchar_t* b, const int blenchars);

#endif /* COMMON_H_ */
