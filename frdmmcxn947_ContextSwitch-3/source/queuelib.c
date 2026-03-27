#include <queuelib.h>

void _initqueue(QUEUE *thequeue)
{
	thequeue->head=0;
	thequeue->tail=0;
}

void _enqueue(QUEUE *thequeue,int value)
{
	thequeue->elements[thequeue->head]=value;
	// increase the pointer
	thequeue->head++;
	thequeue->head=thequeue->head%MAXELEMENTS;
}

int _dequeue(QUEUE *thequeue)
{
	int valueret;
	valueret=thequeue->elements[thequeue->tail];
	// increase the pointer
	thequeue->tail++;
	thequeue->tail=thequeue->tail%MAXELEMENTS;
	return(valueret);
}

int _isqueueempty(QUEUE *thequeue)
{
	return(thequeue->head==thequeue->tail);
}
