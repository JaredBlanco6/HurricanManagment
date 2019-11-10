/*

  Author: Jared Blanco
  Email: JBlanco2018@my.fit.edu
  Course: CSE 2010
  Section:03
  Description:

    My program is broken up into two steps, building the tree and running the query.
    1. To build the tree i tokenize the input and pass it into my 'add_children' function that recusivly finds the correct parent for my child
    2. To run the query I tokenize the file, taking the first word of ever line as the function call, and use strcmp to call the correct function.

    ALL of get 'Get**' functions are recursive, I found this to be the most fun way to do the assignment.
    This is all possible becuase my inosrt function, insort insers strings in order and returns various intergers based on added input, these return values are used in my two of my recursive functions.

    I only use one structure in this assignment that is used both for Tree nodes and Linked List nodes inorder to make the programers life easier.




 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Tree_t{
  //will be either hurricane, a catagory, or the name of a specific hurricane
  char name[50];
  //a single pointer to my parent node
  struct Tree_t* parent;
  //pointer to a list of children
  struct Tree_t* children;
  //points to the text node in the list
  struct Tree_t* next;

}Tree_t;



//all functions are bellow main for organizational purposes
void make_tree(Tree_t **root, char *file_name);
void freeList(Tree_t** head);
void add_children(Tree_t **current_node, char parent_name[50], char child_name[50]);
int insort(Tree_t **root, Tree_t *parent_address, char string[50]);
void print_tree(Tree_t* list);
int print_list(Tree_t* list);
void run_query(char *file_name, Tree_t **root, Tree_t ** query);
void add_query(Tree_t **query, Tree_t *node);
//all of these functions stricly use recursion
void GetNamesByCategory(char catagory[50], Tree_t* list, Tree_t **query);
void GetNamesWithMultipleStates(Tree_t *list, Tree_t **query, Tree_t **temp_list);
void GetNamesByCategoryAndState(char catagory[50], char state[50], Tree_t* list, Tree_t **query);
void GetState(char name[50], Tree_t* list, Tree_t **query);
void GetCategory(char name[50], Tree_t* list, Tree_t **query);
void GetNamesWithMultipleCategories(Tree_t *list, Tree_t **query, Tree_t **temp_list);
void GetNamesByState(char state[50], Tree_t* list, Tree_t **query);

/* --------------------------- */
int main(int argc, char* argv[]){
  Tree_t *root = NULL;
  Tree_t *query = NULL;
  //i know this will be my root so i just allocate memory immidiatly
  insort(&root, NULL, "hurricane");

  //takes in data input, tokenizes it, than passes input into my add_children function to recursivly give the correct parent their children
  make_tree(&root, argv[1]);

  //takes in all function calls from query input, calls in correct funciton than adds all input to a 'query' list that is printed for every function call
  run_query(argv[2], &root, &query);

  freeList(&root);
  freeList(&query);

  return 0;

}
/* --------------------------- */
//Insort means 'Insert and sort', Inserts the nodes in alphabetical order.
//This function will return 0 if nothing went wrong, return 1 if we attempted to add a duplicate, and return 2 if those duplicates had the same grandparents,This is for my 'GetNamesWithMultipleStates' and 'GetNamesWithMultipleCategories' functions
//Takes in 'root' as the current list to add too as well as the parent address to refer too
 int insort(Tree_t **root, Tree_t *parent_address, char string[50]){

  Tree_t *temp = NULL;
  temp = *root;

  //decalres the new_node
  Tree_t *new_node = (Tree_t*)malloc(sizeof(Tree_t));
  strcpy(new_node->name, string);
  new_node-> parent= parent_address;
  new_node-> children= NULL;
  new_node-> next = NULL;


  //if the list is empty
  if(temp == NULL){
    *root = new_node;

  }else{
    //while: list has not ended AND string is bigger than temp to add it at the back
    while((temp->next != NULL) && (strcmp(new_node->name, temp->next->name) >= 0)){

      //CHECKS FOR DUPLICATES, returns 1 if nomal duplicates, retunrs 2 if the duplicates would have different grandpaernts
      if((strcmp(new_node->name, temp->next->name) == 0)){

          //if these two duplicate names have multiple grandparents/catagories return 2
          if((temp->parent->parent != NULL) && (new_node->parent->parent != NULL) && (strcmp(new_node->parent->parent->name, temp->next->parent->parent->name) != 0)){
            //used inside 'GetNamesWithMultipleCategories'
            return 2;
          }

        //used inside 'GetNamesWithMultipleStates'
        return 1;
      }

      //scrolls through list
      temp = temp->next;
    }

    //if adding to the head
    if(temp == *root){

      //CHECKS FOR DUPLICATES when addng to my head, returns 1 if nomal duplicates, retunrs 2 if the duplicates would have different grandpaernts
      if(strcmp(new_node->name, temp->name) == 0){

        //if these two duplicate names have multiple grandparents/catagories return 2
        if((temp->parent != NULL) && (temp->parent->parent != NULL) && (new_node->parent != NULL) && (new_node->parent->parent != NULL)){
          if((strcmp(new_node->parent->parent->name, temp->parent->parent->name) != 0)){
            //used inside 'GetNamesWithMultipleCategories'
            return 2;
          }
        }
        return 1;
      }



      //if string is bigger than current head
      if((strcmp(new_node->name, temp->name) > 0)){
        //my new string is bigger the the old head
        new_node->next = temp->next;
        temp->next = new_node;
      }else{
        //my new string is smaller than the old head
        new_node->next = temp;
        *root = new_node;
      }

    }else{

      //insert the node in the normaly
      new_node->next = temp->next;
      temp->next = new_node;

    }

  }
  return 0;
}

