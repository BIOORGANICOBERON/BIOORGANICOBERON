#if defined __need_mbstate_t
//for wprintf
#error error: __need_mbstate_t must not be defined, but it is defined.
#endif

#if defined __need_wint_t
//for wprintf
#error error: __need_wint_t must not be defined, but it is defined.
#endif

#include "common.h"

//for wprintf
#include <stdio.h>

//for wprintf
#include <wchar.h>

//for exit()
#include <stdlib.h>

void abortIfNull(void* ptr){
	if(ptr==0){
		wprintf(L"Not enough memory.\n");
		exit(2);
	}
}
