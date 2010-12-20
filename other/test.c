#include "tree/list.h"

int main(void) {
	List *myList = List_new();

	List_append(myList, strdup("eggs"));
	List_append(myList, strdup("tomatoes"));
	List_append(myList, strdup("noodles"));
	List_append(myList, strdup("meat"));
	List_print(myList);

	List_remove_all(myList);
	if (myList->length == 0)
		printf("Empty list\n");
	List_print(myList);

	return 0;
}
