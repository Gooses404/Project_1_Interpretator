#pragma once
#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "commands.h"
struct word{

};
struct operatr{



};
struct TreeNode{
    Token token;

    struct TreeNode* left;
    struct TreeNode* right;
};
#endif 
