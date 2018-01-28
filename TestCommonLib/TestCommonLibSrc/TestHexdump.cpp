/*

hexdump.c    reversible hexdump

Copyright (c) 1996 by   Andreas Leitgeb (AvL) <avl@logic.tuwien.ac.at>

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.

*/


#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

int ch[16], line = 0;

void outputbyte(int i, int max);

/* if the isprint(c) function  does not exist on your system
(or for PC/MS-DOS  machines)   you can try to replace it  by
'(c>=32)'
*/
int hexdump(int argc, char *argv[]) {
	int c, i, diff = 1, noabbr = 0, flush = 0;
	char *infname = NULL, *outfname = NULL;

	/* Argument handling */
	for (i = 1; i < argc; ++i) {
		if (argv[i][0] != '-') {
			if (infname) {
				fprintf(stderr, "%s: More than one inputfile given.\n", argv[0]);
				exit(2);
			}
			infname = argv[i];
		}
		else {
			switch (argv[i][1]) {
			case 'i': if (argv[i][2]) goto usage;
				if (i + 1 == argc) {
					fprintf(stderr, "%s: Argument -%c requires a filename given.\n", argv[0], 'i');
					exit(2);
				}
				if (infname) {
					fprintf(stderr, "%s: More than one %sputfile given.\n", argv[0], "in");
					exit(2);
				}
				infname = argv[i + 1]; i++; break;
			case 'o': if (argv[i][2]) goto usage;
				if (i + 1 == argc) {
					fprintf(stderr, "%s: Argument -%c requires a filename given.\n", argv[0], 'o');
					exit(2);
				}
				if (outfname) {
					fprintf(stderr, "%s: More than one %sputfile given.\n", argv[0], "out");
					exit(2);
				}
				outfname = argv[i + 1]; i++; break;
			case 'n': if (argv[i][2]) goto usage;
				noabbr = 1; break;
			case 'f': if (argv[i][2]) goto usage;
				flush = 1; break;
			default:
			usage:
				fprintf(stderr, "Usage: %s [[-i] inputfile] [-o outfile] [-n]\n", argv[0]);
				fprintf(stderr, "  if in/outfile are omitted, stdin/stdout are used instead\n");
				fprintf(stderr, "  -n: repeating lines will NOT be abbreviated (no '*'-lines).\n");
				fprintf(stderr, "  -f: show each incoming byte. (implies -n) \n");
				exit(2);
			}
		}
	}
	if (infname && !freopen(infname, "rb", stdin)) {
		fprintf(stderr, "%s: Cannot open %s for reading.\n", argv[0], infname);
		perror(argv[0]);
		exit(1);
	}
	if (outfname && !freopen(outfname, "w", stdout)) {
		fprintf(stderr, "%s: Cannot open %s for writing.\n", argv[0], outfname);
		perror(argv[0]);
		exit(1);
	}
	if (flush) { noabbr = 1; }

	/*
	three states:
	diff==1 :  usual state: hexdump a chunk of 16 Bytes
	diff==2 :  this line is the first repetition of the previous line.
	output "*\n"
	diff==0 :  more repeating lines: nothing to do.
	*/
	i = 0; line = 0;
	while (!feof(stdin) && ((c = fgetc(stdin)) != EOF)) {
		if (ch[i & 15] != c) { ch[i & 15] = c; diff = 1; }
		if (flush) {
			outputbyte(i, 16);
			fflush(stdout);
		}
		else if ((i & 15) == 15) { /* chunk complete */
			if (diff == 1 || noabbr) {
				for (diff = 0; diff <= 15; diff++) {
					outputbyte(diff, 16);
				}
				diff = 2;
			}
			else if (diff == 2) { printf("*\n"); diff = 0; }
		}
		i++;  if ((i & 15) == 0) { line += i; i = 0; }
	}
	/* below, the variable `diff' is used as backup for `i' */
	if (i) {
		diff = i;
		/*    if flush was on,   then `i' bytes have already been output */
		if (!flush) { i = 0; }      /* otherwise, they still need to ! */

		for (; i <= 15; i++) {
			outputbyte(i, diff);
		}
		line += diff;
	}
	printf("%08x;\n", line);
	return 0;
}

void outputbyte(int i, int max) {
	int c = ch[i];
	/* reads global variable: line */

	if ((i & 15) == 0) { printf("%08x: ", line); }

	printf((i < max) ? "%2.2X " : "   ", c);
	if ((i & 15) == 7) { printf("- "); }

	if ((i & 15) == 15) {
		printf("|");
		for (i = 0; i < max; i++) {
			c = ch[i];
			if (isprint(c)) { printf("%c", c); }
			else { printf(" "); }
		}
		printf("|\n");
	}
}

#include "../../CommonLib/CommonLibSrc/SString/String.h"
#include <sstream>
void ut_print_buf_hex(
	std::ostream&	o,	/*!< in/out: output stream */
	const void*	buf,	/*!< in: memory buffer */
	int		len)	/*!< in: length of the buffer */
{
	const char*		data;
	int			i;

	static const char	hexdigit[16] = {
		'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
	};

	o << "[0x";

	for (data = static_cast<const char*>(buf), i = 0; i < len; i++) {
		char b = *data++;
		o << hexdigit[(int)b >> 4] << hexdigit[b & 0x0F];
		if (i + 1 < len)
		{
			o << " ";
		}
	}

	o << "]";
}

namespace smm
{
	String BinToHexStr(const StrImg& bin)
	{
		String dst;
		dst.ReSize(bin.Size()*2);
		char* p = const_cast<char*>(dst.ToCStr());
		for (int i = 0; i < bin.Size(); i++)
		{
			char c = bin.GetAt(i);
			char s[3] = {0};
			sprintf(s, "%02x", c);
			dst.Append(s);
		}
		return dst;
	}
	void TestSHexdump()
	{
		SARRAY(src, 512);
		src.Append("1a");
		String dst;
		dst = BinToHexStr(src);
		cout << dst.ToCStr() << endl;
	}
	void TestUtPrintHex()
	{
		std::string str("123");
		std::ostringstream sm;
		ut_print_buf_hex(sm,str.c_str(),str.size());
		cout << sm.str() << endl;
	}
};
