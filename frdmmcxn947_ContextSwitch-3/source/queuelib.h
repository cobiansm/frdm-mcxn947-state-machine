#define MAXELEMENTS 10

typedef struct QUEUE {
	int elements[MAXELEMENTS];
	int head;
	int tail;
} QUEUE;

void _initqueue(QUEUE *thequeue);
void _enqueue(QUEUE *thequeue,int value);
int _dequeue(QUEUE *thequeue);
int _isqueueempty(QUEUE *thequeue);
