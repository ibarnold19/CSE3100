#ifndef SHELLP_H
#define SHELLP_H

enum Kind {
   noCMD,exitCMD,cdCMD,pwdCMD,linkCMD,rmCMD,basicCMD,pipelineCMD
};

typedef struct Stage {
   int       _nba;
   char**   _args;
   // Feel free to add fields as needed
} Stage;

typedef struct Command {
   char*      _com;
   enum Kind _kind;
   int       _mode;
   char*    _input;
   char*   _output;
   char**    _args;
   int        _nba;
   Stage** _stages;
   int        _nbs;
   // Feel free to add fields as needed
} Command;

#define TRUE 1
#define FALSE 0


// A pipeline "object" contains 
// 1. An array of nbStages stages
// 2. A mode flag to determine whether 
//            - the pipeline input is redirected
//            - the pipeline output is redirected
// Each stage is a structure that encapsulates
// - The number of arguments
// - Each argument (in string form)
// BY DEFAULT THE MAIN PROGRAM CALLS THE PRINT METHOD OF THE COMMAND
// OBJECT. So you can see how the pipeline description is represented/stored
// Your objective:
//     * Implement the execute method to create and execute the entire pipeline. 
// Hint: all the systems calls seen in class will be useful (fork/exec/open/close/pipe/....)
// Good Luck!

#endif
