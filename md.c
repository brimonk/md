// Brian Chrzanowski
// 2021-09-25 01:33:51
//
// a simple markdown - html processor - goal < 200 lines
//
// USAGE:
//     md [<input>] [<output>]
//     reads and writes to stdin / stdout if files aren't present
//
// MARKDOWN CHEAT SHEET:
//     https://www.markdownguide.org/cheat-sheet/
//
// Read all of the text into a buffer, ignoring '\r' and replacing '\n\n' with just '\n'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

char *get_input(void)
{
	char *s;
	size_t cap, len;
	int c;

	cap = 4096;
	len = 0;

	s = calloc(1, cap);

	while ((c = getchar()) != EOF) {
		if (c == '\r')
			continue;

		s[len++] = (char)c;

		if (len >= cap) {
			cap *= 2;
			s = realloc(s, cap);
		}
	}

	s[len] = 0;

	return s;
}

char *ltrim(char *s)
{
	while (s && isblank(*s))
		s++;
	return s;
}

char *rtrim(char *s)
{
	char *t;
	t = s ? s + strlen(s) : NULL;
	while (s && t && isblank(*t))
		*t-- = '\0';
	return s;
}

char *trim(char *s)
{
	return ltrim(rtrim(s));
}

int streq(char *s, char *t)
{
	return strlen(s) == strlen(t) && strcmp(s, t) == 0;
}

int print_quote(char *s)
{
	return 0;
}

// print_link: returns the number of characters to advance
size_t print_link(char *s)
{
	char *t, *x, *y;

	s++;

	t = strchr(s, ']');
	assert(t != NULL);

	x = strchr(t, '(');
	assert(x != NULL);

	x++;

	y = strchr(x, ')');
	assert(y != NULL);

	printf("<a href=\"%.*s\">%.*s</a>", (int)(y - x), x, (int)(t - s), s);

	return (y) - (s - 1);
}

size_t print_image(char *s)
{
	char *t, *x, *y;

	s = strchr(s, '[');
	assert(s != NULL);

	s++;

	t = strchr(s, ']');
	assert(t != NULL);

	x = strchr(t, '(');
	assert(x != NULL);

	x++;

	y = strchr(x, ')');
	assert(y != NULL);

	printf("<img src=\"%.*s\" alt=\"%.*s\"></img>", (int)(y - x), x, (int)(t - s), s);

	return (y) - (s - 2);
}

int main(int argc, char **argv)
{
	char *input;
	char *s, *t;

	int in_quote = false;
	int in_olist = false;
	int in_ulist = false;
	int in_code = false;

	if (2 <= argc)
		freopen(argv[1], "r", stdin);
	if (3 <= argc)
		freopen(argv[2], "w", stdout);

	input = get_input();

	for (s = strtok(input, "\n"); s; s = strtok(NULL, "\n")) {
		if (*s == '#') {
			int heading;
			for (t = s; *t == '#'; t++)
				;
			heading = t - s;
			t = trim(t);
			printf("<h%d>%s</h%d>\n", heading, t, heading);

			continue;
		}

		if (*s == '>') {
			if (in_quote) {
				printf("%s\n", trim(s + 1));
			} else {
				printf("<blockquote><p>\n");
				printf("%s\n", trim(s + 1));
				in_quote = true;
			}

			continue;
		} else if (*s != '>' && in_quote) {
			printf("</p></blockquote>\n");
			in_quote = false;

			continue;
		}

		if (isdigit(*s)) {
			if (!in_olist) {
				printf("<ol>\n");
				in_olist = true;
			}
			t = strchr(s, '.');
			printf("\t<li>%s</li>\n", trim(t + 1));

			continue;
		}

		if (!isdigit(*s) && in_olist) {
			printf("</ol>\n");
			in_olist = false;

			continue;
		}

		if (*s == '*' && *(s + 1) == ' ') {
			if (!in_ulist) {
				printf("<ul>\n");
				in_ulist = true;
			}
			printf("\t<li>%s</li>\n", trim(s + 1));

			continue;
		}

		if (*s != '*' && in_ulist) {
			printf("</ul>\n");
			in_ulist = false;

			continue;
		}

		if (streq(s, "```")) {
			if (in_code) {
				printf("</code></pre>\n");
			} else {
				printf("<pre><code>");
			}

			in_code = !in_code;

			continue;
		}

		if (in_code) {
			printf("%s\n", s);
			continue;
		}

		if (streq(s, "---")) {
			printf("<hr>\n");
			continue;
		}

		// and finally, "real" output

		{
			int bold, ital, under;

			printf("<p>");

			bold = ital = under = 0;

			for (; *s; s++) {
				if (*s == '_') {
					if (under) {
						printf("<u>");
					} else {
						printf("</u>");
					}

					under = !under;
					continue;
				}

				if (*s == '*') { // perform a lookahead to find the next stars
					if (s[0] == '*' && s[1] == '*' && s[2] == '*') {
						bold = !bold;
						ital = !ital;

						if (bold && ital) {
							printf("<b>");
							printf("<i>");
						} else {
							printf("</i>");
							printf("</b>");
						}

						s += 2;
					} else if (s[0] == '*' && s[1] == '*') {
						bold = !bold;
						printf("<%sb>", !bold ? "/" : "");

						s += 1;
					} else if (s[0] == '*') {
						ital = !ital;
						printf("<%si>", !ital ? "/" : "");
					}

					continue;
				}

				if (s[0] == '!' && s[1] == '[') { // image
					s += print_image(s);
				}

				if (s[0] == '[') {
					s += print_link(s);
					continue;
				}

				putchar(*s);
			}

			printf("</p>\n");
		}
	}

	free(input);

	return 0;
}

