squares.c Errors

	First, #include <stdlib.h> had to be added in to allow the use of atoi, free, and malloc. Now, starting with thesum function, I deleted the arr += n. This is nonsensical as you can't add an element int to an array element. Next, thesummation in the loop should be sum += arr[i], not arr+i because once again an int cannot be added to type array. 
Finally, no return was stated, so I added return sum. 
	For the fillSquare function, I deleted the free(arr) because the memory allocated in the main will still need tobe used by the sum function, so free would erase the data of the array. Free should be used after the array is no longerbeing used. Lastly, this function also is void as it only alters an array so it returns nothing, so I deleted return arr.
	Lastly, the main function mainly had syntax errors. The uses for sum and fillSquare were wrong, so I 
changed them from only accepting the array, to accepting the n argument as well (Ex. sum(arr) into sum(n, arr)).
Next, the n = atoi function doesn't accept the proper argument. It accepts argv[2], which in the function use wouldn'tbe n. So, I changed it to argv[1], where n would be in the argument. Also, the malloc function isn't properly
used. It would only make 10 bits of memory. To fix this for n amount of space of size int, I changed the malloc
usage to malloc(sizeof(int)*n) for the array of integers. Lastly, the deallocation of the used memory by malloc
has to be freed, but it is also used wrong. I changed free(argv) to free(arr).

strings.c

	In terms of code errors, there weren't really many. The function defined doesn't return anything, it just prints out when !(i%1000) and the corresponding string that was copied. So, there is no need for a return type forthe function, so I just made the function void. Next, the malloc uses weren't wrong either, I just want to define
them in a more precise way, so these aren't wrong, but I just like them better this way. The errors in this 
program come from the massive memory leaks. Without changing anything in the program, the program has 20,000 
allocs that take up a total of 41.1 million bytes of memory and these are never freed. This is a huge waste. The
malloc in the main is the bigger memory leak issue, as it leaks the majority of the memory due to the for loop. 
It leaks a total or approx. 40.96 million bytes of memory, so by adding free(ptr), where ptr is the array being
allocated, as the last intruction each time in the for loop, 40.96 bytes are saved at the exit. Now, the other 140thousand are due to the malloc in the copying function. To fix this, simply add free(s) at the very end of the 
function after the if statement, and that will save 140,000 bytes of memory. Now, there are 20,000 frees for the
20,000 allocs and no bytes used at the exit, so the program is fixed. 
Not sure what invalid read size comes from ;/
