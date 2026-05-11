/**
 * 实验：目录树查看器（仿 Linux tree 命令）
 * 学号：______2504020419____  姓名：____李婵______
 * 说明：请补全所有标记为 TODO 的函数体，不要修改其他代码。
 * 目录树查看器（仿 Linux tree 命令）
 * 完整实现版本（C语言，左孩子右兄弟二叉树）
 * 编译：gcc -o tree tree.c -std=c99
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// ================== 二叉树结点定义 ==================
typedef struct FileNode {
    char *name;                  // 文件/目录名
    int isDir;                   // 1:目录 0:文件
    struct FileNode *firstChild; // 左孩子：第一个子项
    struct FileNode *nextSibling;// 右兄弟：下一个同层项
} FileNode;

// ================== 函数声明 ==================
FileNode* createNode(const char *name, int isDir);
int cmpNode(const void *a, const void *b);
FileNode* buildTree(const char *path);
void printTree(FileNode *node, const char *prefix, int isLast);
int countNodes(FileNode *root);
int countLeaves(FileNode *root);
int treeHeight(FileNode *root);
void countDirFile(FileNode *root, int *dirs, int *files);
void freeTree(FileNode *root);
char* getBaseName(void);

// ================== 需要补全的函数 ==================

// 创建新结点（分配内存、复制字符串、初始化指针）
FileNode* createNode(const char *name, int isDir) {
    FileNode *node = (FileNode*)malloc(sizeof(FileNode));
    if (!node) return NULL;
    
    node->name = (char*)malloc(strlen(name) + 1);
    if (!node->name) {
        free(node);
        return NULL;
    }
    strcpy(node->name, name);
    
    node->isDir = isDir;
    node->firstChild = NULL;
    node->nextSibling = NULL;
    return node;
    // TODO: 实现
    return NULL;
}

// 比较函数，用于 qsort 对子项按名称排序
int cmpNode(const void *a, const void *b) {
    FileNode *nodeA = *(FileNode**)a;
    FileNode *nodeB = *(FileNode**)b;
    return strcmp(nodeA->name, nodeB->name);
    // TODO: 实现
    return 0;
}

// 递归构建目录树（核心难点）
FileNode* buildTree(const char *path) {
    // TODO: 实现
    // 步骤提示：
    // 1. opendir 打开目录，失败返回 NULL
    // 2. 从 path 中提取最后的目录名作为当前结点名（注意处理根目录"/"）
    // 3. 创建当前目录结点
    // 4. 循环 readdir，跳过 "." 和 ".."
    // 5. 拼接完整路径，用 stat 判断类型
    // 6. 若是目录，递归调用 buildTree；若是普通文件，调用 createNode
    // 7. 将得到的子结点存入临时数组
    // 8. 关闭目录
    // 9. 对子结点数组排序（调用 qsort 和 cmpNode）
    // 10. 将排序后的子结点链接成兄弟链表（firstChild 指向第一个，后续 nextSibling）
    // 11. 释放临时数组，返回当前目录结点
    DIR *dir = opendir(path);
    if (!dir) return NULL;
    
    // 提取路径中的最后一部分作为目录名
    const char *lastSlash = strrchr(path, '/');
    const char *dirName = (lastSlash && *(lastSlash + 1)) ? lastSlash + 1 : path;
    
    // 创建当前目录结点
    FileNode *curDir = createNode(dirName, 1);
    if (!curDir) {
        closedir(dir);
        return NULL;
    }
    
    // 用于存储子结点的临时数组
    FileNode **children = NULL;
    int childCount = 0;
    struct dirent *entry;
    
    // 遍历目录
    while ((entry = readdir(dir)) != NULL) {
        // 跳过 "." 和 ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        // 构造完整路径
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
        
        // 获取文件状态
        struct stat st;
        if (stat(fullPath, &st) != 0) continue;
        
        FileNode *child = NULL;
        if (S_ISDIR(st.st_mode)) {
            // 目录：递归构建
            child = buildTree(fullPath);
        } else if (S_ISREG(st.st_mode)) {
            // 普通文件：创建结点
            child = createNode(entry->d_name, 0);
        }
        
        if (child) {
            // 扩展临时数组
            FileNode **newChildren = realloc(children, (childCount + 1) * sizeof(FileNode*));
            if (newChildren) {
                children = newChildren;
                children[childCount++] = child;
            } else {
                freeTree(child);
            }
        }
    }
    
    closedir(dir);
    
    // 对子结点排序
    if (childCount > 0) {
        qsort(children, childCount, sizeof(FileNode*), cmpNode);
        
        // 链接成兄弟链表
        curDir->firstChild = children[0];
        for (int i = 0; i < childCount - 1; i++) {
            children[i]->nextSibling = children[i + 1];
        }
        if (childCount > 0) {
            children[childCount - 1]->nextSibling = NULL;
        }
    }
    
    free(children);
    return curDir;
    return NULL;
}

// 树形输出（仿 tree 命令）
void printTree(FileNode *node, const char *prefix, int isLast) {
    // TODO: 实现
    // 步骤提示：
    // 1. 如果 node 为空，返回
    // 2. 输出前缀、分支符号（isLast ? "`-- " : "|-- "）、结点名
    // 3. 如果是目录，输出 "/"
    // 4. 换行
    // 5. 如果没有孩子，返回
    // 6. 遍历孩子链表，对每个孩子：
    //     计算新前缀 = prefix + (isLast ? "    " : "|   ")
    //     判断是否为最后一个孩子
    //     递归调用 printTree
    if (!node) return;
    
    // 输出前缀和分支符号
    printf("%s", prefix);
    printf(isLast ? "`-- " : "|-- ");
    
    // 输出结点名
    printf("%s", node->name);
    if (node->isDir) printf("/");
    printf("\n");
    
    // 处理孩子结点
    if (node->firstChild) {
        // 构建新前缀
        char newPrefix[1024];
        snprintf(newPrefix, sizeof(newPrefix), "%s%s", prefix, isLast ? "    " : "|   ");
        
        // 遍历孩子链表
        FileNode *child = node->firstChild;
        int childIdx = 0;
        int totalChildren = 0;
        FileNode *tmp = child;
        while (tmp) {
            totalChildren++;
            tmp = tmp->nextSibling;
        }
        
        while (child) {
            int isLastChild = (++childIdx == totalChildren);
            printTree(child, newPrefix, isLastChild);
            child = child->nextSibling;
        }
    }
}

// 统计二叉树结点总数
int countNodes(FileNode *root) {
    // TODO: 实现（递归）
    if (!root) return 0;
    // 结点总数 = 1 + 左子树结点数 + 右子树结点数
    return 1 + countNodes(root->firstChild) + countNodes(root->nextSibling);
    return 0;
}

// 统计叶子结点数（firstChild == NULL 的结点）
int countLeaves(FileNode *root) {
    // TODO: 实现（递归）
    if (!root) return 0;
    // 如果是叶子结点（没有孩子）
    if (!root->firstChild) {
        return 1 + countLeaves(root->nextSibling);
    }
    // 非叶子：统计左子树和右子树的叶子
    return countLeaves(root->firstChild) + countLeaves(root->nextSibling);
    return 0;
}

// 计算二叉树高度（根深度为1，空树高度为0）
int treeHeight(FileNode *root) {
    // TODO: 实现（递归）
    if (!root) return 0;
    
    // 计算左子树高度（孩子树）
    int leftHeight = treeHeight(root->firstChild);
    // 计算右子树高度（兄弟树）
    int rightHeight = treeHeight(root->nextSibling);
    
    // 当前结点的高度 = max(左子树高度, 0) + 1
    // 注意：兄弟树的高度不计入当前结点的高度，只用于比较
    int childHeight = leftHeight + 1;
    
    // 返回 max(当前结点高度, 兄弟树高度)
    return childHeight > rightHeight ? childHeight : rightHeight;
    return 0;
}

// 统计目录数和文件数（遍历整棵树）
void countDirFile(FileNode *root, int *dirs, int *files) {
    // TODO: 实现（递归）
    if (!root) return;
    
    // 统计当前结点
    if (root->isDir) {
        (*dirs)++;
    } else {
        (*files)++;
    }
    
    // 递归统计左子树（孩子）和右子树（兄弟）
    countDirFile(root->firstChild, dirs, files);
    countDirFile(root->nextSibling, dirs, files);
}

// 释放整棵树的内存
void freeTree(FileNode *root) {
    // TODO: 实现（递归释放左右子树，最后释放当前结点）
    if (!root) return;
    
    // 递归释放左子树（孩子）和右子树（兄弟）
    freeTree(root->firstChild);
    freeTree(root->nextSibling);
    
    // 释放当前结点
    free(root->name);
    free(root);
}

// 获取当前工作目录的“基本名称”（用于显示根结点名）
char* getBaseName(void) {
    // TODO: 实现
    // 提示：调用 getcwd(NULL,0) 获取绝对路径，提取最后一个 '/' 之后的部分
    // 注意释放 getcwd 分配的内存
     char *cwd = getcwd(NULL, 0);
    if (!cwd) return NULL;
    
    // 提取最后一个 '/' 之后的部分
    char *lastSlash = strrchr(cwd, '/');
    char *baseName;
    
    if (lastSlash && *(lastSlash + 1)) {
        baseName = strdup(lastSlash + 1);
    } else {
        baseName = strdup(cwd);
    }
    
    free(cwd);
    return baseName;
    return NULL;
}

int main(int argc, char *argv[]) {
    char targetPath[1024];
    if (argc >= 2) {
        strncpy(targetPath, argv[1], sizeof(targetPath)-1);
        targetPath[sizeof(targetPath)-1] = '\0';
    } else {
        if (getcwd(targetPath, sizeof(targetPath)) == NULL) {
            perror("getcwd");
            return 1;
        }
    }

    int len = strlen(targetPath);
    if (len > 0 && targetPath[len-1] == '/')
        targetPath[len-1] = '\0';

    struct stat st;
    if (stat(targetPath, &st) != 0) {
        perror("stat");
        return 1;
    }
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "错误: %s 不是目录\n", targetPath);
        return 1;
    }

    FileNode *root = buildTree(targetPath);
    if (!root) {
        fprintf(stderr, "无法构建目录树\n");
        return 1;
    }

    // 输出根目录名
    char *displayName = NULL;
    if (argc >= 2) {
        displayName = root->name;
    } else {
        displayName = getBaseName();
    }
    printf("%s/\n", displayName);
    if (argc < 2) free(displayName);

    FileNode *child = root->firstChild;
    int childCount = 0;
    FileNode *tmp = child;
    while (tmp) { childCount++; tmp = tmp->nextSibling; }
    int idx = 0;
    while (child) {
        int isLast = (++idx == childCount);
        printTree(child, "", isLast);
        child = child->nextSibling;
    }

    int dirs = 0, files = 0;
    countDirFile(root, &dirs, &files);
    printf("\n%d 个目录, %d 个文件\n", dirs, files);
    printf("二叉树结点总数: %d\n", countNodes(root));
    printf("叶子结点数: %d\n", countLeaves(root));
    printf("树的高度: %d\n", treeHeight(root));

    freeTree(root);
    return 0;
}