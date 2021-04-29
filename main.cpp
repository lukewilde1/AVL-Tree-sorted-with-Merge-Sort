/*
*   Luke Wilde: lw102
*   Assignment 1
*/

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

const int poolSize = 500000;
const int wordSize = 50000;
const int sortedSize = 50000;

string getWord(int);
int insert(string, int);
int getHeight(int);
int max(int, int);
int rotateRight(int);
int rotateLeft(int);
int doubleRight(int);
int doubleLeft(int);
void visit(int);
void mergeSort(int, int);
void merge(int, int, int);

struct Word {
    int start;      // start index of a word in pool[]
    int end;        // end index of a word in pool[]
    int count;      // number of times word has been seen
    int left;       // index of left child in text[]
    int right;      // index of right child in text[]
    int height;     // avl height 
    int index;
};

char pool[poolSize];
Word word[wordSize];
Word alphaSorted[wordSize];
int poolIndex = 0;
int wordIndex = 0;
int sortIndex = 0;
int length = 0;

int main() {
    int root = 0;
    bool inWord = false;

    string fileName;
	cerr << "Enter File Name: ";	//for user input
	cin >> fileName;

	ifstream inFile(fileName);

	if (!inFile.is_open()) {		// throws error if file not found
		cerr << "File " << fileName << " does not exist." << endl;
		return 1;
	}
    
    // reading in char by char
    char charIn;
    while(inFile.get(charIn)) {
        if(isspace(charIn) || charIn == '\n' || charIn == '\t') {
            if(inWord == true) {
                word[wordIndex].end = (poolIndex - 1);
                word[wordIndex].count = 1;
                word[wordIndex].index = wordIndex;
                root = insert(getWord(wordIndex), root);
                inWord = false;
                length = 0;
            }
        }
        else if(isalpha(charIn)) {
            if (inWord == false) {
                word[++wordIndex].start = poolIndex;
                inWord = true;
            }
            pool[poolIndex++] = tolower(charIn);
            length++;
        }
    }
    if(inWord == true) {
        word[wordIndex].end = (poolIndex - 1);
        word[wordIndex].count = 1;
        root = insert(getWord(wordIndex), root);
        inWord = false;
        word[wordIndex].end = (poolIndex - 1);
    }
	inFile.close();
    visit(root);

    mergeSort(1, wordIndex);
    
    cout << "first 10" << endl;
    for (int i=1; i<=10; i++) {
        cout << "Word: " << getWord(i) << "     ---      Count: " << word[i].count << endl;
    }
    cout << "last 10" << endl;
    for (int i=9; i>=0; i--) {
        cout << "Word: " << getWord(wordIndex-i) << "     ---      Count: " << word[wordIndex-i].count << endl;
    }


    return 0;
}

string getWord(int index) {
    string temp;
    for(int j=word[index].start; j <= word[index].end; j++) {
           temp = temp + pool[j]; 
    }
    return temp;
}

int insert(string x, int index) {
    if (index == 0) {
        //cout << "word index : " << wordIndex << endl;
        return wordIndex;
    } else if (x == getWord(index)) {
        word[index].count += 1;
        //cout << "equals bruv" << endl;
        wordIndex--;
        poolIndex - length;
        return index;
    } else if (x < getWord(index)) {
        word[index].left = insert(x, word[index].left);
        if ((getHeight(word[index].left) - getHeight(word[index].right)) == 2) {
            if (x < getWord(word[index].left)) {
                // return rotate right (rotateRight(index))
                return rotateRight(index);
            } else {
                // return double right
                return doubleRight(index);
            }
        }
        //cout << "Less than" << endl;
    } else if (x > getWord(index)) {
        word[index].right = insert(x, word[index].right);
        if ((getHeight(word[index].right) - getHeight(word[index].left)) == 2) {
            if (x < getWord(word[index].right)) {
                // return double left
                return doubleLeft(index); 
            } else {
                // return rotate left
                return rotateLeft(index);
            }
        }
        //cout << "Greater than" << endl;
    }
    word[index].height = max(getHeight(word[index].left), getHeight(word[index].right)) + 1;
    return index;
}

int getHeight(int index) {
    if (index == 0) {
        return -1;
    } else {
        return word[index].height;
    } 
}

int max(int a, int b) {
    return (a<b)?b:a;
}

int rotateRight(int index) {
    int x = word[index].left;
    word[index].left = word[x].right;
    word[x].right = index;
    word[index].height = max(getHeight(word[index].left), getHeight(word[index].right)) + 1;
    word[x].height = max(getHeight(word[x].left), getHeight(index)) + 1;
    return x;
}

int rotateLeft(int index) {
    int x = word[index].right;
    word[index].right = word[x].left;
    word[x].left = index;
    word[index].height = max(getHeight(word[index].left), getHeight(word[index].right)) + 1;
    word[x].height = max(getHeight(index), getHeight(word[x].right)) + 1;
    return x;
}

int doubleRight(int index) {
    word[index].left = rotateLeft(word[index].left);
    return rotateRight(index);
}

int doubleLeft(int index) {
    word[index].right = rotateRight(word[index].right);
    return rotateLeft(index);
}

void visit(int index) {
    if (word[index].left != 0) {
        visit(word[index].left);
    }

    alphaSorted[sortIndex].index = index;
    alphaSorted[sortIndex++].count = word[index].count;

    cout <<setw(3) << "Index: " << index << setw(15) << "   Value: " << getWord(index) << "     Count: "<< word[index].count << setw(2) << "    Height: "<< word[index].height;
    cout <<"    Left: " << setw(2) << word[index].left << "    Right: " << setw(2) << word[index].right << endl;

    if (word[index].right != 0) {
        visit(word[index].right);
    }
}

void mergeSort(int start, int top) {
    int mid = (start + top) / 2;

    if (top <= start)
        return;
    mergeSort(start, mid);
    mergeSort(mid + 1, top);
    merge(start, mid, top);
}

void merge(int start, int mid, int top) {
    int x = 1;
    int left = start;
    int right = mid + 1;
    Word sorted[wordSize];

    while (left <= mid && right <= top) {
        if (word[left].count >= word[right].count) {
            sorted[x++] = word[left++];
        } else {
            sorted[x++] = word[right++];
        }
    }
    while (left <= mid) {
        sorted[x++] = word[left++];
    }
    while (right <= top) {
        sorted[x++] = word[right++];
    }
    int y = 1;
    while (start <= top) {
        word[start++] = sorted[y++];
    }
}
