#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <cmath>

class BasicInterpreter {
private:
    std::map<int, std::string> program;
    std::map<std::string, int> variables;
    std::map<std::string, std::vector<int>> arrays;
    std::map<int, int> lineNumbers;
    std::vector<int> executionOrder;
    int currentLineIndex;
    bool running;

    enum class TokenType {
        NUMBER, STRING, IDENTIFIER, OPERATOR, KEYWORD, EOL, UNKNOWN
    };

    struct Token {
        TokenType type;
        std::string value;
        int lineNumber;
    };

public:
    BasicInterpreter() : currentLineIndex(0), running(false) {}

    void loadProgram(const std::vector<std::string>& lines) {
        program.clear();
        lineNumbers.clear();
        executionOrder.clear();
        
        for (const auto& line : lines) {
            auto tokens = tokenize(line);
            if (!tokens.empty() && tokens[0].type == TokenType::NUMBER) {
                int lineNum = std::stoi(tokens[0].value);
                program[lineNum] = line;
                lineNumbers[lineNum] = executionOrder.size();
                executionOrder.push_back(lineNum);
            }
        }
        
        std::sort(executionOrder.begin(), executionOrder.end());
    }

    void loadProgramFromFile(const std::string& filename) {
        program.clear();
        lineNumbers.clear();
        executionOrder.clear();
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            
            if (!line.empty()) {
                auto tokens = tokenize(line);
                if (!tokens.empty() && tokens[0].type == TokenType::NUMBER) {
                    int lineNum = std::stoi(tokens[0].value);
                    program[lineNum] = line;
                    lineNumbers[lineNum] = executionOrder.size();
                    executionOrder.push_back(lineNum);
                }
            }
        }
        
        file.close();
        std::sort(executionOrder.begin(), executionOrder.end());
        std::cout << "Loaded " << executionOrder.size() << " lines from " << filename << std::endl;
    }

    void listProgram() {
        std::cout << "\nProgram Listing:\n";
        for (int lineNum : executionOrder) {
            std::cout << program[lineNum] << std::endl;
        }
        std::cout << std::endl;
    }

    void run() {
        if (executionOrder.empty()) {
            std::cout << "No program loaded." << std::endl;
            return;
        }

        currentLineIndex = 0;
        running = true;
        
        while (running && currentLineIndex < executionOrder.size()) {
            int lineNum = executionOrder[currentLineIndex];
            executeLine(program[lineNum]);
            currentLineIndex++;
        }
    }

    void runInteractive() {
        std::cout << "BASIC Interpreter - Interactive Mode\n";
        std::cout << "Type 'LOAD filename' to load a program, 'LIST' to show program, 'RUN' to execute, 'QUIT' to exit\n";
        
        std::string command;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, command);
            
            if (command == "QUIT" || command == "quit") {
                break;
            } else if (command.substr(0, 4) == "LOAD" || command.substr(0, 4) == "load") {
                if (command.length() > 5) {
                    std::string filename = command.substr(5);
                    loadProgramFromFile(filename);
                } else {
                    std::cout << "Usage: LOAD filename\n";
                }
            } else if (command == "LIST" || command == "list") {
                listProgram();
            } else if (command == "RUN" || command == "run") {
                run();
            } else if (command == "CLEAR" || command == "clear") {
                program.clear();
                variables.clear();
                arrays.clear();
                lineNumbers.clear();
                executionOrder.clear();
                std::cout << "Program cleared.\n";
            } else {
                std::cout << "Unknown command. Available: LOAD, LIST, RUN, CLEAR, QUIT\n";
            }
        }
    }

