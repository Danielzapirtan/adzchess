#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <cctype>

class BasicToC {
private:
    std::map<int, std::string> program;
    std::vector<int> lineNumbers;
    std::set<std::string> variables;
    std::set<std::string> arrays;
    std::map<std::string, int> arrayDimensions;
    std::ofstream outFile;
    
    enum class TokenType {
        NUMBER, STRING, IDENTIFIER, OPERATOR, KEYWORD, EOL, UNKNOWN
    };
    
    struct Token {
        TokenType type;
        std::string value;
    };

public:
    BasicToC() {}
    
    void loadProgram(const std::string& filename) {
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
                    lineNumbers.push_back(lineNum);
                }
            }
        }
        
        file.close();
        std::sort(lineNumbers.begin(), lineNumbers.end());
        std::cout << "Loaded " << lineNumbers.size() << " lines from " << filename << std::endl;
    }
    
    void translate(const std::string& outputFilename) {
        outFile.open(outputFilename);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not create output file " << outputFilename << std::endl;
            return;
        }
        
        // First pass: collect all variables and arrays
        collectVariables();
        
        // Generate C code
        generateHeader();
        generateVariableDeclarations();
        generateMainFunction();
        generateLabelsAndCode();
        generateFooter();
        
        outFile.close();
        std::cout << "Translation complete. Output written to " << outputFilename << std::endl;
    }

