/*	THE HASHER

	30 September 1987 - Michael Zanussi
*/


#include <stdio.h>

#define HASH_SIZE 67	/* size of this hashtable */
#define STRINGLEN 81	/* maximum string length */

struct node {			/* individual element in hashtable */
	char sfield[STRINGLEN];	/* string field                    */
	int nfield;		/* value field                     */
	struct node *next;	/* pointer to next node            */
};

static struct node *hashtable[HASH_SIZE];	/* the hashtable   */


main(argc, argv)
	int argc;
	char *argv[];
{
	int line;			/* line from input file being worked on	*/
	char strfld[STRINGLEN];		/* string fetched from input file	*/
	int numfld;			/* value fetched from input file	*/
	struct node *ptr;		/* temporary pointer to node in table	*/
	char comm;			/* command fetched from input file	*/
	int hash_index();
	struct node *hash_enter();
	struct node *hash_lookup();	/* general function declarations	*/
	FILE *fp, *fopen();

	line = 0;

	if (argc == 1)
		printf("ERROR(0):  NO FILESPEC\n");
	else {
		fp = fopen (*++argv, "r");
		while(fscanf(fp, "%c %s %d\n", &comm, strfld, &numfld) != EOF) {
			line++;
			switch(comm) {
				case 'e':
					ptr = hash_lookup(strfld);		/* if ptr returns null    */
					if (ptr == NULL)			/* enter input, otherwise */
						hash_enter(strfld,numfld);	/* print an error         */
					else {
						printf("ERROR(%d):  ", line);
						printf("MULTIPLY DEFINED SYMBOL (%s, %d).\n", strfld,numfld);
					}
					ptr = NULL;
					break;
				case 'c':
					ptr = hash_lookup(strfld);		/* look for the string in */
					if (ptr == NULL)			/* the table.  if its not */
						hash_enter(strfld, numfld);	/* there, enter the input */
					else					/* otherwise edit the     */
						hash_edit(strfld, numfld);	/* current value          */
					ptr = NULL;
					break;
				case 'l':
					ptr = hash_lookup(strfld);		/* find the string. if    */
					if (ptr == NULL) {			/* its there confirm it   */
						printf("ERROR(%d):   ", line);	/* otherwise its an error */
						printf("UNDEFINED SYMBOL (%s).\n", strfld);
					}
					else
						printf("FOUND entry (%s).\n", strfld);
					ptr = NULL;
					break;
				default:				/* error for illegal com. */
					printf("ERROR(%d):  ", line);
					printf("ILLEGAL COMMAND (%c).\n", comm);
					break;
			}
		}
		fclose(fp);
		hash_print();
	}
}


/* searches hashtable for specific element */

struct node *hash_lookup(s)
	char *s;
{
	struct node *fptr;
	int index;

	index = hash_index(s);
	fptr = hashtable[index];

	while (fptr != NULL) {
		if (strcmp(s, fptr->sfield) == 0)
			return(fptr);
		fptr = fptr->next;
	}
	return(NULL);
}


/* find the 'magic number' for the string for the hashtable */

hash_index(s)
	char *s;
{
	int asciival = 0;
	long index = 1;
	int i = 0;
	int j = 16;

	/* find ASCII value for each character, do a calculation on it, then 
	   add them together */

	while (((asciival = (int) *s) != '\0') && (i < 8) && (j > 0)) {
		index = index + ((i * j + 1) * asciival);
		i++;
		j-=2;
		s++;
	}
	return(index % HASH_SIZE);
}


/* enters new elements into hashtable */

struct node *hash_enter(s, n)
	char *s;
	int n;
{
	struct node *fptr, *newnode;	/* table pointer, new node pointer	*/
	char *calloc();
	int index;			/* hash index of string			*/

	index = hash_index(s);
	fptr = hashtable[index];

	/* if the table pointer is null, then there are no elements in that array 
	   position.  create a new node, put in the string and value, then attach 
	   it to the table.  if the t able pointer isn't null then there are nodes 
	   in that array position.  find the end of the list in that position, 
	   create a new node, put in the string and value, and attach to last node */

	if (fptr == NULL) {
		newnode = (struct node *)calloc(1,sizeof(struct node));
		strcpy(newnode->sfield, s);
		newnode->nfield = n;
		newnode->next = NULL;
		hashtable[index] = newnode;
		return;
	}
	else {
		while (fptr->next != NULL)
			fptr = fptr->next;
		newnode = (struct node *)calloc(1,sizeof(struct node));
		strcpy(newnode->sfield, s);
		newnode->nfield = n;
		newnode->next = NULL;
		fptr->next = newnode;
		return;
	}
}


/* print the hashtable */

hash_print()
{
	struct node *fptr;
	int index;

	index = 0;

	printf("\nHASH TABLE ELEMENTS\n");
	printf("-------------------\n");
	while (index < HASH_SIZE) {
		fptr = hashtable[index];
		if (fptr == NULL)
			index++;
		else {
			while (fptr != NULL) {
				printf("  %s %d\n", fptr->sfield, fptr->nfield);
				fptr = fptr->next;
			}
			index++;
		}
		fptr = NULL;
	}
	return;
}


/* edit the value in the table specified by the string and value */

hash_edit(s, n)
	char *s;
	int n;
{
	struct node *fptr;	/* table pointer */

	fptr = hash_lookup(s);
	fptr->nfield = n;
	return;
}
