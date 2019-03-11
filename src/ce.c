/*
 * Cyrillic Encoding, a program for conversion between Cyrillic code pages
 * Copyright (C) 1999  OnMind Systems
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: ce.c,v 1.4 2002/06/27 00:44:00 vadimp Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "config.h"
#include "cyrillic.h"
#include "fe.h"

int input_codepage = CYR_CP_DEFAULT;
int output_codepage = CYR_CP_DEFAULT;
int strict = 0;

void usage ( void ) {

	printf ( 
 
	"Usage: ce [options]\n"
	"Converts standard input from one Cyrillic code page to another.\n\n"
	
	"-i cp,   --input-codepage  Set Cyrillic code page of input to cp\n"
	"-o cp,   --output-codepage Set Cyrillic code page of output to cp\n"
	"-s,      --strict          Ignores everything but ASCII and\n"
	"                           Cyrillic letters valid for the input\n"
	"                           code page\n"
	"-h, -?,  --help            Display this help and exit\n"
	"-v,      --version         Display version information and exit\n\n"
	
	"cp can be one of the "
		"following:\n\n"
	"koi8-r    -- KOI8-R (default on UNIX-compatible platforms)\n"
	"cp866     -- MS-DOS CP866 (aka 'alternative', default on Win32 "
		"platforms)\n"
	"cp1251    -- Windows CP1251\n"
        "mac       -- Cyrillic encoding used on Apple Macintosh\n"
	"iso8859-5 -- ISO 8859-5\n\n"
	
	"Report bugs to Vadim Penzin <penzin@attglobal.net>\n"
	"Please make sure there are words 'Fresh Eye' in the Subject: line\n"
	
	);
	
	exit ( 0 );
	
}

void version ( void ) {
	
	printf ( 
	
	"ce ("PACKAGE" version "VERSION" ("PLATFORM" ["CYR_CP_NAME"]))\n"
	"Copyright (C) 1999 OnMind Systems.\n"
	"Fresh Eye is distributed in the hope that it will be useful,\n"
	"but THERE IS ABSOLUTELY NO WARRANTY OF ANY KIND for this software.\n"
	"You may redistribute copies of Fresh Eye\n"
	"under the terms of the GNU General Public License.\n"
	"For more information, see the file named COPYING.\n" 
	
	);
		
	exit ( 0 );
}

int parse_command_line ( int argc, char* argv [] ) {

	static const char* options = "i:o:shv?";
	int option_index;
	static struct option long_options [] = {
		{ "input-codepage", 1, NULL, 'i' },
		{ "output-codepage", 1, NULL, 'o' },
		{ "strict", 0, NULL, 's' },
		{ "help", 0, NULL, 'h' },
		{ "version", 0, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};
	int ch;

	while ( (ch = getopt_long ( argc, argv, options, long_options, 
			&option_index )) != -1 )
		switch ( ch ) {
			case 'i':
				input_codepage = 
					cyr_codepage_by_name ( optarg );
				break;

			case 'o':
				output_codepage =
					cyr_codepage_by_name ( optarg );
				break;
	
			case 's':
				strict = 1;
				break;
				
			case '?':
			case 'h':
				usage ();
				
			case 'v':
				version ();

		}

	return optind;
}

int main ( int argc, char* argv [] ) {
	
	int ch;
	
	parse_command_line ( argc, argv );

	if ( input_codepage == CYR_CP_UNDEFINED ) {
		fprintf ( stderr, "ce: Bad input code page specified\n" );
		return -1;
	}
	
	if ( output_codepage == CYR_CP_UNDEFINED ) {
		fprintf ( stderr, "ce: Bad output code page specified\n" );
		return -1;
	}

	while ( (ch = getchar ()) != EOF )
		if ( !cyr_isletter_ex ( (cyr_letter) ch, input_codepage ) ) {
			if ( !strict )
				putchar ( translate_special_character (
					output_codepage, input_codepage, ch ) );
			else {
				if ( ch < 0x80 )
					putchar ( ch );
			}
		}
		else {
			int ord = cyr_ord_ex ( (cyr_letter) ch, 
					input_codepage );
			putchar ( cyr_chr_ex ( ord, output_codepage ) );
		}
	
	return 0;
}

