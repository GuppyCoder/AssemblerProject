#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <map>
bool A_COMMAND = false;
bool C_COMMAND = true;
bool L_COMMAND;

void assemblyToMachineC_Instruction(const std::string& comp, std::string& c_instruction);
int main() {
    std::map<std::string, std::string> symbolTable;
    std::ifstream assembly("AssemblyCode/Rect.asm");  // Read from assembly instruction
    std::ofstream assemblyAndHack{"JustAssembly.txt"};   // Write machine language instruction
    std::ofstream hack("Hack.hack");

    // check to see if function succeded in opening file
    if(assembly.is_open()){
        std::string assemblyInstruction;
        int lineNumber = 0;
        while(std::getline(assembly, assemblyInstruction)) {
            assemblyInstruction.erase(remove_if(assemblyInstruction.begin(), assemblyInstruction.end(), isspace), assemblyInstruction.end());
            /** std::unique will check two chars at a time, the function checks to see if both equal a new line.
             *  if they both equal a new line then remove one of the newline characters. Continue doing this until we
             *  reach the end iterator of the pass2. */
             if(assemblyInstruction.empty())
                 continue;
            if (assemblyInstruction.compare("\r") == 0)
                continue;
            // check if the pass2 is a comment.
            if (assemblyInstruction.substr(0, 2) == "//")
                continue;
            size_t posOfComment = assemblyInstruction.find("/");
            if(posOfComment != std::string::npos)
                assemblyInstruction.erase(posOfComment);

            auto begin = assemblyInstruction.find_first_of("(");
            auto end = assemblyInstruction.find_last_of(")");

            if (std::string::npos!=begin && std::string::npos!=end && begin <= end) {
                assemblyInstruction.erase( std::remove_if(assemblyInstruction.begin(), assemblyInstruction.end(),
                                        [](char ch){ return ch=='(' || ch ==')'; }),assemblyInstruction.end() );
                symbolTable.insert({assemblyInstruction, std::to_string(lineNumber)});    // inserting labels into table.
                continue;
            }
            // From here on out the instructions are just assembly.
            assemblyAndHack << assemblyInstruction << std::endl;    // and labels
            ++lineNumber;
        }
        assemblyAndHack.close();
        std::ifstream assemblyCodeOnly("JustAssembly.txt");
            // check to see the instruction type
            std::string pass2;
            int variableRegister = 16;
            if(assemblyCodeOnly.is_open()) {
                while (std::getline(assemblyCodeOnly, pass2)) {
                    switch (pass2[0]) {
                        case '@':
                            A_COMMAND = true;
                            break;
                        default:
                            A_COMMAND = false;
                            break;
                    }
                    // A_INSTRUCTION translation
                    if (A_COMMAND) {
                        pass2.erase(0, 1);
                        // check for pre-defined labels
                        if (true) {
                            if (!pass2.compare("SP") || !pass2.compare("R0")) {
                                pass2.assign("0");
                            } else if (!pass2.compare("LCL") || !pass2.compare("R1")) {
                                pass2.assign("1");
                            } else if (!pass2.compare("ARG") || !pass2.compare("R2")) {
                                pass2.assign("2");
                            } else if (!pass2.compare("THIS") || !pass2.compare("R3")) {
                                pass2.assign("3");
                            } else if (!pass2.compare("THAT") || !pass2.compare("R4")) {
                                pass2.assign("4");
                            } else if (!pass2.compare("R5")) {
                                pass2.assign("5");
                            } else if (!pass2.compare("R6")) {
                                pass2.assign("6");
                            } else if (!pass2.compare("R7")) {
                                pass2.assign("7");
                            } else if (!pass2.compare("R8")) {
                                pass2.assign("8");
                            } else if (!pass2.compare("R9")) {
                                pass2.assign("9");
                            } else if (!pass2.compare("R10")) {
                                pass2.assign("10");
                            } else if (!pass2.compare("R11")) {
                                pass2.assign("11");
                            } else if (!pass2.compare("R12")) {
                                pass2.assign("12");
                            } else if (!pass2.compare("R13")) {
                                pass2.assign("13");
                            } else if (!pass2.compare("R14")) {
                                pass2.assign("14");
                            } else if (!pass2.compare("R15")) {
                                pass2.assign("15");
                            } else if (!pass2.compare("SCREEN")) {
                                pass2.assign("16384");
                            } else if (!pass2.compare("KBD")) {
                                pass2.assign("24576");
                            }
                        }   // I think it works. Pretty sure works.
                        auto it = symbolTable.find(pass2);
                        // for labels and for variables.
                        if(it != symbolTable.end()){
                            pass2 = (*it).second;   // equal the value stored at the key?
                        }else if (isalpha(pass2[0])) {
                            symbolTable.insert({pass2, std::to_string(variableRegister++)});
                            pass2 = symbolTable.find(pass2)->second;
                        }
                        std::stringstream A_Instruction(pass2); // store string in stringstream object?
                        int address{};
                        A_Instruction >> address;   // the instruction has been converted to an int.

                        std::string a_instruction;
                        int i = 0;
                        while (address > 0) {
                            a_instruction.insert(i, std::to_string(address % 2));
                            address = address / 2;
                            i++;
                        }
                        std::reverse(a_instruction.begin(), a_instruction.end());
                        while (a_instruction.size() < 16) {
                            a_instruction.insert(0, "0");
                        }   // initialize a_instruction with 0.
                        // assemblyAndHack << a_instruction << std::endl;
                        hack << a_instruction << std::endl;
                    }
                    // C_INSTRUCTION translation
                    if (!A_COMMAND) {
                        std::string c_instruction(16, '0');   // c_instruction is size 16
                        // first three bits of c_instruction are always 1.
                        for (int i = 0; i < 3; i++)
                            c_instruction[i] = '1';
                        std::string::size_type posDest = pass2.find('=');
                        /* Destination Logic */
                        if (posDest != std::string::npos) {
                            // character found
                            std::string destStr = pass2.substr(0, posDest);
                            // getchar(); //for debug purposes
                            // Now we have the destination we can do another switch case?
                            if (!destStr.compare("M")) {
                                c_instruction[10] = '0';
                                c_instruction[11] = '0';
                                c_instruction[12] = '1';
                            } else if (!destStr.compare("D")) {
                                c_instruction[10] = '0';
                                c_instruction[11] = '1';
                                c_instruction[12] = '0';
                            } else if (!destStr.compare("MD")) {
                                c_instruction[10] = '0';
                                c_instruction[11] = '1';
                                c_instruction[12] = '1';
                            } else if (!destStr.compare("A")) {
                                c_instruction[10] = '1';
                                c_instruction[11] = '0';
                                c_instruction[12] = '0';
                            } else if (!destStr.compare("AM")) {
                                c_instruction[10] = '1';
                                c_instruction[11] = '0';
                                c_instruction[12] = '1';
                            } else if (!destStr.compare("AD")) {
                                c_instruction[10] = '1';
                                c_instruction[11] = '1';
                                c_instruction[12] = '0';
                            } else {
                                c_instruction[10] = '1';
                                c_instruction[11] = '1';
                                c_instruction[12] = '1';
                            }
                        } else {
                            c_instruction[10] = '0';
                            c_instruction[11] = '0';
                            c_instruction[12] = '0';
                        }
                        /*                   */

                        /* Jump Logic */
                        std::string::size_type posJump = pass2.find(';');
                        if (posJump != std::string::npos) // if character found
                        {
                            std::string jumpStr = pass2.substr(posJump + 1);
                            jumpStr.erase(std::remove(jumpStr.begin(), jumpStr.end(), '\r'), jumpStr.end());
                            // getchar(); //for debug purposes
                            // Now we have the destination we can do another switch case?
                            if (!jumpStr.compare("JGT")) {
                                c_instruction[13] = '0';
                                c_instruction[14] = '0';
                                c_instruction[15] = '1';
                            } else if (!jumpStr.compare("JEQ")) {
                                c_instruction[13] = '0';
                                c_instruction[14] = '1';
                                c_instruction[15] = '0';
                            } else if (!jumpStr.compare("JGE")) {
                                c_instruction[13] = '0';
                                c_instruction[14] = '1';
                                c_instruction[15] = '1';
                            } else if (!jumpStr.compare("JLT")) {
                                c_instruction[13] = '1';
                                c_instruction[14] = '0';
                                c_instruction[15] = '0';
                            } else if (!jumpStr.compare("JNE")) {
                                c_instruction[13] = '1';
                                c_instruction[14] = '0';
                                c_instruction[15] = '1';
                            } else if (!jumpStr.compare("JLE")) {
                                c_instruction[13] = '1';
                                c_instruction[14] = '1';
                                c_instruction[15] = '0';
                            } else if (!jumpStr.compare("JMP")) {
                                c_instruction[13] = '1';
                                c_instruction[14] = '1';
                                c_instruction[15] = '1';
                            }
                        } else {
                            c_instruction[13] = '0';
                            c_instruction[14] = '0';
                            c_instruction[15] = '0';
                        }
                        /*            */

                        // now we need to do the comparison logic oooooo weee
                        std::string comp{};
                        if (posDest != std::string::npos)
                            comp = pass2.substr(posDest + 1);
                        else if (posJump != std::string::npos)
                            comp = pass2.substr(0, posJump);
                        comp.erase(std::remove(comp.begin(), comp.end(), '\r'), comp.end());
                        assemblyToMachineC_Instruction(comp, c_instruction);
                        assemblyAndHack << c_instruction << std::endl;
                        hack << c_instruction << std::endl;
                    }
                };
            }
        assembly.close();
        assemblyAndHack.close();
        hack.close();
    }
}


