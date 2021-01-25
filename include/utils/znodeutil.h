//
// Created by lukas on 1/18/21.
//

#ifndef ZPATH_ZNODEUTIL_H
#define ZPATH_ZNODEUTIL_H

class ZLabel;

#include <ui/nodeview.h>
#include <queue>

using namespace std;

class ZNodeUtil {

    //string graphToString(ZNodeView* node);

public:
    static ZNodeUtil& get(){
        static ZNodeUtil instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    vector<ZNodeView *> stringToGraph(string input) {


        string testString = "3 + 4(10.0014 +2*2)+ max(1,2)";


        vector<string> tokens = getTokens(testString);

        stack<string> operators;
        queue<string> outputs;


        // Shunting-yard implementation
        for (string token : tokens) {
            if (isNumber(token)) {
                outputs.push(token);
            }

        }


        return vector<ZNodeView*>();
    }

    vector<string> getTokens(string input) {
        vector<string> tokens;

        input.erase(std::remove_if(input.begin(), input.end(), ::isspace), input.end());

        char testArray[input.length() + 1];
        strcpy(testArray, input.c_str());
        char *copy = strdup(testArray);

        const char* delim = " ,+-*/^()";
        char* afterSplit = strtok(testArray, delim);
        tokens.emplace_back(string(afterSplit));

        while (afterSplit != nullptr) {
            char d = copy[afterSplit - testArray + strlen(afterSplit)];
            afterSplit = strtok(nullptr, delim);

            if (afterSplit != nullptr) {
                tokens.emplace_back(string(1, d));
                tokens.emplace_back(string(afterSplit));
            }
        }

        return tokens;
    }

    static bool isNumber(const std::string& str) {
        int i = 0, j = str.length() - 1;

        // Handling whitespaces
        while (i < str.length() && str[i] == ' ') {
            i++;
        }
        while (j >= 0 && str[j] == ' ') {
            j--;
        }

        if (i > j) {
            return false;
        }

        if (i == j && !(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }

        if (str[i] != '.' && str[i] != '+'
            && str[i] != '-' && !(str[i] >= '0' && str[i] <= '9')) {
            return false;
        }

        // To check if a '.' or 'e' is found in given
        // string. We use this flag to make sure that
        // either of them appear only once.
        bool flagDotOrE = false;

        for (i; i <= j; i++) {
            // If any of the char does not belong to
            // {digit, +, -, ., e}
            if (str[i] != 'e' && str[i] != '.'
                && str[i] != '+' && str[i] != '-'
                && !(str[i] >= '0' && str[i] <= '9')) {
                return false;
            }

            if (str[i] == '.') {
                // checks if the char 'e' has already
                // occurred before '.' If yes, return 0.
                if (flagDotOrE) {
                    return false;
                }

                // If '.' is the last character.
                if (i + 1 > str.length()) {
                    return false;
                }

                // if '.' is not followed by a digit.
                if (!(str[i + 1] >= '0' && str[i + 1] <= '9')) {
                    return false;
                }
            }

            else if (str[i] == 'e') {
                // set flagDotOrE = 1 when e is encountered.
                flagDotOrE = true;

                // if there is no digit before 'e'.
                if (!(str[i - 1] >= '0' && str[i - 1] <= '9')) {
                    return false;
                }

                // If 'e' is the last Character
                if (i + 1 > str.length()) {
                    return false;
                }

                // if e is not followed either by
                // '+', '-' or a digit
                if (str[i + 1] != '+' && str[i + 1] != '-'
                    && (str[i + 1] >= '0' && str[i] <= '9')) {
                    return false;
                }
            }
        }

        /* If the string skips all above cases, then
        it is numeric*/
        return true;
    }
};


#endif //ZPATH_ZNODEUTIL_H
