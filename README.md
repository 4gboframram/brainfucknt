# Brainfuckn't
## Backstory
Brainfuckn't is an esolang created by me (4gboframram) that is similar to brainfuck, but definitely isn't.

The name came from a conversation where I was asking for name suggestions and someone suggested "not a brainfuck derivative." However, a joke language with that name already existed. So, the intellectual that I am came up with "brainfuckn't"

### Language Specification
Brainfuckn't is a tape-based esolang where there are two tapes with binary cells and resizing pointers with linked sizes (when one resizes the other does too).

When the pointers resize, they expand/contract from the right.

Bitwise operations can be applied between the contents of the 2 pointers. The result of the operation overrides the content of the first pointer.

|Operation|Effect|
|---------|------|
| `~` | Flip each bit in the content of the first pointer| 
| `<` and `>`| Move the first pointer to the left and right respectively|
| `{` and `}`| Move the second pointer to the left and right respectively|
|`^`| Apply a bitwise xor operation between the content of the two pointers and override the content of the first pointer with the result|
|`|`|Apply a bitwise or operation between the content of the two pointers and override the content of the first pointer with the result|
|`&`|Apply a bitwise and operation between the content of the two pointers and override the content of the first pointer with the result|
|`@`|Swap the first and second tapes (and their pointers)|
|`+` and `-`| Increase/decrease the size of both pointers|
|`[`instructions`]`| Repeat the instructions inside the braces until either the data the first pointer is pointing at is all zeroes or the size of the pointers are all zeroes. Do not execute the code inside the braces the first time if the conditions don't match the above (essentially a while loop) |
|`.`|Print the contents of the first pointer as an ASCII character, with the largest value bits first|
|`,`|Print the contents of the first pointer as a decimal number. Should be accurate for at least 32 bits|
|`*`|Get a single byte of input from standard input and put the value into the first pointer. If the number is too large for the pointer, then truncate the bytes so that the lowest value bytes are put into the pointer.|
|`_`|Print out both tapes with the first bits to the left and an indication of pointer size and position. Used for debug. Actual contents printed depends on implementation|
Any character that are not one of these instructions are ignored
## Examples:
Due to the nature of this language, it is hard to do much of anything, but here we go. Even adding 2 numbers requires a mess of logic gates and creating half-adders and full-adders.