/* --------------------------- */
//scans in the input file data, tokenizes the string and send the data to my 'add_children' funcition
void make_tree(Tree_t **root, char *file_name){
  //opens the file
  FILE *fin = fopen(file_name, "r");

  //used for scanning and tokenizing
  char temp_input[200];
  int input_length;
  char *token;

  //i pass these into my add_children function,
  //the parent_name is the first word in the dataInput
  //child_name is all of the follwoing words becuase those are the names of children behind the parent_name
  char parent_name[50];
  char child_name[50];



  while(fgets(temp_input, 200, fin)){

    //string length, used to get rid of 'invisable chars'
    input_length = strlen(temp_input);


    //removes all of the non-printable characters from my scanned in input
    for(int i = 0; temp_input[input_length - i] <= '!'; i++){
      //if the end of my string has a char that has an ascii vulue less than '!' , implies that it is an unreadable char so i make it null
      temp_input[input_length - i] = '\0';
    }


    //gets the first word from the scanned in line. Will act as the parent for the remaining children
    token = strtok(temp_input, " ");
    strcpy(parent_name, token);

    //loop will tokenize the remaining input and add those children to my tree

    while(1){
      //while 'true' loop, to be able to break right after tokenize comes back NULL
      token = strtok(NULL, " ");

      if(token == NULL){
        break;
      }

      strcpy(child_name, token);

      //recusivly travels through function to find matching parent, than adds name to children list
      add_children(&(*root), parent_name, child_name);

    }
  }

  fclose(fin);
}

/* --------------------------- */
//seaches for a parent and gives them their kid
void add_children(Tree_t **current_node, char parent_name[50], char child_name[50]){
  //base case is we've ran out of children to look at
  if(*current_node == NULL){
    return;
  }

  //will go through the list
  add_children(&(*current_node)->next, parent_name, child_name);
  add_children(&(*current_node)->children, parent_name, child_name);

  //if we find the correct parent to give their children too
  if(strcmp(parent_name, (*current_node)->name) == 0){
    //insert the child into the parents children list
    insort(&((*current_node)->children), *current_node, child_name);
  }


}

/* --------------------------- */
//recursive way of printing the tree, prints a node, than calls funciton again with the next node and children node
void print_tree(Tree_t* list){

  //base case: if there is no list, do nothing
  if(list == NULL){
    return;
  }
  //visit
  printf("%s ", list->name);

  //continues through the tree
  print_tree(list->next);
  print_tree(list->children);


}

/* --------------------------- */
//recursive way of printing the tree, prints a node, than calls funciton again with the next node. Returns 0 if nothing printed and returns a number greater than 0 if something did print
int print_list(Tree_t* list){

  //base case: if there is no list, do nothing
  if(list == NULL){
    return 0;
  }
  printf("%s ", list->name);
  return 1 + print_list(list->next);


}

/* --------------------------- */
//recursive way to free my entire tree!
void freeList(Tree_t** head){
  //base case: if we have no list, don't do anything
  if(*head == NULL){
    return;
  }
  //calls the same function with the next node
  freeList(&(*head)->next);
  freeList(&(*head)->children);


  //free's the current node
  free(*head);
  //fun fact: freeing something does not make it point to NULL but instead just deallcates the memeory and leaves the pointer
  *head = NULL;



}