private:
    // Check if a number is prime
    bool isPrime(int n) {
        for (int i = 3; i <= n / i; i += 2) {
            if ((n % i) == 0)
                return false;
        }
        return true;
    }

    std::vector<Token> tokenize(const std::string& line) {
        std::vector<Token> tokens;
        bool firstToken = true;

        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            if (std::isspace(c)) {
                continue;
            }
            
            if (std::isdigit(c) && firstToken) {
                std::string num;
                while (i < line.length() && std::isdigit(line[i])) {
                    num += line[i++];
                }
                i--;
                tokens.push_back({TokenType::NUMBER, num, 0});
                firstToken = false;
            }
            else if (c == '#') {
                // Prime check operator
                tokens.push_back({TokenType::OPERATOR, "#", 0});
            }
            else if (std::isalpha(c)) {
                std::string ident;
                while (i < line.length() && (std::isalnum(line[i]) || line[i] == '$')) {
                    ident += line[i++];
                }
                i--;
                
                if (isKeyword(ident)) {
                    tokens.push_back({TokenType::KEYWORD, ident, 0});
                } else {
                    tokens.push_back({TokenType::IDENTIFIER, ident, 0});
                }
            }
            else if (c == '"') {
                std::string str;
                i++;
                while (i < line.length() && line[i] != '"') {
                    str += line[i++];
                }
                tokens.push_back({TokenType::STRING, str, 0});
            }
            else if (std::isdigit(c)) {
                std::string num;
                while (i < line.length() && (std::isdigit(line[i]) || line[i] == '.')) {
                    num += line[i++];
                }
                i--;
                tokens.push_back({TokenType::NUMBER, num, 0});
            }
            else if (c == '<' && i + 1 < line.length() && line[i + 1] == '>') {
                tokens.push_back({TokenType::OPERATOR, "<>", 0});
                i++;
            }
            else if (c == '<' && i + 1 < line.length() && line[i + 1] == '=') {
                tokens.push_back({TokenType::OPERATOR, "<=", 0});
                i++;
            }
            else if (c == '>' && i + 1 < line.length() && line[i + 1] == '=') {
                tokens.push_back({TokenType::OPERATOR, ">=", 0});
                i++;
            }
            else {
                tokens.push_back({TokenType::OPERATOR, std::string(1, c), 0});
            }
        }
        
        return tokens;
    }

    bool isKeyword(const std::string& word) {
        static const std::vector<std::string> keywords = {
            "PRINT", "LET", "IF", "THEN", "GOTO", "INPUT", "END", "REM", "DIM"
        };
        
        std::string upperWord = word;
        std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);
        
        return std::find(keywords.begin(), keywords.end(), upperWord) != keywords.end();
    }

    void executeLine(const std::string& line) {
        auto tokens = tokenize(line);
        if (tokens.empty()) return;

        size_t tokenIndex = 1;
        if (tokenIndex >= tokens.size()) return;

        std::string keyword = tokens[tokenIndex].value;
        std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::toupper);

        if (keyword == "PRINT") {
            executePrint(tokens, tokenIndex + 1);
        }
        else if (keyword == "LET") {
            executeLet(tokens, tokenIndex + 1);
        }
        else if (keyword == "IF") {
            executeIf(tokens, tokenIndex + 1);
        }
        else if (keyword == "GOTO") {
            executeGoto(tokens, tokenIndex + 1);
        }
        else if (keyword == "INPUT") {
            executeInput(tokens, tokenIndex + 1);
        }
        else if (keyword == "DIM") {
            executeDim(tokens, tokenIndex + 1);
        }
        else if (keyword == "END") {
            running = false;
        }
        else if (keyword == "REM") {
            return;
        }
    }

    int evaluateExpression(const std::vector<Token>& tokens, size_t startIndex, size_t endIndex) {
        if (startIndex >= endIndex || startIndex >= tokens.size()) return 0;
        
        // Handle array access and prime check in expressions
        std::vector<Token> processed;
        
        for (size_t i = startIndex; i < endIndex && i < tokens.size(); ++i) {
            if (tokens[i].value == "#" && i + 1 < endIndex) {
                // Prime check: #variable or #expression
                size_t exprEnd = i + 1;
                
                // If next token is an identifier or number, just take that
                if (tokens[i + 1].type == TokenType::IDENTIFIER || 
                    tokens[i + 1].type == TokenType::NUMBER) {
                    
                    int value = getTokenValue(tokens[i + 1]);
                    int result = isPrime(value) ? 1 : 0;
                    processed.push_back({TokenType::NUMBER, std::to_string(result), 0});
                    i++; // Skip the next token as we've processed it
                }
            }
            else if (tokens[i].type == TokenType::IDENTIFIER && 
                i + 1 < endIndex && tokens[i + 1].value == "[") {
                // This is an array access
                std::string arrayName = tokens[i].value;
                size_t closeBracket = i + 2;
                int bracketDepth = 1;
                
                while (closeBracket < endIndex && bracketDepth > 0) {
                    if (tokens[closeBracket].value == "[") bracketDepth++;
                    else if (tokens[closeBracket].value == "]") bracketDepth--;
                    closeBracket++;
                }
                
                // Evaluate the index expression
                int index = evaluateExpression(tokens, i + 2, closeBracket - 1);
                
                // Get the array value
                int value = 0;
                if (arrays.find(arrayName) != arrays.end()) {
                    if (index >= 0 && index < static_cast<int>(arrays[arrayName].size())) {
                        value = arrays[arrayName][index];
                    }
                }
                
                // Replace array access with its value
                processed.push_back({TokenType::NUMBER, std::to_string(value), 0});
                i = closeBracket - 1;
            } else {
                processed.push_back(tokens[i]);
            }
        }
        
        // Process * and / first
        for (size_t i = 1; i + 1 < processed.size(); ) {
            if (processed[i].value == "*" || processed[i].value == "/") {
                int left = getTokenValue(processed[i - 1]);
                int right = getTokenValue(processed[i + 1]);
                int result;
                
                if (processed[i].value == "*") {
                    result = left * right;
                } else {
                    result = (right != 0) ? left / right : 0;
                }
                
                processed[i - 1] = {TokenType::NUMBER, std::to_string(result), 0};
                processed.erase(processed.begin() + i, processed.begin() + i + 2);
            } else {
                i += 2;
            }
        }
        
        // Process + and -
        if (processed.empty()) return 0;
        
        int result = getTokenValue(processed[0]);
        
        for (size_t i = 1; i + 1 < processed.size(); i += 2) {
            std::string op = processed[i].value;
            int rightValue = getTokenValue(processed[i + 1]);
            
            if (op == "+") result += rightValue;
            else if (op == "-") result -= rightValue;
        }
        
        return result;
    }

    int getTokenValue(const Token& token) {
        if (token.type == TokenType::NUMBER) {
            return std::stoi(token.value);
        }
        else if (token.type == TokenType::IDENTIFIER) {
            if (variables.find(token.value) != variables.end()) {
                return variables[token.value];
            }
        }
        return 0;
    }

