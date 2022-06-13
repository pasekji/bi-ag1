#ifndef __PROGTEST__
#include "flib.h"
#endif //__PROGTEST__

int32_t buffer[1];
int32_t buffer2[1];
int arrayLength;
int size1;
int size2;
int size3;

void swap(int32_t& a, int32_t& b)
{
	int32_t tmp = a;
	a = b;
	b = tmp;
}

void heapify(int32_t* heap, int n, int i)
{
	int largest = i;
	while (true)
	{
		int left = 2 * i + 1;
		int right = 2 * i + 2;

		if (left < n && heap[left] > heap[largest])
			largest = left;

		if (right < n && heap[right] > heap[largest])
			largest = right;

		if (largest != i)
		{
			swap(heap[i], heap[largest]);
			i = largest;
		}
		else break;
	}
}

void sort(int32_t* heap, int n)
{
	for (int i = n / 2 - 1; i >= 0; i--)
	{
		heapify(heap, n, i);
	}
	for (int i = n - 1; i > 0; i--)
	{
		swap(heap[0], heap[i]);
		heapify(heap, i, 0);
	}
}

int32_t* heap;
int32_t* heap2;
int32_t* outBuffer;

void write(int32_t value, int32_t out_file, int32_t* buffer, int buffSize, int& pos)
{
	buffer[pos++] = value;
	if (pos == buffSize)
	{
		flib_write(out_file, buffer, pos);
		pos = 0;
	}
}

void closeAfterWriting(int32_t out_file, int32_t* buffer, int& pos)
{
	if (pos > 0)
		flib_write(out_file, buffer, pos);
	pos = 0;
	flib_close(out_file);
}

void openForReading(int32_t in_file, int buffSize, int& pos)
{
	flib_open(in_file, READ);
	pos = buffSize;
}

void openForWriting(int32_t in_file, int buffSize, int& pos)
{
	flib_open(in_file, WRITE);
	pos = 0;
}

bool read(int32_t in_file, int32_t* buffer, int buffSize, int& pos, int& nRead, int32_t* result)
{
	if (pos == buffSize)
	{
		nRead = flib_read(in_file, buffer, buffSize);
		pos = 0;
	}
	if (pos == nRead) return false;
	*result = buffer[pos++];
	return true;
}

int rozdelNaSoubory(int32_t in_file, int32_t arrayLength)
{
	int count = 0;
	flib_open(in_file, READ);
	//int32_t* heap = new int32_t[arrayLength];
	int heapSize = flib_read(in_file, heap, arrayLength);
	while (heapSize > 0)
	{
		sort(heap, heapSize);
		int out_file = count + 2;
		count++;
		flib_open(out_file, WRITE);
		flib_write(out_file, heap, heapSize);
		flib_close(out_file);
		heapSize = flib_read(in_file, heap, arrayLength);
	}
	flib_close(in_file);
	return count;
}

void merge(int32_t in_file1, int32_t in_file2, int32_t out_file)
{
	int pos1, pos2, posOut;
	int nRead1, nRead2;
	int32_t value1, value2;

	openForReading(in_file1, size1, pos1);
	openForReading(in_file2, size2, pos2);
	openForWriting(out_file, size3, posOut);
	
	bool ok1 = read(in_file1, heap, size1, pos1, nRead1, &value1);
	bool ok2 = read(in_file2, heap2, size2, pos2, nRead2, &value2);

	while (ok1 && ok2)
	{
		if (value1 < value2)
		{
			write(value1, out_file, outBuffer, size3, posOut);
			ok1 = read(in_file1, heap, size1, pos1, nRead1, &value1);
		}
		else
		{
			write(value2, out_file, outBuffer, size3, posOut);
			ok2 = read(in_file2, heap2, size2, pos2, nRead2, &value2);
		}
	}
	while (ok1)
	{
		write(value1, out_file, outBuffer, size3, posOut);
		ok1 = read(in_file1, heap, size1, pos1, nRead1, &value1);
	}
	while (ok2)
	{
		write(value2, out_file, outBuffer, size3, posOut);
		ok2 = read(in_file2, heap2, size2, pos2, nRead2, &value2);
	}
	closeAfterWriting(out_file, outBuffer, posOut);
	flib_close(in_file2);
	flib_close(in_file1);
}

void copy_file(int32_t in_file, int32_t out_file)
{
	flib_open(in_file, READ);
	flib_open(out_file, WRITE);
	int nRead = flib_read(in_file, heap, arrayLength);
	while (nRead > 0)
	{
		flib_write(out_file, heap, nRead);
		nRead = flib_read(in_file, heap, arrayLength);
	}
	flib_close(in_file);
	flib_close(out_file);
}

void tarant_allegra ( int32_t in_file, int32_t out_file, int32_t bytes ){
	arrayLength = bytes / 4 - 50;
	heap = new int32_t[arrayLength];
	size1 = arrayLength / 3;
	size2 = (arrayLength - size1) / 2;
	size3 = arrayLength - size1 - size2;
	heap2 = heap + size1;
	outBuffer = heap + size1 + size2;
	int pocetSouboru = rozdelNaSoubory(in_file, arrayLength) + 2;
	int prvniSoubor = 2;
	while (prvniSoubor + 1 < pocetSouboru)
	{
		merge(prvniSoubor, prvniSoubor + 1, pocetSouboru);
		flib_remove(prvniSoubor);
		flib_remove(prvniSoubor + 1);
		pocetSouboru++;
		prvniSoubor += 2;
	}
	copy_file(prvniSoubor, out_file);
	flib_remove(prvniSoubor);
	delete[] heap;
}


#ifndef __PROGTEST__

uint64_t total_sum_mod;
void create_random(int output, int size){
    total_sum_mod=0;
    flib_open(output, WRITE);
    /* srand(time(NULL)); */
#define STEP 100ll
    int val[STEP];
    for(int i=0; i<size; i+=STEP){
        for(int j=0; j<STEP && i+j < size; ++j){
            val[j]=-1000 + (rand()%(2*1000+1));
            total_sum_mod += val[j];
        }
        flib_write(output, val, (STEP < size-i) ? STEP : size-i);
    }
    flib_close(output);
}

void tarant_allegra ( int in_file, int out_file, int bytes );

void check_result ( int out_file, int SIZE ){
    flib_open(out_file, READ);
    int q[30], loaded, last=-(1<<30), total=0;
    uint64_t current_sum_mod=0;
    while(loaded = flib_read(out_file, q, 30), loaded != 0){
        total += loaded;
        for(int i=0; i<loaded; ++i){
            if(last > q[i]){
                printf("the result file contains numbers %d and %d on position %d in the wrong order!\n", last, q[i], i-1);
                exit(1);
            }
            last=q[i];
            current_sum_mod += q[i];
        }
    }
    if(total != SIZE){
        printf("the output contains %d but the input had %d numbers\n", total, SIZE); exit(1);
    }
    if(current_sum_mod != total_sum_mod){
        printf("the output numbers are not the same as the input numbers\n");
        exit(1);
    }
    flib_close(out_file);
}

int main(int argc, char **argv){
    const uint16_t MAX_FILES = 65535;
    flib_init_files(MAX_FILES);
    int INPUT = 0;
    int RESULT = 1;
    int SIZE = 140;

    create_random(INPUT, SIZE);
    tarant_allegra(INPUT, RESULT, 1000);
    check_result(RESULT, SIZE);

    flib_free_files();
    return 0;
}
#endif //__PROGTEST__