void assemblyToMachineC_Instruction(const std::string& comp, std::string& c_instruction)
{
    if(!comp.compare("0"))
    {
        c_instruction.replace(4, 6, "101010");
    }else if(!comp.compare("1")){
        c_instruction.replace(4, 6, "111111");
    }else if(!comp.compare("-1")){
        c_instruction.replace(4, 6,"111010");
    }else if(!comp.compare("D")){
        c_instruction.replace(4, 6, "001100");
    }else if(!comp.compare("A") || !comp.compare("M")){
        c_instruction.replace(4, 6, "110000");
    }else if(!comp.compare("!D")){
        c_instruction.replace(4, 6, "001101");
    }else if(!comp.compare("!A") || !comp.compare("!M")){
        c_instruction.replace(4, 6, "110001");
    }else if(!comp.compare("-D")){
        c_instruction.replace(4, 6, "001111");
    }else if(!comp.compare("-A") || !comp.compare("-M")){
        c_instruction.replace(4, 6, "110011");
    }else if(!comp.compare("D+1")){
        c_instruction.replace(4, 6, "011111");
    }else if(!comp.compare("A+1") || !comp.compare("M+1")){
        c_instruction.replace(4, 6, "110111");
    }else if(!comp.compare("D-1")){
        c_instruction.replace(4, 6, "001110");
    }else if(!comp.compare("A-1") || !comp.compare("M-1")){
        c_instruction.replace(4, 6, "110010");
    }else if(!comp.compare("D+A") || !comp.compare("D+M")){
        c_instruction.replace(4, 6, "000010");
    }else if(!comp.compare("D-A") || !comp.compare("D-M")){
        c_instruction.replace(4, 6, "010011");
    }else if(!comp.compare("A-D") || !comp.compare("M-D")){
        c_instruction.replace(4, 6, "000111");
    }else if(!comp.compare("D&A") || !comp.compare("D&M")){
        c_instruction.replace(4, 6, "000000");
    }else if(!comp.compare("D|A") || !comp.compare("D|M")){
        c_instruction.replace(4, 6, "010101");
    }

    if(comp.find("M") != std::string::npos)
        c_instruction[3] = '1';
    else
        c_instruction[3] = '0';
}

