/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
	return ~(x&y)&~(~x&~y);
}

/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
	return 1 << 31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  //return !(~(x + x + 1));
	int i = x + 1;//Tmin = 0b1000 0000
	x = ~(x + i);//0
	i = !i;//exclude x=0xffff...
	x = x + i;
	return !x;
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
	int mask = 0xAA + (0xAA << 8);
	mask = mask + (mask << 16);
	return !((mask & x) ^ mask);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
	int high_byte_validation = !!((x >> 4) ^ 0x3);
	int low_byte = x & 0xf;
	int lb_vali1 = !!(low_byte >> 3);
	int lb_vali2 = !!(low_byte ^ 0x8);
	int lb_vali3 = !!(low_byte ^ 0x9);
	return !(high_byte_validation + (lb_vali1 & lb_vali2 & lb_vali3));
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
	x = !!x;
	x = ~x + 1;
	return (x & y) | (~x & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  // Get the sign bit of x
  // 1 = Negative
  // 0 = Non-Negative
  int x_sign_bit = x >> 31;

  //Get the sign bit of y
  // 1 = Negative
  // 0 = Non-Negative
  int y_sign_bit = y >> 31;

  // True if signs are the same and (x-y <= 0)
  // x-y <= 0 ----> y-x >= 0 :This conversion makes it easier to determine the sign bit
  // If signs are the same: x_sign_bit ^ y_sign_bit == 0
  // If y-x >= 0 : sign_bit_of(y + (~x + 1)) == 0
  int same_sign = !(x_sign_bit ^ y_sign_bit) & !((y + (~x + 1)) >> 31);

  // True if the signs are not the same and x<0&&y>0
  int diff_sign = x_sign_bit & !y_sign_bit;

  // Only one of same_sign and diff_sign is true to return true
  return same_sign | diff_sign;
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
	/*problem:
			x == 0 return 1
			x != 0 return 0

		base case:
			If x==0, then the sign bits of x and -x are both 0.
			If x!=0, then the sign bits of x and -x are different
	*/
	int neg_x = ~x + 1;

	//0 = x and neg_x are both 0
	//1 = x is not 0
	int sign_flag = ((neg_x | x) >> 31) & 0x1;

	//sign_flag == 0, return -1 + 2 = 1
	//sign_flag == 1, return -2 + 2 = 0
	return (~sign_flag + 2);

}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
	/*
		If x is a positive number, then the index where the highest bit is 1 is added by 1.
		Example: 
			0x37 --> [0011 0111]
			The index where highest bit is 1 is 6,and the minium number of bits is 6 + 1;

		If x is a negative number, then the minimum number of bits to represent the number 
		is the index of the highest bit 0 plus one,derived from the sign bit
		Example:
			0xC7 --> [1100 0111]
			The index where highest bit is 0 is 6,and the minium number of bits is 6 + 1;

		Observe the NOT operation on negative numbers:
			~(0xC7) = ~[1100 0111] = [0011 1000]
										^             ^
		Result:After the NOT operation of negative numbers,
						the highest bit of the positive number is obtained as 1, 
						which is equivalent to obtaining the highest bit of the negative number as 0.
	*/
	// sign_mask:
	//	[1111 .. 1111]: x is negative
	//	[0000 .. 0000]: x is positive
	int sign_mask = x >> 31;
	
	// x : (x & [11..11]) | (~x & 0) = x | 0 = x
	//~x : (x & 0 ) | (~x & [11..11]) = 0 | ~x = ~x
	x = (x & (~sign_mask)) | ((~x) & sign_mask);

	//Get the highest bit of 1
	int bit_16 = (!!(x >> 16)) << 4;
	x = x >> bit_16;

	int bit_8 = (!!(x >> 8)) << 3;
	x = x >> bit_8;

	int bit_4 = !!(x >> 4) << 2;
	x = x >> bit_4;

	int bit_2 = !!(x >> 2) << 1;
	x = x >> bit_2;

	int bit_1 = !!(x >> 1);
	x = x >> bit_1;

	int bit_0 = x;
	return bit_16 + bit_8 + bit_4 + bit_2 + bit_1 + bit_0 + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
	unsigned    s = uf >> 31;
	unsigned  exp = uf >> 23 & 0xFF;
	unsigned frac = uf & 0x7FFFFF;

	//Denormalized:f = (-1)^S * 2^E * M (hint: (-1)^S and 2^E are both constant) 
	// f = M * CONSTANT
	//2f = 2M * CONSTANT = M << 1 * CONSTANT
	if(exp == 0){
		frac = frac << 1;
	}
	//Infinity:return original value
	else if(exp == 0xFF){
		return uf;
	}
	//Normalized:f = (-1)^S * 2^E * M
	//          2f = (-1)^S * 2^(E+1) * M
	else{
		exp = exp + 1;
	}
	return (s<<31) | (exp<< 23) | frac;

}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
	unsigned s = uf >> 31;
	unsigned exp = uf >> 23 & 0xFF;
	unsigned frac = uf & 0x7FFFFF;

	//E = exp - Bias
	int E = exp - 127;
	frac = frac | (1 << 23);

	if(E < 0) return 0;
	else if(E >= 31) return 0x1 << 31;
	else {
		if(E<23){
			frac = frac >> (23 - E);
		}else{
			frac = frac << (E - 23);
		}
	}
	if (s)
		return ~frac + 1;
	return frac;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x){
	if(x < -149)
		return 0;
	else if(x < -126)
		return 1 << (x + 149);
	else if(x <= 127)
		return (x + 127) << 23;
	else 
		return (0xFF) << 23;
}