### Cat (12 characters)
```
+++++++ Set size to 8
~ Invert the byte so that the loop can start
[*.] Keep printing each character in the input.
```
Or without comments
```
+++++++~[*.]
```
### Truth Machine (29 characters)
This is as short as I could make. If the input is anything but 0 (or an ascii null, but that's not typeable), it counts as a 1.

```
>>+~++++++<< 
Put ascii of 0 onto first tape

@* 
Get input character into second tape

^
Check if the characters are equal
If they are a zero is put onto the tape otherwise there will be a non zero value on the first tape

[@
If that value is not zero switch tapes

>>[-]+[,] 
Move to the first 1 in the binary of 0 and decrease the pointer size to 1 and continuously print the 1 as a digit 

]
End of loop

@. 
Otherwise switch tapes and print the 0 once
```
Or without comments
```
>>+~++++++<<@*^[@>>[-]+[,]]@.
```
### Hello World (129 characters)
I challenge you to make a shorter `Hello World!` with the exclamation point and a trailing newline. I'm not even going to bother commenting this because I don't remember what I did. I believe sub 100 characters is possible
```
++++++~@~->@^>}[~->@^>}]@++++<<.{{^<<<.@}>^<<<<<..|-----[>]~+++++<<<<<<.>>>>~<-.+<^+>>>~->.^{<|.<~<<.>>~>&<<<.&.>->>>>&<.@>>>>--.
```
## Implementation Details
- The default tape length is 30000 for the interpreter and transpiler, while the repl has a tape length of 32
### Known "Issues"
- Loops with braces the opposite direction do not get detected by the syntax checker. However, this basically only causes either a non-terminating program or something. I don't feel like fixing. It's not undefined behaviour in the language, it's just that fixing the problem would take too much effort and increase pre-interpret times.
- There is no syntax checking (matching braces) for the transpiler. Likely will not be fixed, as you can tell something went wrong when you compile it.
- Accessing bits outside of the tape results in a `segfault`. Yes, this is intentional behaviour, because I want to keep the execution time as low as possible.
- In the repl, the `*` instruction is broken. I might fix.

There are 2 implementations in this package: an interpreter written in C++ and a (somewhat) optimizing transpiler to C++, also written in C++.

### Repl 
The repl can be accessed by just running the executable with no commandline arguments. In this, the length of the tape is set to 32 so that your entire screen isn't filled with zeroes and you can test code.

#### Special Commands
There are special repl-only commands to help with development
- `exit` : Exit the repl
- `clear`: Reset the tapes back to their default (size 1, pos 0 on both tapes, and tape all zeroes)
### Commandline Interface
You can view help information about the commandline interface by using the argument `--help` or `-h`
- `[filename]+`: Interprets the files with the default interpreter (precjmp).
- `-i --interpreter [precjmp | instcnt]` :  Sets the interpreter to be that interpreter. `instcnt` counts the number of instructions executed in addition to interpeting the file. `precjmp` calculates jump positions before interpreting the code to make sure that the interpreter doesn't need to look for the position to jump to after each iteration of a loop. If not given, `precjmp` is default and recommended.
- `-len --tapelen [integer]`: Sets the tape length to `integer` be used when interpreting/transpiling. If not specified, the value is `30000`. Should be placed before `-t` if transpiling and before the filename if interpreting
- `-str --string`: Convert a string to brainfucknt with an unoptimized algorithm. Assumes there is 8 bits of zeroes to the right of both pointers. 
- `-t --transpile [file]`: Transpiles [file] to a single standalone C++ program located at the file specified by `-o`. Make sure that file is empty or deleted before running. Uses a relatively mediocre optimizer.
- `-o --output [file]` : Sets the output file for the transpiler to be `[file]`. **Should be put before `-t`**. If not specified, the ouput file is `out.cpp`
### Benchmarks
- I don't have any official tests, but I did a few on replit and the interpreter with precalculated jumps/ With the interpreter compiled with `g++ -Ofast` can achieve about 17-20 mips (million instructions per second) with many loops, and 27-31 mips with no loops. 

Conclusion: Mediocre speed. At least it beats Python, but the bar was set too low.

## Installation UwU
### Building from source
- Cmake (boost installed automatically:
   - clone repo and change to its directory
   - `cd ./build`
   - `cmake ..`
   - `cmake --build .`
   - You should see a binary named `brainfucknt` in the build folder. If you are Windows, you may need to check the `Debug` folder.
   - If you are using gcc/clang, you can uncomment
- No cmake (requires boost and not guranteed to work):
   - clone repo and change to its directory
   - `{compiler of choice} main.cpp -Ofast -o brainfucknt -I path/to/boost`


### Other Installation
- Windows x64 binaries may be provided (if possible) in the `releases` tab. They may be outdated, so check the version.
- If you can't get this to compile, you can use the online version [here](https://replit.com/@4gboframram/brainfucknt-online)

- This is my first actual C++ project. Go easy on me. :(

## QNA
- Q: Is this language Turing-Complete? It seems similar to brainfuck.
  - A: Yes. If you ignore the second tape, keep the pointer size at 1, and ignore every bitwise operation except `~`, then it is the same instruction set as a brainfuck with binary cells, which has been proven to be Turing-Complete. Since a subset of the language is Turing-Complete, that means the entire language is Turing-Complete.
- Q: Why is the transpiler only outputting to `out.cpp` even though I used `-o`?
  - A: `-o` should be placed before the `-t` because I wanted to make my own commandline parser without any libraries and this was the best way I could do it.
- Q: What is "string2bfnt.py" and why is it here?
  - A: string2bfnt.py has a very slightly better string to brainfucknt converter that uses the same algorithm as the C++ one, but is more efficient when the character can be represented with less bits. It requires you to manually change the inputted string
- Q: Why does the transpiler use `new` to create a `Tape` object instead of just calling the constructor normally? Also why don't you `delete` the objects after?
  - A: `std::swap` needs a pointer. Also I didn't use `delete` because the end of the program automatically deletes everything anyway.
- Q: Why didn't you use a commandline parsing library?
  - A: Because I didn't feel like it.
- Q: Why does `brainfucknt()` take input and output stream arguments and has a return value?
  - A: You can integrate `brainfucknt`into your own programs and get the output state. But it's kinda buggy. I will not document the api until I'm asked.
- Q: Why are you using `std::tuple` for a return value?
  - A: No idea.
- Q: *insert code choice question*
  - A: :(
- Q: Why is this qna not as fun as Senpai's?
  - A: This language is not as fun and I'm not in the mood.
- Q: *Implementation-related question*
  - A: Ask me on discord at yuwuko#0001
- Q: Hey! There's a bug! 
  - A: Hey! That's not a question! But you can make an issue and or a pull request. Or preferably, message me on discord. 

## TODO
- Speed up the interpreter even further
- Add more optimizations to the transpiler
- UwU