private:
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
                tokens.push_back({TokenType::NUMBER, num});
                firstToken = false;
            }
            else if (c == '#') {
                tokens.push_back({TokenType::OPERATOR, "#"});
            }
            else if (std::isalpha(c)) {
                std::string ident;
                while (i < line.length() && (std::isalnum(line[i]) || line[i] == '$')) {
                    ident += line[i++];
                }
                i--;
                
                if (isKeyword(ident)) {
                    tokens.push_back({TokenType::KEYWORD, ident});
                } else {
                    tokens.push_back({TokenType::IDENTIFIER, ident});
                }
            }
            else if (c == '"') {
                std::string str;
                i++;
                while (i < line.length() && line[i] != '"') {
                    str += line[i++];
                }
                tokens.push_back({TokenType::STRING, str});
            }
            else if (std::isdigit(c)) {
                std::string num;
                while (i < line.length() && (std::isdigit(line[i]) || line[i] == '.')) {
                    num += line[i++];
                }
                i--;
                tokens.push_back({TokenType::NUMBER, num});
            }
            else if (c == '<' && i + 1 < line.length() && line[i + 1] == '>') {
                tokens.push_back({TokenType::OPERATOR, "<>"});
                i++;
            }
            else if (c == '<' && i + 1 < line.length() && line[i + 1] == '=') {
                tokens.push_back({TokenType::OPERATOR, "<="});
                i++;
            }
            else if (c == '>' && i + 1 < line.length() && line[i + 1] == '=') {
                tokens.push_back({TokenType::OPERATOR, ">="});
                i++;
            }
            else {
                tokens.push_back({TokenType::OPERATOR, std::string(1, c)});
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
    
    void collectVariables() {
        for (int lineNum : lineNumbers) {
            auto tokens = tokenize(program[lineNum]);
            if (tokens.size() < 2) continue;
            
            size_t i = 1;
            std::string keyword = tokens[i].value;
            std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::toupper);
            
            if (keyword == "LET" && i + 1 < tokens.size()) {
                std::string varName = tokens[i + 1].value;
                if (i + 2 < tokens.size() && tokens[i + 2].value == "[") {
                    // Array variable
                    arrays.insert(varName);
                } else {
                    // Regular variable
                    variables.insert(varName);
                }
            }
            else if (keyword == "DIM" && i + 1 < tokens.size()) {
                std::string arrayName = tokens[i + 1].value;
                arrays.insert(arrayName);
                
                // Find array size
                if (i + 2 < tokens.size() && tokens[i + 2].value == "[") {
                    size_t closeBracket = i + 3;
                    while (closeBracket < tokens.size() && tokens[closeBracket].value != "]") {
                        closeBracket++;
                    }
                    if (closeBracket < tokens.size() && i + 3 < tokens.size()) {
                        if (tokens[i + 3].type == TokenType::NUMBER) {
                            arrayDimensions[arrayName] = std::stoi(tokens[i + 3].value);
                        }
                    }
                }
            }
            else if (keyword == "INPUT" && i + 1 < tokens.size()) {
                variables.insert(tokens[i + 1].value);
            }
        }
    }
    
    void generateHeader() {
        outFile << "#include <stdio.h>\n";
        outFile << "#include <stdlib.h>\n";
        outFile << "#include <string.h>\n";
        outFile << "\n";
        outFile << "/* Prime checking function */\n";
        outFile << "int isPrime(int n) {\n";
        outFile << "    if (n <= 1) return 0;\n";
        outFile << "    if (n == 2) return 1;\n";
        outFile << "    if (n % 2 == 0) return 0;\n";
        outFile << "    for (int i = 3; i * i <= n; i += 2) {\n";
        outFile << "        if (n % i == 0) return 0;\n";
        outFile << "    }\n";
        outFile << "    return 1;\n";
        outFile << "}\n\n";
    }
    
    void generateVariableDeclarations() {
        outFile << "/* Global variables */\n";
        for (const auto& var : variables) {
            outFile << "int " << var << " = 0;\n";
        }
        for (const auto& arr : arrays) {
            if (arrayDimensions.find(arr) != arrayDimensions.end()) {
                outFile << "int " << arr << "[" << arrayDimensions[arr] << "];\n";
            } else {
                outFile << "int " << arr << "[1000];\n";  // Default size
            }
        }
        outFile << "\n";
    }
    
    void generateMainFunction() {
        outFile << "int main() {\n";
        
        // Initialize arrays to zero
        for (const auto& arr : arrays) {
            if (arrayDimensions.find(arr) != arrayDimensions.end()) {
                outFile << "    memset(" << arr << ", 0, sizeof(" << arr << "));\n";
            }
        }
        
        if (!arrays.empty()) {
            outFile << "\n";
        }
    }
    
    void generateLabelsAndCode() {
        for (int lineNum : lineNumbers) {
            outFile << "L" << lineNum << ":\n";
            translateLine(program[lineNum]);
        }
    }
    
    void translateLine(const std::string& line) {
        auto tokens = tokenize(line);
        if (tokens.size() < 2) return;
        
        size_t i = 1;
        std::string keyword = tokens[i].value;
        std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::toupper);
        
        if (keyword == "PRINT") {
            translatePrint(tokens, i + 1);
        }
        else if (keyword == "LET") {
            translateLet(tokens, i + 1);
        }
        else if (keyword == "IF") {
            translateIf(tokens, i + 1);
        }
        else if (keyword == "GOTO") {
            translateGoto(tokens, i + 1);
        }
        else if (keyword == "INPUT") {
            translateInput(tokens, i + 1);
        }
        else if (keyword == "DIM") {
            // DIM is handled in variable declarations, skip here
        }
        else if (keyword == "END") {
            outFile << "    return 0;\n";
        }
        else if (keyword == "REM") {
            outFile << "    /* ";
            for (size_t j = i + 1; j < tokens.size(); j++) {
                outFile << tokens[j].value << " ";
            }
            outFile << "*/\n";
        }
    }
    
    void translatePrint(const std::vector<Token>& tokens, size_t startIndex) {
        size_t i = startIndex;
        
        while (i < tokens.size()) {
            if (tokens[i].type == TokenType::STRING) {
                outFile << "    printf(\"%s\", \"" << tokens[i].value << "\");\n";
                i++;
            }
            else if (tokens[i].value == ";") {
                i++;
                continue;
            }
            else {
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
                
                outFile << "    printf(\"%d\", " << translateExpression(tokens, i, exprEnd) << ");\n";
                i = exprEnd;
            }
        }
        
        bool endsWithSemicolon = !tokens.empty() && tokens.back().value == ";";
        if (!endsWithSemicolon) {
            outFile << "    printf(\"\\n\");\n";
        }
    }
    
    void translateLet(const std::vector<Token>& tokens, size_t startIndex) {
        if (startIndex + 2 >= tokens.size()) return;
        
        std::string varName = tokens[startIndex].value;
        
        if (startIndex + 1 < tokens.size() && tokens[startIndex + 1].value == "[") {
            // Array assignment
            size_t closeBracket = startIndex + 2;
            int bracketDepth = 1;
            
            while (closeBracket < tokens.size() && bracketDepth > 0) {
                if (tokens[closeBracket].value == "[") bracketDepth++;
                else if (tokens[closeBracket].value == "]") bracketDepth--;
                closeBracket++;
            }
            
            if (closeBracket >= tokens.size() || tokens[closeBracket].value != "=") return;
            
            std::string index = translateExpression(tokens, startIndex + 2, closeBracket - 1);
            std::string value = translateExpression(tokens, closeBracket + 1, tokens.size());
            
            outFile << "    " << varName << "[" << index << "] = " << value << ";\n";
        } else {
            // Regular variable assignment
            if (tokens[startIndex + 1].value != "=") return;
            
            std::string expr = translateExpression(tokens, startIndex + 2, tokens.size());
            outFile << "    " << varName << " = " << expr << ";\n";
        }
    }
    
    void translateIf(const std::vector<Token>& tokens, size_t startIndex) {
        if (startIndex + 4 >= tokens.size()) return;
        
        size_t thenIndex = startIndex;
        while (thenIndex < tokens.size()) {
            std::string val = tokens[thenIndex].value;
            std::transform(val.begin(), val.end(), val.begin(), ::toupper);
            if (val == "THEN") break;
            thenIndex++;
        }
        
        if (thenIndex >= tokens.size()) return;
        
        // Check for prime operator
        bool hasPrimeCheck = false;
        size_t primeCheckIndex = 0;
        for (size_t i = startIndex; i < thenIndex; ++i) {
            if (tokens[i].value == "#") {
                hasPrimeCheck = true;
                primeCheckIndex = i;
                break;
            }
        }
        
        std::string condition;
        
        if (hasPrimeCheck) {
            if (primeCheckIndex + 1 < thenIndex) {
                std::string value = translateExpression(tokens, primeCheckIndex + 1, primeCheckIndex + 2);
                condition = "isPrime(" + value + ")";
            }
        } else {
            // Find comparison operator
            size_t opIndex = startIndex;
            std::string op;
            
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
            
            std::string left = translateExpression(tokens, startIndex, opIndex);
            std::string right = translateExpression(tokens, opIndex + 1, thenIndex);
            
            std::string cOp = op;
            if (op == "=") cOp = "==";
            else if (op == "<>") cOp = "!=";
            
            condition = left + " " + cOp + " " + right;
        }
        
        if (thenIndex + 1 < tokens.size() && tokens[thenIndex + 1].type == TokenType::NUMBER) {
            outFile << "    if (" << condition << ") goto L" << tokens[thenIndex + 1].value << ";\n";
        }
    }
    
    void translateGoto(const std::vector<Token>& tokens, size_t startIndex) {
        if (startIndex < tokens.size() && tokens[startIndex].type == TokenType::NUMBER) {
            outFile << "    goto L" << tokens[startIndex].value << ";\n";
        }
    }
    
    void translateInput(const std::vector<Token>& tokens, size_t startIndex) {
        if (startIndex < tokens.size() && tokens[startIndex].type == TokenType::IDENTIFIER) {
            std::string varName = tokens[startIndex].value;
            outFile << "    printf(\"? \");\n";
            outFile << "    scanf(\"%d\", &" << varName << ");\n";
        }
    }
    
    std::string translateExpression(const std::vector<Token>& tokens, size_t startIndex, size_t endIndex) {
        if (startIndex >= endIndex || startIndex >= tokens.size()) return "0";
        
        std::stringstream result;
        
        for (size_t i = startIndex; i < endIndex && i < tokens.size(); ++i) {
            if (tokens[i].value == "#" && i + 1 < endIndex) {
                // Prime check
                std::string value;
                if (tokens[i + 1].type == TokenType::IDENTIFIER || 
                    tokens[i + 1].type == TokenType::NUMBER) {
                    value = tokens[i + 1].value;
                    i++;
                }
                result << "isPrime(" << value << ")";
            }
            else if (tokens[i].type == TokenType::IDENTIFIER && 
                     i + 1 < endIndex && tokens[i + 1].value == "[") {
                // Array access
                std::string arrayName = tokens[i].value;
                size_t closeBracket = i + 2;
                int bracketDepth = 1;
                
                while (closeBracket < endIndex && bracketDepth > 0) {
                    if (tokens[closeBracket].value == "[") bracketDepth++;
                    else if (tokens[closeBracket].value == "]") bracketDepth--;
                    closeBracket++;
                }
                
                std::string index = translateExpression(tokens, i + 2, closeBracket - 1);
                result << arrayName << "[" << index << "]";
                i = closeBracket - 1;
            }
            else if (tokens[i].type == TokenType::NUMBER) {
                result << tokens[i].value;
            }
            else if (tokens[i].type == TokenType::IDENTIFIER) {
                result << tokens[i].value;
            }
            else if (tokens[i].value == "=") {
                result << "==";
            }
            else if (tokens[i].value == "<>") {
                result << "!=";
            }
            else {
                result << tokens[i].value;
            }
            
            if (i + 1 < endIndex && tokens[i + 1].value != "[" && tokens[i + 1].value != "]") {
                result << " ";
            }
        }
        
        return result.str();
    }
    
    void generateFooter() {
        outFile << "    return 0;\n";
        outFile << "}\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.bas> [output.c]\n";
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc >= 3) ? argv[2] : "output.c";
    
    BasicToC translator;
    translator.loadProgram(inputFile);
    translator.translate(outputFile);
    
    return 0;
}