/* --------------------------- */
//calls all of the functions specified in my query input. After every call, the output in inserted onto my 'query' list that is printed at the end of every inputs
void run_query(char *file_name, Tree_t **root, Tree_t ** query){
  //opens the file
  FILE *fin = fopen(file_name, "r");

  //used for scanning and tokenizing
  char temp_input[200];
  int input_length;
  char *token;
  char function_call[50];
  char catagory[50];
  char state[50];
  char name[50];
  Tree_t *temp_list = NULL;

  //printf("PRINTING FUNCTION CALLS\n" );

  while(fgets(temp_input, 200, fin)){

    //string length, used to get rid of 'invisable chars'
    input_length = strlen(temp_input);

    //removes all of the non-printable characters from my scanned in input
    for(int i = 0; temp_input[input_length - i] <= '!'; i++){
      //if the end of my string has a char that has an ascii vulue less than '!' , implies that it is an unreadable char so i make it null
      temp_input[input_length - i] = '\0';
    }


    //gets the first word of the inputted string from query, this word acts as the function call
    token = strtok(temp_input, " ");
    strcpy(function_call, token);
    printf("%s ", function_call);


    //will find correct function than add all output onto a 'query' list that is printed at the end of the loop
    if(strcmp(function_call, "GetNamesByCategory") == 0){

      //tokenizes all of the expected input of the function, than passes in input into my recursive function
      token = strtok(NULL, " ");
      strcpy(catagory, token);
      printf("%s ", catagory);


      //recusrive function so search the tree for the catagories children and adds them to my query list
      GetNamesByCategory(catagory, (*root), &(*query));

    }else if(strcmp(function_call, "GetNamesByState") == 0){

      token = strtok(NULL, " ");
      strcpy(state, token);
      printf("%s ", state);

      //searches tree for node with the same name as 'state', than adds it to query list
      GetNamesByState(state,(*root),&(*query));
    }else if(strcmp(function_call, "GetNamesByCategoryAndState") == 0){

      token = strtok(NULL, " ");
      strcpy(catagory, token);

      token = strtok(NULL, " ");
      strcpy(state, token);
      printf("%s %s ", catagory, state);

      //searches tree for the correct parmeters than adds onto query list
      GetNamesByCategoryAndState(catagory, state, *root, &(*query));

    }else if(strcmp(function_call, "GetNamesWithMultipleStates") == 0){

      temp_list = NULL;

      //will add all names onto a temp_list, than adds all duplicates onto the query list
      GetNamesWithMultipleStates(*root, &(*query), &temp_list);

      freeList(&temp_list);
    }else if(strcmp(function_call, "GetNamesWithMultipleCategories") == 0){
      temp_list = NULL;

      GetNamesWithMultipleCategories(*root, &(*query), &temp_list);

      freeList(&temp_list);
    }else if(strcmp(function_call, "GetCategory") == 0){
      token = strtok(NULL, " ");
      strcpy(name, token);
      printf("%s ", name);

      //finds the name of the storm than adds the grandpanret onto my query list
      GetCategory(name, *root, &(*query));
    }else if(strcmp(function_call, "GetState") == 0){
      token = strtok(NULL, " ");
      strcpy(name, token);
      printf("%s ", name);

      //finds the name of the storm than adds the name of the panret onto my list
      GetState(name, *root, &(*query));

    }



    //in every function we add all of the found data to a query list, we just print the list than empty the list.
    if(print_list(*query) == 0){
      //if quary was empty, meaning there were no results from the function calls, we print none.
      printf("none\n");
    }else{
      printf("\n");
    }



    freeList(&(*query));
  }
  fclose(fin);
}

/* --------------------------- */
//takes the nodes from one list and puts them into my query list
void add_query(Tree_t **query, Tree_t *node){
  if(node == NULL){
    return;
  }


    insort(&(*query), node->parent, node->name);

    add_query(&(*query), node->next);


}

/* --------------------------- */
//finds all nodes with parens of the specified catagory, than adds that nodes children to my query list
void GetNamesByCategory(char catagory[50], Tree_t* list, Tree_t **query){

  if(list == NULL){
    return;
  }
  //triverse the entire tree... normally would just level 1 of the tree BUT just incase of some werid test case
  GetNamesByCategory(catagory, list->next, &(*query));
  GetNamesByCategory(catagory, list->children, &(*query));

  if((list->parent != NULL) && (strcmp(list->parent->name, catagory) == 0)){

    //adds all of the nodes from my list to my query list
    add_query(&(*query), list->children);

  }
}

