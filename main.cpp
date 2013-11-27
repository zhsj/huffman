/*
 * Author
 * SJ Zhu @ USTC
 * PB12000320
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <bitset>
#include <queue>
#define ERROR 0
#define OK 1

using namespace std;

typedef struct HTNode
{
    int weight;
    short int parent, lchild, rchild;
} HTNode, * HuffmanTree;

typedef char * * HuffmanCode;

int Select(HuffmanTree &HT, int n, int &s1, int &s2);//选出哈夫曼树中权值最小的两个节点
void HuffmanTreeCreate(HuffmanTree &HT, int * w, int n);//建立哈夫曼树
void HuffmanCoding(HuffmanTree &HT, HuffmanCode &HC);//建立每个字符的哈夫曼编码
void FileCoding(const char * input, const char * output);//对文件进行编码
void FileDecoding(const char * input, const char * output);//对文件进行解码

int main(int argc, char **argv)
{
    if(argc == 4)
    {
        char * op, * in, * out;
        op = argv[1];
        in = argv[2];
        out = argv[3];
        if(strcmp(op,"coding") == 0)FileCoding(in,out);
        else if(strcmp(op,"decoding") == 0)FileDecoding(in,out);
        else cout<<"operation error\n";
    }
    else
    {
        FileCoding("int","out");
        FileDecoding("out","ttt");
        //cout<<"Usage: ./Huffman coding[decoding] inputfile outputfile"<<endl;
    }
    return 0;
}


int Select(HuffmanTree &HT, int n, int &s1, int &s2)
{
    int i = 0;

    while(1)
    {
        if(HT[i].weight != 0 && HT[i].parent == 0)
        {
            s1 = i;
            break;
        }
        else i++;
    }//找到第一个权值>0且不在树内的节点
    i++;
    while(1)
    {
        if(HT[i].weight != 0 && HT[i].parent == 0)
        {
            s2 = i;
            break;
        }
        else i++;
    }//找到第二个权值>0且不在树内的节点
    if(HT[s2].weight < HT[s1].weight)
    {
        int temp = s1;
        s1 = s2;
        s2 = temp;
    }//使s1的权值比s2小
    for(i = 0; i <= n; i++)
    {
        if(HT[i].parent == 0 && HT[i].weight != 0 && HT[i].weight < HT[s1].weight)
            s1 = i;
    }//直接遍历所有节点
    for(i = 0; i <= n; i++)
    {
        if(HT[i].parent == 0 && HT[i].weight != 0 && i != s1 && HT[i].weight < HT[s2].weight)
            s2 = i;
    }
    if(HT[s1].parent | HT[s2].parent)
        return ERROR;
    return OK;
}

void HuffmanTreeCreate(HuffmanTree &HT, int * w, int n)
{
    int m = 256 + n - 1;//哈夫曼树节点树，n为文件中的不同字符数，256为所有可能的不同字符数
    int i;
    HuffmanTree p;
    HT = (HuffmanTree)malloc(m * sizeof(HTNode));

    for(p = HT, i = 0; i < 256; i++, p++, w++)//赋初值
        *p = { *w, 0, 0 ,0 };
    for(; i < m; i++, p++)
        *p = { 0, 0, 0, 0 };

    for(i = 256; i < m; i++)
    {
        int s1 = 0 ,s2 = 0;
        Select(HT,i - 1,s1,s2);
        HT[s1].parent = i;
        HT[s2].parent = i;
        HT[i].lchild = s1;
        HT[i].rchild = s2;
        HT[i].weight = HT[s1].weight + HT[s2].weight;
    }//建立关系
}

void HuffmanCoding(HuffmanTree &HT, HuffmanCode &HC)
{
    int n = 256;//一个char可以有256个值，以char的值为数组下标
    HC = (HuffmanCode)malloc(n * sizeof(char *));//每个字符的哈夫曼编码
    char * cd = (char *)malloc(n * sizeof(char));//一个哈夫曼编码的临时存储
    cd[n-1]='\0';
    for(int i = 0; i < n; i++)
    {
        if(HT[i].weight == 0)
        {
            HC[i] = NULL;
            continue;
        }//若该字符权值为0,则不进行编码
        else
        {
            int start = n - 2;
            for(int c = i, f = HT[i].parent; f !=0; c = f, f = HT[f].parent)
            {
                if(HT[f].lchild == c) cd[start] = '0';//左孩子为0
                else cd[start] = '1';//右孩子为1
                start--;
            }//到根节点（没有父节点，f == 0）为止
            HC[i] = (char *)malloc((n-start) * sizeof(char));
            strcpy(HC[i],cd + start+1);//将临时存储的编码拷贝到HC中
        }
    }
    free(cd);//释放临时存储
}


void FileCoding(const char * input, const char * output)
{
    int w[256]= {0};//存储每个字符的权值
    unsigned char remain = 0, c;
    short int sum = 0;
    ifstream fin(input,ios_base::in|ios_base::binary);//以二进制打开
    while(fin.read((char *)&c,sizeof(c)))w[c]++;//存储每个字符的权值
    for(int i = 0; i < 256; i++)if(w[i] > 0)sum++;//不同的字符数
    HuffmanTree HT;
    HuffmanCode HC;
    HuffmanTreeCreate(HT,w,sum);//建立哈夫曼树
    HuffmanCoding(HT,HC);//建立每个出现的字符的哈夫曼编码

    //下面为文件编码

    bitset<8> bits;//每八个二进制存入bits，然后对bits做处理
    queue<unsigned char> buffers;//最后压缩后得到的字符临时存储
    fin.clear();
    fin.seekg(0,ios_base::beg);// 回到文件开始

    int tag = 0;//标志，0-7计数
    while(fin.read((char *)&c, sizeof(c)))
    {
        const char * cd = HC[c];//得到当前字符的哈夫曼编码
        int len = strlen(cd);
        for(int i = 0; i < len; i++)
        {
            bits.set(7 - tag,cd[i] - '0');
            if(tag == 7)
            {
                unsigned char b;
                b = bits.to_ulong();//用char来存储八个二进制表示的信息
                buffers.push(b);//存入临时存储区
                tag = 0;
            }
            else tag++;
        }
    }
    if(tag != 0)//最后剩余tag（< 8)个二进制编码
    {
        remain = tag;//将最后剩余的二进制编码数记录，最后写在文件头
        buffers.push((unsigned char)bits.to_ulong());
    }
    ofstream fout(output,ios_base::out|ios_base::binary);
    fout.write((char *)&sum, sizeof(sum));//文件头，包含原文件字符种数
    fout.write((char *)&remain, sizeof(remain));//哈夫曼编码尾中二进制数剩余不满8个的数目
    fout.write((char *)HT,sizeof(HTNode)*(256 + sum - 1));//哈夫曼树

    while(buffers.size() >= (unsigned int)1024)//1M文件一写
    {
        unsigned char temp[1024];
        for(int i = 0; i < 1024; i++)
        {

            temp[i] = buffers.front();
            buffers.pop();
        }
        fout.write((char *)&temp,sizeof(temp));
    }
    int size = buffers.size();
    unsigned char temp[size];
    for(int i = 0; i < size; i++)
    {
        temp[i] = buffers.front();
        buffers.pop();
    }
    fout.write((char *)&temp,sizeof(temp));

    fin.close();//关闭文件
    fout.close();
}

void FileDecoding(const char * input, const char * output)
{
    ifstream fin(input,ios_base::in|ios_base::binary);
    ofstream fout(output,ios_base::out|ios_base::binary);
    //读取文件头
    short int sum;
    unsigned char remain;
    fin.read((char * )&sum, sizeof(sum));
    fin.read((char *)&remain, sizeof(remain));

    HuffmanTree HT = (HuffmanTree)malloc(sizeof(HTNode)*(256 + sum - 1));
    fin.read((char *)HT, sizeof(HTNode)*(256 + sum - 1));

    unsigned char c;
    bitset<8> bits;
    queue<unsigned char> code;
    queue<char> buffers;

    while(fin.read((char *)&c,sizeof(c)))
    {
        bitset<8> bits(c);
        for(int i = 0; i < 8; i++)
        {
            code.push((unsigned char)bits[7 - i]);
        }
    }
    int count = code.size() - (((int)remain == 0) ? 0 : (8 - (int)remain));
    //count为文件中有效的哈夫曼编码中的二进制数的个数

    int root = 256 + sum - 2;//哈夫曼树的根节点

    for(int i = 0, cd = root; i < count; i++)
    {

        if(code.front() == 0)
            cd = HT[cd].lchild;
        else if(code.front() == 1)
            cd = HT[cd].rchild;
        if(HT[cd].lchild == 0 && HT[cd].rchild == 0)//达到哈夫曼树的叶节点
        {
            buffers.push((char)cd);//将叶节点的值临时存储
            cd = root;
        }
        code.pop();
    }

    while(buffers.size() >= (unsigned int)1024)//1M一写
    {
        char temp[1024];
        for(int i = 0; i < 1024; i++)
        {

            temp[i] = buffers.front();
            buffers.pop();
        }
        fout.write((char *)&temp,sizeof(temp));
    }
    int size = buffers.size();
    char temp[size];
    for(int i = 0; i < size; i++)
    {
        temp[i] = buffers.front();
        buffers.pop();
    }
    fout.write((char *)&temp,sizeof(temp));

    fin.close();//关闭文件
    fout.close();
}
