#ifndef NODE_H
#define NODE_H


class Node
{
public:
    Node(int h, int w);
    int getWidth();
    int getHeight();

private:
    int width;
    int height;
};

#endif // NODE_H