/* --------------------------- */
//seaches tree for state name, than adds children of the state onto my query list
void GetNamesByState(char state[50], Tree_t* list, Tree_t **query){

  if(list == NULL){
    return;
  }
  //triverse the entire tree... normally would just level 1 of the tree BUT just incase of some werid test case
  GetNamesByState(state, list->next, &(*query));
  GetNamesByState(state, list->children, &(*query));


  //removes the number on the end of my state name within the tree
  char temp_name[50];
  strcpy(temp_name, list->name);
  temp_name[strlen(temp_name) - 1] = '\0';

  if(strcmp(temp_name, state) == 0){

    //adds all of the nodes from my list to my query list
    add_query(&(*query), list->children);

  }

}

/* --------------------------- */
//seaches list for the state, and if that states parent is the same as the catagory we put that name onto my quary list
void GetNamesByCategoryAndState(char catagory[50], char state[50], Tree_t* list, Tree_t **query){
  if(list == NULL){
    return;
  }


  GetNamesByCategoryAndState(catagory, state, list->next, &(*query));
  GetNamesByCategoryAndState(catagory, state, list->children, &(*query));

  //removes the number on the end of my state name within the tree
  char temp_name[50];
  strcpy(temp_name, list->name);
  temp_name[strlen(temp_name) - 1] = '\0';



  //checks for matching state name and matching parent/catagory name
  if((strcmp(temp_name, state) == 0) && (strcmp(list->parent->name, catagory) == 0)){
    add_query(&(*query), list->children);
  }


}

/* --------------------------- */
//searches list for the name of the storm, than adds parent to my query list
void GetState(char name[50], Tree_t* list, Tree_t **query){
  if(list == NULL){
    return;
  }

  GetState(name, list->next, &(*query));
  GetState(name, list->children, &(*query));

  //if i find my storm name, the parent is the state affected. So i add that to my query
  if(strcmp(name, list->name) == 0){
    insort(&(*query), NULL, list->parent->name);
  }
}

//searches list for the name of the storm, than adds grand-parent to my query list
void GetCategory(char name[50], Tree_t* list, Tree_t **query){
  if(list == NULL){
    return;
  }

  GetCategory(name, list->next, &(*query));
  GetCategory(name, list->children, &(*query));

  //finds the name of the storm, than adds the grandpaernt of the node, becuase that is the catagory
  if(strcmp(name, list->name) == 0){
    insort(&(*query), NULL, list->parent->parent->name);
  }
}

/* --------------------------- */
//adds all names onto a temp_list, than all duplicates of those names onto my query list
void GetNamesWithMultipleStates(Tree_t *list, Tree_t **query, Tree_t **temp_list){
  if(list == NULL){
    return;
  }

  GetNamesWithMultipleStates(list->next, &(*query), &(*temp_list));
  GetNamesWithMultipleStates(list->children, &(*query), &(*temp_list));

  //if the last character of the string is greater than a number (a letter), all of the names in the tree end with letter and not a number (IE: catagory4 vs katrina)
  if(list->name[strlen(list->name) - 1] > '9'){
    //insort returns 1 if we are adding a duplicate and 0 if we are not, so if we go to add a duplicate to my temp list, we instead add it to my quary
    if(insort(&(*temp_list), list->parent, list->name) > 0){
      //adding the name that came up twice to my quary
      insort(&(*query), list->parent, list->name);
    }
  }
}

/* --------------------------- */
//adds all names onto a temp_list, than all duplicates of those names THAT have different grandpaernts, i add them onto my query list
void GetNamesWithMultipleCategories(Tree_t *list, Tree_t **query, Tree_t **temp_list){
  if(list == NULL){
    return;
  }

  GetNamesWithMultipleCategories(list->next, &(*query), &(*temp_list));
  GetNamesWithMultipleCategories(list->children, &(*query), &(*temp_list));

  //if the last character of the string is greater than a number (a letter), all of the names in the tree end with letter and not a number (IE: catagory4 vs katrina)
  if(list->name[strlen(list->name) - 1] > '9'){
    //insort returns 2 if adding duplicates that have different grandparents onto my temp_list
    if(insort(&(*temp_list), list->parent, list->name) == 2){
      //adding the name that came up twice with different grandpaernts to my quary
      insort(&(*query), list->parent, list->name);
    }
  }
}
