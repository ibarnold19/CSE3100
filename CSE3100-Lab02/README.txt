factorial.c
The errors that I found in this file were in regards to what was contained inside the factorial function defined. First, even though it doesn't technically
break the program, there is no prototype definition for the factorial function,
only a implementation. This allows anyone to access the function from anywhere,
so to get into the habit of prototyping each function in the future, I added
one in for the factorial funciton.
Next, the contents of the factorial function were completely wrong. First, therewas another variable for result defined in the function. The result variable
only needs to be looked at after the result of the function is found, which
is already done correctly in the main function. So, I deleted the int result
declaration in the function. Next, since there is no more result variable in thefunction and it made no sense in the first place, I edited the if(n ==0) line 
to if(n == 0) return 1;, since a base case is required and 1 is the base case
for this function. Lastly, the second line never returned anything so the
recurrsion wouldn't work properly and end in a core dump or just 0 if result
were still defined in the function. return factorial(n - 1) * n; will reduce therecurrsion correctly, n-1 was n+1 so I changed that as well, as well as give thecorrect computation for a factorial function.

sumItFor.c
The errors in this program were much simpler. First, the scanf() was incorrect
as the destination pointer was i when it should be n, and there was no & either which would give an error. scanf("n: ", &n); is correct. Next, the headers for
the for loops were incorrect. Each one needs to go up to 2*n or n, so it should be <= not just <. Next, each i was set to i or 2*i when it should be i = n or 2*n. Lastly, the printf for the sum was also incorrectly formatted, it shouldn't
be + sum. Instead, it should be printf("sum: ", sum);

quad.c
There were quite a few errors in this program. First, no prototype declaration, so I inserted one just for good practice. Next, in the x_to_y function, the 
recurrsion is incorrect. It depends on y reaching 0 to hit the base, but never
decrements it in the recurrsive call below the base case. So, I changed the
second recursive call to x-1 instead of y-1. Also, I changed the base case to
y == 1, because if y == 0, you would get x cubed instead of squared, since threecalls would be made. Next, one small thing that doesn't really break anything, Iadded L behind 4.0L for the discriminant, as defining a double requires that. 
	Now, the D > 0 is wrong, since that checks if the answer is imaginary or not, so it needs to be changed to D < 0. Next, since displaying/calculating the
imaginary number requires dividing -b and the discriminant seperately to be 
displayed. So I added a chunk of code that does this. The first line is 
z1 = -b / (2*a), to display -b/(2*a) portion. Next, I changed the discriminant
to positive, just to find the square root in the next line, then divided that 
by 2*a and put it into z2. This allows me to show the imaginary portion and the
normal portion once printed. I also made the calculations orinigally in the code
into an else statement, since different calculations are done for positive
numbers. 
	For the original calculations in the code, I didn't like how they were
done in the print statements, so I just did them in the calculations. z1 was foradding -b and the discriminant, and z2 was for -b minus the discriminant. This
just allowed me to plug these values into the print lines and avoid calculationsduring the printf statements. Lastly, in the printf statements, %d is the wrong
way to print out doubles, so I replaced them all with %lf. And to keep things 
more simple in the printf, I kept the same output for imaginary numbers, but
the real numbers I got rid of the operations in printf and put z1 for addition
and z2 for the subtraction portion of the quadratic equation. This made the codecleaner and avoided operations at printf. 

