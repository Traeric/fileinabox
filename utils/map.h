/**
 * Author: Eric JIn
 * Date: 2019-03-26
 */
#ifndef MAP_H_
#define MAP_H_

// 定义一个map
typedef struct MAP {
    char *key;
    char *value;
    struct MAP *next;
} map, *p_map;

// 添加元素
p_map addItem(p_map dict, char key[], char value[]);

// 查询元素
p_map findItem(p_map dict, char key[]);

#endif

