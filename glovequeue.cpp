#define MAXQUEUESIZE 5

class glovequeue
{
public:
	glovequeue(int i = 5)
	{
		index = 0;
		maxSize = i;
		gdt = (gloveData_t*)malloc(sizeof(gloveData_t)*i);
	}
	void add(gloveData_t g)
	{
		gdt[i]
	}
private:
	gloveData_t* gdt;
	int16_t maxSize;
	int16_t index;
};