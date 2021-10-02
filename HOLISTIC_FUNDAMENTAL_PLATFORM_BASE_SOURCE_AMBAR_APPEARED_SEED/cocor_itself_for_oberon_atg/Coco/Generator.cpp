/*----------------------------------------------------------------------
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

As an exception, it is allowed to write an extension of Coco/R that is
used as a plugin in non-free software.

If not otherwise stated, any source code generated by Coco/R (other than
Coco/R itself) does not fall under the GNU General Public License.
-----------------------------------------------------------------------*/

#include "Generator.h"
#include "Scanner.h"

#include "errno.h"

namespace Coco {

	Generator::Generator(Tab *tab, Errors *errors) {
		this->errors = errors;
		this->tab = tab;
		fram = NULL;
		gen = NULL;
		frameFile = NULL;
	}

	FILE* Generator::OpenFrame(const wchar_t* frame) {
		if (coco_string_length(tab->frameDir) != 0) {
			frameFile = coco_string_create_append(tab->frameDir, L"/");
			coco_string_merge(frameFile, frame);
			char *chFrameFile = coco_string_create_char(frameFile);
			fram = fopen(chFrameFile, "r");
			delete [] chFrameFile;
		}
		if (fram == NULL) {
			delete [] frameFile;
			frameFile = coco_string_create_append(tab->srcDir, frame);  /* pdt */
			char *chFrameFile = coco_string_create_char(frameFile);
			fram = fopen(chFrameFile, "r");
			delete [] chFrameFile;
		}
		if (fram == NULL) {
			wchar_t *message = coco_string_create_append(L"-- Cannot find : ", frame);
			errors->Exception(message);
			delete [] message;
		}

	    return fram;
	}


	FILE* Generator::OpenGen(const wchar_t *genName) { /* pdt */
		wchar_t *fn = coco_string_create_append(tab->outDir, genName); /* pdt */
		char *chFn = coco_string_create_char(fn);

		if ((gen = fopen(chFn, "r")) != NULL) {
			fclose(gen);
			wchar_t *oldName = coco_string_create_append(fn, L".old");
			char *chOldName = coco_string_create_char(oldName);
			remove(chOldName); rename(chFn, chOldName); // copy with overwrite
			coco_string_delete(chOldName);
			coco_string_delete(oldName);
		}
		if ((gen = fopen(chFn, "w")) == NULL) {
			wchar_t *message = coco_string_create_append(L"-- Cannot generate : ", genName);
			errors->Exception(message);
			delete [] message;
		}
		coco_string_delete(chFn);
		coco_string_delete(fn);

		return gen;
	}


	void Generator::GenCopyright() {
		FILE *file = NULL;

		if (coco_string_length(tab->frameDir) != 0) {
			wchar_t *copyFr = coco_string_create_append(tab->frameDir, L"/Copyright.frame");
			char *chCopyFr = coco_string_create_char(copyFr);
			file = fopen(chCopyFr, "r");
			delete [] copyFr;
			delete [] chCopyFr;
		}
		if (file == NULL) {
			wchar_t *copyFr = coco_string_create_append(tab->srcDir, L"Copyright.frame");
			char *chCopyFr = coco_string_create_char(copyFr);
			file = fopen(chCopyFr, "r");
			delete [] copyFr;
			delete [] chCopyFr;
			int retcode=file!=NULL?fseek(file, 0L,SEEK_CUR):-1;
			if(retcode!=0){
				wchar_t *message;
				wchar_t* str=coco_string_create(strerror(errno));
				message = coco_string_create_append(L" -- Error opening frame file: ", frameFile);
				wchar_t* msg2=coco_string_create_append(message, L": ");
				wchar_t* msg3=coco_string_create_append(msg2, str);
				errors->Exception(msg3);
				delete [] message;
				delete [] str;
				delete [] msg2;
				delete [] msg3;
			}
		}
		if (file == NULL) {
			return;
		}

		FILE *scannerFram = fram;
		fram = file;

		CopyFramePart(NULL);
		fram = scannerFram;

		fclose(file);
	}

	void Generator::GenPrefixFromNamespace() {
		const wchar_t *nsName = tab->nsName;
		if (nsName == NULL || coco_string_length(nsName) == 0) {
			return;
		}
		const int len = coco_string_length(nsName);
		int startPos = 0;
		do {
			int curLen = coco_string_indexof(nsName + startPos, COCO_CPP_NAMESPACE_SEPARATOR);
			if (curLen == -1) { curLen = len - startPos; }
			wchar_t *curNs = coco_string_create(nsName, startPos, curLen);
			fwprintf(gen, L"%ls_", curNs);
			coco_string_delete(curNs);
			startPos = startPos + curLen + 1;
		} while (startPos < len);
	}

	void Generator::SkipFramePart(const wchar_t *stop) {
		CopyFramePart(stop, false);
	}

	void Generator::CopyFramePart(const wchar_t *stop) {
		CopyFramePart(stop, true);
	}

	void Generator::CopyFramePart(const wchar_t* stop, bool generateOutput) {
		wchar_t startCh = 0;
		int endOfStopString = 0;
		wchar_t ch = 0;

		if (stop != NULL) {
			startCh = stop[0];
			endOfStopString = coco_string_length(stop)-1;
		}

//		int retcode = fwscanf(fram, L"%lc", &ch);
		wint_t retcode1 = fgetwc(fram);
		if(retcode1 == (wint_t) WEOF) {
			wchar_t *message;
			wchar_t* str=coco_string_create(strerror(errno));
			message = coco_string_create_append(L" -- Error reading data from frame file: ", frameFile);
			wchar_t* msg2=coco_string_create_append(message, L": ");
			wchar_t* msg3=coco_string_create_append(msg2, str);
			errors->Exception(msg3);
			delete [] message;
			delete [] str;
			delete [] msg2;
			delete [] msg3;
			return;
		}
		ch = (wchar_t) retcode1;
		while (!feof(fram)) { // ch != EOF
			if (stop != NULL && ch == startCh) {
				int i = 0;
				do {
					if (i == endOfStopString) return; // stop[0..i] found
					fwscanf(fram, L"%lc", &ch); i++;
				} while (ch == stop[i]);
				// stop[0..i-1] found; continue with last read character
				if (generateOutput) {
					wchar_t *subStop = coco_string_create(stop, 0, i);
					fwprintf(gen, L"%ls", subStop);
					coco_string_delete(subStop);
				}
			} else {
				if (generateOutput) {
					wint_t retcode = fputwc(ch, gen);//fwprintf(gen, L"%lc", ch);
					if(retcode == WEOF) {
						wchar_t *message = coco_string_create_append(L" -- Error generating data from frame file: ", frameFile);
						errors->Exception(message);
						delete [] message;
						return;
					}
				}
				wint_t retcode = fgetwc(fram); //fwscanf(fram, L"%lc", &ch);
				if(retcode == WEOF && !feof(fram)) {
					wchar_t *message;
					wchar_t* str=coco_string_create(strerror(errno));
					message = coco_string_create_append(L" -- Error reading data from frame file: ", frameFile);
					wchar_t* msg2=coco_string_create_append(message, L": ");
					wchar_t* msg3=coco_string_create_append(msg2, str);
					errors->Exception(msg3);
					delete [] message;
					delete [] str;
					delete [] msg2;
					delete [] msg3;
					return;
				}
				ch = (wchar_t) retcode;
			}
		}
		if (stop != NULL) {
			wchar_t *message = coco_string_create_append(L" -- Incomplete or corrupt frame file: ", frameFile);
			errors->Exception(message);
			delete [] message;
		}
	}

}
