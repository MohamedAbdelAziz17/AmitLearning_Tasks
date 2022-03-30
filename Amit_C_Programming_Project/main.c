#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>





/***Database Structure***/
typedef struct
{
    uint8_t     Student_ID;
    uint8_t     Student_Year;

    uint8_t     Course_1_ID;
    uint8_t     Course_1_Grade;

    uint8_t     Course_2_ID;
    uint8_t     Course_2_Grade;

    uint8_t     Course_3_ID;
    uint8_t     Course_3_Grade;

}SimpleDb;


/*******************Doubly Linked List Code*******************/
struct node
{
    SimpleDb data_entry;
    struct node *next;
    struct node *prev;
};

typedef struct node node_t;

node_t* head = NULL;

node_t* Node_Create()
{
    node_t* ptr_to_newnode = (node_t*) malloc( sizeof( node_t ) );

    ptr_to_newnode->next = NULL;
    ptr_to_newnode->prev = NULL;

    return ptr_to_newnode ;
}

node_t* Node_Push()
{
    node_t* ptr_to_newnode = Node_Create();

    if(head == NULL)
    {
        head = ptr_to_newnode;
        return ptr_to_newnode;
    }

    head->prev = ptr_to_newnode;
    ptr_to_newnode->next = head;
    head = ptr_to_newnode;

    return ptr_to_newnode;

}

node_t* Node_Search( uint8_t id )
{
    node_t* temporary = head;

    while(temporary != NULL)
    {
        if(temporary->data_entry.Student_ID == id)
        {

            return temporary;
        }

        temporary = temporary->next;
    }

    return NULL;
}
/********************************************************/

/**Database Functions**/
uint8_t SDB_GetUsedSize(void)
{
    uint8_t nodes_ctr = 0;

    node_t* tmp = head;

    while(tmp != NULL)
    {
        nodes_ctr++;

        tmp = tmp->next;
    }

    return nodes_ctr;
}

bool SDB_IsFull(void)
{
    uint8_t nodes_ctr = SDB_GetUsedSize();

    if(nodes_ctr >= 10)
        return true;    /*full*/
    else
        return false;   /*not full*/
}

void SDB_DeleteEntry(uint8_t id)
{

    node_t* temp = head;

    node_t* lastNodeAddr = head;

    while(lastNodeAddr != NULL)
    {
       lastNodeAddr = lastNodeAddr->next;
    }

	while(temp != NULL)
	{

		if(temp->data_entry.Student_ID == id)
		{
			if(temp->prev == NULL)
			{
				head = temp->next;
			}
			else
			{
				temp->prev->next = temp->next;
			}
			if(temp == lastNodeAddr)
			{
				lastNodeAddr = temp->prev;
			}
			node_t* temporaryHolder = temp;

			temp = temp->next;

			free(temporaryHolder);
		}
		else
		{
			temp = temp->next;
		}
	}

}

bool SDB_IsIdExist ( uint8_t id )
{
    node_t* tmp = head;

    while(tmp != NULL)
    {
        if(tmp->data_entry.Student_ID == id)
        {
            return true;
        }
        tmp = tmp->next;
    }
    return false;
}

bool SDB_AddEntry(uint8_t id , uint8_t year, uint8_t* subjects, uint8_t* grades)
{
    bool ENTRY_ADDED = true;

    /*check if there is a place in the linked list or not*/
    if(SDB_IsFull() == true)
    {
        ENTRY_ADDED = false;
        return ENTRY_ADDED;
    }


    node_t* ptr_to_node = Node_Push();         /*insert new node at head to hold this entry*/

    /** Input Data **/
    ptr_to_node->data_entry.Student_ID          = id;
    ptr_to_node->data_entry.Student_Year        = year;


    /* subject 1 */
    ptr_to_node->data_entry.Course_1_ID         = *(subjects + 0);


    if(*(grades+0) < 0 && *(grades+0) > 100)
    {
        return ENTRY_ADDED;

        SDB_DeleteEntry(id);

        ENTRY_ADDED = false;
        return ENTRY_ADDED;
    }
    else
    {
        ptr_to_node->data_entry.Course_1_Grade      = *(grades + 0);
    }

    /* subject 2 */
    ptr_to_node->data_entry.Course_2_ID         = *(subjects + 1);

    if(*(grades+1) < 0 && *(grades+1) > 100)
    {
        return ENTRY_ADDED;

        SDB_DeleteEntry(id);

        ENTRY_ADDED = false;
        return ENTRY_ADDED;
    }
    else
    {
        ptr_to_node->data_entry.Course_2_Grade      = *(grades + 1);
    }

    /* subject 3 */
    ptr_to_node->data_entry.Course_3_ID         = *(subjects + 2);

    if(*(grades+2) < 0 && *(grades+2) > 100)
    {
        return ENTRY_ADDED;

        SDB_DeleteEntry(id);

        ENTRY_ADDED = false;
        return ENTRY_ADDED;
    }
    else
    {
       ptr_to_node->data_entry.Course_3_Grade      = *(grades + 2);
    }


    return ENTRY_ADDED;

}

bool SDB_ReadEntry(uint8_t id, uint8_t* year , uint8_t* subjects,uint8_t* grades )
{
    bool isExist = SDB_IsIdExist(id);

    node_t* tmp = Node_Search( id );

    if(!isExist)
    {
       return false;
    }

    *year = tmp->data_entry.Student_Year;

    if(subjects != NULL)
    {
        *(subjects+0) = tmp->data_entry.Course_1_ID;
        *(subjects+1) = tmp->data_entry.Course_2_ID;
        *(subjects+2) = tmp->data_entry.Course_3_ID;
    }

    if(grades != NULL)
    {
        *(grades+0) = tmp->data_entry.Course_1_Grade;
        *(grades+1) = tmp->data_entry.Course_2_Grade;
        *(grades+2) = tmp->data_entry.Course_3_Grade;
    }


    return true;
}

void SDB_GetIdList(uint8_t* count , uint8_t* list)
{
    *count = SDB_GetUsedSize();

    node_t* tmp = head;

    uint8_t i = 0;

    for( i = 0; i < (*count) ; i++ )
    {
        list[i] =  tmp->data_entry.Student_ID;

        tmp = tmp->next;
    }

}

int main()
{

    uint8_t subjects[3];        /*for each student*/

    uint8_t grades[3];          /*for each student*/

    uint8_t count = 0;          /*number of entries*/

    uint8_t list[10];           /*list of id's*/

    SDB_AddEntry(52, 22, subjects, grades);
    SDB_AddEntry(14, 17, subjects, grades);
    SDB_AddEntry(35, 14, subjects, grades);
    SDB_AddEntry(75, 13, subjects, grades);
    SDB_AddEntry(59, 20, subjects, grades);
    SDB_AddEntry(45, 99, subjects, grades);
    SDB_AddEntry(21, 20, subjects, grades);
    SDB_AddEntry(38, 20, subjects, grades);

    SDB_GetIdList( &count , list );

    for(int i = 0 ; i < count ; i++)
    {
        printf("ID %d:\t %d\n", i+1 , list[i]);
    }

    printf("Size of database: %d\n" , SDB_GetUsedSize());

    printf("Is ID: %d exist?: %s", 52, SDB_IsIdExist(52)? "true exist":"false not exist");









    return 0;
}