void executePrint(const std::vector<Token>& tokens, size_t startIndex) {
    std::string output;
    size_t i = startIndex;
    bool hasSemicolon = false;
    
    while (i < tokens.size()) {
        if (tokens[i].type == TokenType::STRING) {
            output += tokens[i].value;
            i++;
        }
        else if (tokens[i].value == ";") {
            hasSemicolon = true;
            i++;
            continue;
        }
        else {
            // Find the end of the expression (until semicolon or end)
            size_t exprEnd = i;
            int parenDepth = 0;
            int bracketDepth = 0;
            
            while (exprEnd < tokens.size()) {
                if (tokens[exprEnd].value == "(") parenDepth++;
                else if (tokens[exprEnd].value == ")") parenDepth--;
                else if (tokens[exprEnd].value == "[") bracketDepth++;
                else if (tokens[exprEnd].value == "]") bracketDepth--;
                else if (tokens[exprEnd].value == ";" && parenDepth == 0 && bracketDepth == 0) break;
                
                exprEnd++;
            }
            
            // Evaluate the expression
            int result = evaluateExpression(tokens, i, exprEnd);
            output += std::to_string(result);
            
            i = exprEnd;
        }
    }
    
    // Check if the last token was a semicolon
    if (!tokens.empty() && tokens.back().value == ";") {
        hasSemicolon = true;
    }
    
    std::cout << output;
    if (!hasSemicolon) {
        std::cout << std::endl;
    }
}
    void executePrint(const std::vector<Token>& tokens, size_t startIndex) {
        std::string output;
        size_t i = startIndex;
        
        while (i < tokens.size()) {
            if (tokens[i].type == TokenType::STRING) {
                output += tokens[i].value;
                i++;
            }
            else if (tokens[i].value == ";") {
                i++;
                continue;
            }
            else {
                // Find the end of the expression (until semicolon or end)
                size_t exprEnd = i;
                int parenDepth = 0;
                int bracketDepth = 0;
                
                while (exprEnd < tokens.size()) {
                    if (tokens[exprEnd].value == "(") parenDepth++;
                    else if (tokens[exprEnd].value == ")") parenDepth--;
                    else if (tokens[exprEnd].value == "[") bracketDepth++;
                    else if (tokens[exprEnd].value == "]") bracketDepth--;
                    else if (tokens[exprEnd].value == ";" && parenDepth == 0 && bracketDepth == 0) break;
                    
                    exprEnd++;
                }
                
                // Evaluate the expression
                int result = evaluateExpression(tokens, i, exprEnd);
                output += std::to_string(result);
                
                i = exprEnd;
            }
        }
        
        std::cout << output << std::endl;
    }

    void executeLet(const std::vector<Token>& tokens, size_t startIndex) {
        if (startIndex + 2 >= tokens.size()) return;
        
        std::string varName = tokens[startIndex].value;
        
        // Check if it's an array assignment
        if (startIndex + 1 < tokens.size() && tokens[startIndex + 1].value == "[") {
            // Array assignment: LET A[index] = value
            size_t closeBracket = startIndex + 2;
            int bracketDepth = 1;
            
            while (closeBracket < tokens.size() && bracketDepth > 0) {
                if (tokens[closeBracket].value == "[") bracketDepth++;
                else if (tokens[closeBracket].value == "]") bracketDepth--;
                closeBracket++;
            }
            
            if (closeBracket > tokens.size()) return;
            
            int index = evaluateExpression(tokens, startIndex + 2, closeBracket - 1);
            
            if (closeBracket >= tokens.size() || tokens[closeBracket].value != "=") return;
            
            int value = evaluateExpression(tokens, closeBracket + 1, tokens.size());
            
            if (arrays.find(varName) != arrays.end()) {
                if (index >= 0 && index < static_cast<int>(arrays[varName].size())) {
                    arrays[varName][index] = value;
                }
            }
        } else {
            // Regular variable assignment
            if (tokens[startIndex + 1].value != "=") return;
            
            int value = evaluateExpression(tokens, startIndex + 2, tokens.size());
            variables[varName] = value;
        }
    }

    void executeDim(const std::vector<Token>& tokens, size_t startIndex) {
        if (startIndex + 3 >= tokens.size()) return;
        
        std::string arrayName = tokens[startIndex].value;
        
        if (tokens[startIndex + 1].value != "[") return;
        
        size_t closeBracket = startIndex + 2;
        while (closeBracket < tokens.size() && tokens[closeBracket].value != "]") {
            closeBracket++;
        }
        
        if (closeBracket >= tokens.size()) return;
        
        int size = evaluateExpression(tokens, startIndex + 2, closeBracket);
        
        if (size > 0) {
            arrays[arrayName] = std::vector<int>(size, 0);
        }
    }

    void executeIf(const std::vector<Token>& tokens, size_t startIndex) {
        if (startIndex + 4 >= tokens.size()) return;
        
        // Find THEN keyword
        size_t thenIndex = startIndex;
        while (thenIndex < tokens.size()) {
            std::string val = tokens[thenIndex].value;
            std::transform(val.begin(), val.end(), val.begin(), ::toupper);
            if (val == "THEN") break;
            thenIndex++;
        }
        
        if (thenIndex >= tokens.size()) return;
        
        // Check for # operator (prime check) in condition
        bool hasPrimeCheck = false;
        size_t primeCheckIndex = 0;
        for (size_t i = startIndex; i < thenIndex; ++i) {
            if (tokens[i].value == "#") {
                hasPrimeCheck = true;
                primeCheckIndex = i;
                break;
            }
        }
        
        bool condition = false;
        
        if (hasPrimeCheck) {
            // Evaluate: IF #variable THEN
            if (primeCheckIndex + 1 < thenIndex) {
                int value = getTokenValue(tokens[primeCheckIndex + 1]);
                condition = isPrime(value);
            }
        } else {
            // Regular comparison condition
            int left = 0;
            std::string op;
            int right = 0;
            
            // Find the comparison operator
            size_t opIndex = startIndex;
            while (opIndex < thenIndex) {
                if (tokens[opIndex].value == "=" || tokens[opIndex].value == "<" || 
                    tokens[opIndex].value == ">" || tokens[opIndex].value == "<=" ||
                    tokens[opIndex].value == ">=" || tokens[opIndex].value == "<>") {
                    op = tokens[opIndex].value;
                    break;
                }
                opIndex++;
            }
            
            if (opIndex >= thenIndex) return;
            
            left = evaluateExpression(tokens, startIndex, opIndex);
            right = evaluateExpression(tokens, opIndex + 1, thenIndex);
            
            if (op == "=") condition = (left == right);
            else if (op == "<") condition = (left < right);
            else if (op == ">") condition = (left > right);
            else if (op == "<=") condition = (left <= right);
            else if (op == ">=") condition = (left >= right);
            else if (op == "<>") condition = (left != right);
        }
        
        if (condition && thenIndex + 1 < tokens.size()) {
            if (tokens[thenIndex + 1].type == TokenType::NUMBER) {
                int gotoLine = std::stoi(tokens[thenIndex + 1].value);
                jumpToLine(gotoLine);
            }
        }
    }

    void executeGoto(const std::vector<Token>& tokens, size_t startIndex) {
        if (startIndex < tokens.size() && tokens[startIndex].type == TokenType::NUMBER) {
            int gotoLine = std::stoi(tokens[startIndex].value);
            jumpToLine(gotoLine);
        }
    }

    void executeInput(const std::vector<Token>& tokens, size_t startIndex) {
        if (startIndex < tokens.size() && tokens[startIndex].type == TokenType::IDENTIFIER) {
            std::string varName = tokens[startIndex].value;
            std::cout << "? ";
            
            std::string input;
            std::getline(std::cin, input);
            
            try {
                variables[varName] = std::stoi(input);
            } catch (...) {
                variables[varName] = 0;
            }
        }
    }

    int getValue(const Token& token) {
        if (token.type == TokenType::NUMBER) {
            return std::stoi(token.value);
        }
        else if (token.type == TokenType::IDENTIFIER) {
            if (variables.find(token.value) != variables.end()) {
                return variables[token.value];
            }
        }
        return 0;
    }

    void jumpToLine(int lineNumber) {
        auto it = lineNumbers.find(lineNumber);
        if (it != lineNumbers.end()) {
            currentLineIndex = it->second - 1;
        }
    }
};

int main() {
    BasicInterpreter interpreter;
//    interpreter.runInteractive();
    interpreter.loadProgramFromFile("start.bas");
    interpreter.listProgram();
    interpreter.run();
    return 0;
}
