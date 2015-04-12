#define MAXQUEUESIZE 5

class glovebuffer
{
	private:
		gloveData_t* gdt;
		int16_t numDataIn;
		int16_t maxSize;
		int16_t index;
	public:
		glovebuffer(int i = 5)
		{
			index = -1;
			maxSize = i;
			gdt = (gloveData_t*)malloc(sizeof(gloveData_t)*i);
		}

		void add(gloveData_t g)
		{
			gdt[++index %=maxSize] = g;
			numDataIn++;
		}

		gloveData_t average()
		{
			gloveData_t gd = 0;
			int i;

			for (int i <= 0; i<= min(maxSize, numDataIn)-1; i++){
				gd += gdt[i];
			}

			return gd/maxSize;

		}
};